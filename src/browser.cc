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

#include <QString>
#include <cstdio>

#include "./browser.h"
#include "./macros.h"
#include "./conversion.h"

using v8::Handle;
using v8::Local;
using v8::Persistent;
using v8::Function;
using v8::Arguments;
using v8::Undefined;
using v8::Exception;
using v8::HandleScope;
using v8::FunctionTemplate;
using node::AtExit;
using node::SetPrototypeMethod;

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

#ifdef DEBUG
void atExitChecks(void* args) {
  printf("running browsers: %u\n", runningBrowsers);
  printf("globalApplication is null: %d\n", globalApplication == NULL);
}
#endif

Persistent<Function> Browser::constructor;

void Browser::Init(Handle<Object> exports) {
  // Prepare constructor template
  auto tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("Browser"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  SetPrototypeMethod(tpl, "load", load_static);
  SetPrototypeMethod(tpl, "close", close_static);
  SetPrototypeMethod(tpl, "screenshot", screenshot_static);
  SetPrototypeMethod(tpl, "setSize", setSize_static);
  SetPrototypeMethod(tpl, "show", show_static);
  SetPrototypeMethod(tpl, "send", send_static);

  constructor = Persistent<Function>::New(tpl->GetFunction());
  exports->Set(String::NewSymbol("Browser"), constructor);

#ifdef DEBUG
  AtExit(atExitChecks);
#endif
}

Handle<Value> Browser::New(const Arguments& args) {
  HandleScope scope;

  if (!args.IsConstructCall()) {
    THROW(Exception::TypeError,
        "Please create an instance with 'new Browser(...)'");
  }

  Browser* obj = new Browser();
  obj->Wrap(args.This());
  return args.This();
}

Browser::Browser() : _webPage(NULL) {
}

Browser::~Browser() {
  _close();
}

WebPage* Browser::webPage() {
  _open();
  return _webPage;
}

Handle<Value> Browser::load(const Arguments& args) {
  QUrl url = QUrlFromValue(args[0]);
  webPage()->mainFrame()->load(url);
  return Undefined();
}

Handle<Value> Browser::close(const Arguments& args) {
  _close();
  return Undefined();
}

void Browser::_close() {
  if (!isOpen()) {
    return;
  }

  auto tmp = _webPage;
  _webPage = NULL;
  delete tmp;

  runningBrowsers--;
}

Handle<Value> Browser::setSize(const Arguments& args) {
  QSize size;
  if (args[0]->IsObject()) {
    size = QSizeFromValue(args[0]);
  }

  auto defaultSize = webPage()->mainFrame()->contentsSize();

  if (size.width() <= 0) {
    size.setWidth(defaultSize.width());
  }

  if (size.height() <= 0) {
    size.setHeight(defaultSize.height());
  }

  webPage()->setViewportSize(size);

  globalApplication->processEvents();

  return Undefined();
}

Handle<Value> Browser::screenshot(const Arguments& args) {
  if (!args[0]->IsString()) {
    THROW(Exception::TypeError,
        "Browser#screenshot expects a string as argument");
  }

  QString fileName = QStringFromValue(args[0]);

  auto frame = webPage()->mainFrame();
  auto size = webPage()->viewportSize().boundedTo(frame->contentsSize());

  QImage image(size, QImage::Format_ARGB32_Premultiplied);

  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::TextAntialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  // webPage()->mainFrame()->documentElement().render(&painter);
  webPage()->mainFrame()->render(&painter);
  painter.end();

  image.save(fileName);

  return Undefined();
}

Handle<Value> Browser::show(const Arguments& args) {
  auto page = webPage();

  _mainWindow = new QMainWindow();

  _webView = new QWebView(_mainWindow);
  _webView->setPage(page);

  _mainWindow->setCentralWidget(_webView);
  _mainWindow->show();

  globalApplication->setActiveWindow(_mainWindow);

  return Undefined();
}

Handle<Value> Browser::send(const Arguments& args) {
  auto nextArgument = QVariantFromValue(args[0]);
  webPage()->bridge()->setNextArgument(&nextArgument);
  auto result = webPage()->mainFrame()->evaluateJavaScript(
      "window.booh.receive ? window.booh.receive(booh._getNextArgument()) : undefined");
  return AsValue(result);
}

bool Browser::isOpen() {
  return _webPage != NULL;
}

void Browser::_open() {
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

  _webPage = new WebPage(this, globalApplication);
  _webPage->setViewportSize(QSize(1024, 200));
  _webPage->mainFrame()->setScrollBarPolicy(
      Qt::Vertical,
      Qt::ScrollBarAlwaysOff);
  _webPage->mainFrame()->setScrollBarPolicy(
      Qt::Horizontal,
      Qt::ScrollBarAlwaysOff);

  QObject::connect(_webPage, &QWebPage::loadFinished, [this](bool ok) {
    auto event = Object::New();
    event->Set(AsValue("name"), AsValue("loadFinished"));
    event->Set(AsValue("success"), AsValue(ok));

    this->emitEvent(event);
  });

  QObject::connect(_webPage, &QWebPage::loadProgress, [this](int progress) {
    auto event = Object::New();
    event->Set(AsValue("name"), AsValue("loadProgress"));
    event->Set(AsValue("progress"), AsValue(progress));

    this->emitEvent(event);
  });

  QObject::connect(
      _webPage->networkAccessManager(),
      &QNetworkAccessManager::finished,
      [this](QNetworkReply *reply) {
        auto event = Object::New();
        event->Set(AsValue("name"), AsValue("requestFinished"));
        event->Set(AsValue("request"), AsValue(reply->request()));
        event->Set(AsValue("url"), AsValue(reply->url()));
        event->Set(AsValue("headers"), AsValue(reply->rawHeaderPairs()));

        this->emitEvent(event);
      });
}

void Browser::emitEvent(Local<Object> event) {
  Local<Value> argv[] = { event };
  call("processEvent", argv);
}

Handle<Value> Browser::call(
        const char* method,
        Local<Value> argv[]) {
  auto fn = handle_->Get(AsValue(method));

  if (fn->IsFunction()) {
    return CALL(Local<Function>::Cast(fn), argv);
  }
  return Undefined();
}
