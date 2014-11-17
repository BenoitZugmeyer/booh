// Copyright (c) 2014 Benoît Zugmeyer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <cstdio>

#include "./browser.h"
#include "./util.h"

using v8::Handle;
using v8::Local;
using v8::Persistent;
using v8::Function;
using v8::Arguments;
using v8::Undefined;
using v8::Exception;
using v8::HandleScope;
using v8::FunctionTemplate;

int globalApplicationArgc = 0;
char **globalApplicationArgv = NULL;
QApplication *globalApplication = NULL;
uv_timer_t timer;
unsigned int runningBrowsers = 0;

void processEvents(uv_timer_t* handle, int status) {
  if (globalApplication != NULL) {
    if (runningBrowsers > 0) {
      globalApplication->processEvents();
    } else {
      delete globalApplication;
      globalApplication = NULL;
      uv_timer_stop(&timer);
    }
  }
}


Persistent<Function> Browser::constructor;

void Browser::Init(Handle<Object> exports) {
  // Prepare constructor template
  auto tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("Browser"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  node::SetPrototypeMethod(tpl, "load", Load);
  node::SetPrototypeMethod(tpl, "close", Close);
  node::SetPrototypeMethod(tpl, "screenshot", Screenshot);
  node::SetPrototypeMethod(tpl, "setSize", SetSize);
  node::SetPrototypeMethod(tpl, "show", Show);

  constructor = Persistent<Function>::New(tpl->GetFunction());
  exports->Set(String::NewSymbol("Browser"), constructor);
}

Handle<Value> Browser::New(const Arguments& args) {
  HandleScope scope;

  if (!args.IsConstructCall()) {
    THROW(Exception::TypeError,
        "Please create an instance with 'new Browser(...)'");
  }

  if (!args[0]->IsFunction()) {
    THROW(Exception::TypeError,
        "First argument should be a function");
  }

  auto processEvent = Persistent<Function>::New(
      Handle<Function>::Cast(args[0]));

  Browser* obj = new Browser(processEvent);
  obj->Wrap(args.This());
  return args.This();
}

Browser::Browser(Persistent<Function> processEvent)
  : _processEvent(processEvent) {
  _webPage = NULL;
}

Browser::~Browser() {
  close();
}

WebPage* Browser::webPage() {
  open();
  return _webPage;
}

Handle<Value> Browser::Load(const Arguments& args) {
  SELF(Browser);

  HandleScope scope;

  QUrl url = QUrlFromValue(args[0]);
  self->webPage()->mainFrame()->load(url);

  return scope.Close(Undefined());
}

Handle<Value> Browser::Close(const Arguments& args) {
  SELF(Browser);

  HandleScope scope;

  self->close();

  return scope.Close(Undefined());
}

Handle<Value> Browser::SetSize(const Arguments& args) {
  SELF(Browser);

  HandleScope scope;

  QSize size = QSizeFromValue(args[0]);
  auto webPage = self->webPage();
  auto defaultSize = webPage->mainFrame()->contentsSize();
  if (size.width() == 0) {
    size.setWidth(defaultSize.width());
  }
  if (size.height() == 0) {
    size.setHeight(defaultSize.height());
  }

  webPage->setViewportSize(size);

  globalApplication->processEvents();

  return scope.Close(Undefined());
}

Handle<Value> Browser::Screenshot(const Arguments& args) {
  SELF(Browser);

  HandleScope scope;

  if (!args[0]->IsString()) {
    THROW(Exception::TypeError,
        "Browser#screenshot expects a string as argument");
  }

  QString fileName = QStringFromValue(args[0]);

  auto frame = self->webPage()->mainFrame();
  auto size = self->webPage()->viewportSize().boundedTo(frame->contentsSize());

  QImage image(size, QImage::Format_ARGB32_Premultiplied);

  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::TextAntialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  // self->webPage()->mainFrame()->documentElement().render(&painter);
  self->webPage()->mainFrame()->render(&painter);
  painter.end();

  image.save(fileName);

  return scope.Close(Undefined());
}

Handle<Value> Browser::Show(const Arguments& args) {
  SELF(Browser);

  HandleScope scope;

  auto webPage = self->webPage();

  auto window = new QMainWindow();
  globalApplication->setActiveWindow(window);
  auto webView = new QWebView(window);
  window->setCentralWidget(webView);
  webView->setPage(webPage);
  window->show();

  return scope.Close(Undefined());
}

bool Browser::isOpen() {
  return _webPage != NULL;
}

void Browser::open() {
  if (isOpen()) {
    return;
  }

  runningBrowsers++;

  if (globalApplication == NULL) {
    globalApplication = new QApplication(
        globalApplicationArgc,
        globalApplicationArgv);
    uv_timer_init(uv_default_loop(), &timer);
    uv_timer_start(&timer, processEvents, 0, 10);
  }

  _webPage = new WebPage(handle_, globalApplication);
  _webPage->setViewportSize(QSize(1024, 200));
  _webPage->mainFrame()->setScrollBarPolicy(
      Qt::Vertical,
      Qt::ScrollBarAlwaysOff);
  _webPage->mainFrame()->setScrollBarPolicy(
      Qt::Horizontal,
      Qt::ScrollBarAlwaysOff);

  QObject::connect(_webPage, &QWebPage::loadFinished, [this](bool ok) {
    if (!this->isOpen()) {
      return;
    }

    auto event = Object::New();
    event->Set(AsValue("name"), AsValue("loadFinished"));
    event->Set(AsValue("success"), AsValue(ok));

    this->emitEvent(event);
  });

  QObject::connect(_webPage, &QWebPage::loadProgress, [this](int progress) {
    if (!this->isOpen()) {
      return;
    }

    auto event = Object::New();
    event->Set(AsValue("name"), AsValue("loadProgress"));
    event->Set(AsValue("progress"), AsValue(progress));

    this->emitEvent(event);
  });

  QObject::connect(
      _webPage->networkAccessManager(),
      &QNetworkAccessManager::finished,
      [this](QNetworkReply *reply) {
        if (!this->isOpen()) {
          return;
        }

        auto event = Object::New();
        event->Set(AsValue("name"), AsValue("requestFinished"));
        event->Set(AsValue("request"), AsValue(reply->request()));
        event->Set(AsValue("url"), AsValue(reply->url()));
        event->Set(AsValue("headers"), AsValue(reply->rawHeaderPairs()));

        this->emitEvent(event);
      });
}

void Browser::close() {
  if (!isOpen()) {
    return;
  }

  auto tmp = _webPage;
  _webPage = NULL;
  delete tmp;

  runningBrowsers--;
}

void Browser::emitEvent(Local<Object> event) {
  Local<Value> argv[] = { event };
  CALL(this->_processEvent, argv);
}
