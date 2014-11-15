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


v8::Persistent<v8::Function> Browser::constructor;

void Browser::Init(v8::Handle<v8::Object> exports) {
  // Prepare constructor template
  auto tpl = v8::FunctionTemplate::New(New);
  tpl->SetClassName(v8::String::NewSymbol("Browser"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  node::SetPrototypeMethod(tpl, "load", Load);
  node::SetPrototypeMethod(tpl, "close", Close);
  node::SetPrototypeMethod(tpl, "screenshot", Screenshot);

  constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
  exports->Set(v8::String::NewSymbol("Browser"), constructor);
}

v8::Handle<v8::Value> Browser::New(const v8::Arguments& args) {
  v8::HandleScope scope;

  if (!args.IsConstructCall()) {
    THROW(v8::Exception::TypeError,
        "Please create an instance with 'new Browser(...)'");
  }

  if (!args[0]->IsFunction()) {
    THROW(v8::Exception::TypeError, "First argument should be a function");
  }

  auto processEvent = v8::Persistent<v8::Function>::New(
      v8::Handle<v8::Function>::Cast(args[0]));

  Browser* obj = new Browser(processEvent);
  obj->Wrap(args.This());
  return args.This();
}

Browser::Browser(v8::Persistent<v8::Function> processEvent)
  : _processEvent(processEvent) {
  _webPage = NULL;
}

Browser::~Browser() {
  close();
}

QWebPage* Browser::webPage() {
  open();
  return _webPage;
}

v8::Handle<v8::Value> Browser::Load(const v8::Arguments& args) {
  SELF(Browser);

  v8::HandleScope scope;

  QUrl url = QUrl::fromUserInput(ToQString(args[0]));
  self->webPage()->setViewportSize(QSize(512, 512));
  self->webPage()->mainFrame()->load(url);

  return scope.Close(v8::Undefined());
}

v8::Handle<v8::Value> Browser::Close(const v8::Arguments& args) {
  SELF(Browser);

  v8::HandleScope scope;

  self->close();

  return scope.Close(v8::Undefined());
}

v8::Handle<v8::Value> Browser::Screenshot(const v8::Arguments& args) {
  SELF(Browser);

  v8::HandleScope scope;

  if (!args[0]->IsString()) {
    THROW(v8::Exception::TypeError, "Browser#screenshot expects a string as argument");
  }

  QString fileName = ToQString(args[0]);

  auto frame = self->webPage()->mainFrame();
  auto size = frame->contentsSize();

  if (size.width() == 0 || size.height() == 0) {
    THROW(v8::Exception::Error, "Nothing is painted (yet?)");
  }

  QImage image(size, QImage::Format_ARGB32_Premultiplied);

  image.fill(Qt::transparent);

  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::TextAntialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  frame->documentElement().render(&painter);
  painter.end();

  image.save(fileName);

  return scope.Close(v8::Undefined());
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
    int argc = 0;
    char **argv = NULL;
    globalApplication = new QApplication(argc, argv);

    uv_timer_init(uv_default_loop(), &timer);
    uv_timer_start(&timer, processEvents, 0, 10);
  }
  _webPage = new QWebPage(globalApplication);

  QObject::connect(_webPage, &QWebPage::loadFinished, [this](bool ok) {
    if (!this->isOpen()) {
      return;
    }

    // This fixes some unfinished rendering of the page for some sites
    // (example: http://google.fr)
    globalApplication->processEvents();

    const unsigned argc = 1;
    v8::Local<v8::Value> argv[argc] = {
      v8::Local<v8::Value>::New(v8::String::New("loadFinished"))
    };
    auto global = v8::Context::GetCurrent()->Global();
    this->_processEvent->Call(global, argc, argv);
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
