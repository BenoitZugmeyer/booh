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

#ifndef SRC_BROWSER_H_
#define SRC_BROWSER_H_

#include <node.h>
#include <QWebView>
#include <QMainWindow>

#include "./webpage.h"
#include "./callable.h"

using v8::Handle;
using v8::Local;
using v8::Persistent;
using v8::Function;
using v8::Arguments;
using v8::Object;
using v8::Value;

#define JSMETHOD(_type, _name) \
  static v8::Handle<v8::Value> _name ## _static(const v8::Arguments &args) {\
    v8::HandleScope scope; \
    _type *self = node::ObjectWrap::Unwrap<_type>(args.This()); \
    return scope.Close(self->_name(args)); \
  } \
  v8::Handle<v8::Value> _name(const v8::Arguments &args);

class Browser :
    public node::ObjectWrap,
    public Callable {
 public:
  static void Init(Handle<Object> exports);
  Handle<Value> call(const char* method, Local<Value> argv[]);

 private:
  static Persistent<Function> constructor;
  static Handle<Value> New(const Arguments& args);

  JSMETHOD(Browser, load)
  JSMETHOD(Browser, close)
  JSMETHOD(Browser, screenshot)
  JSMETHOD(Browser, show)
  JSMETHOD(Browser, setSize)
  JSMETHOD(Browser, send)

  WebPage *_webPage;
  QWebView *_webView;
  QMainWindow *_mainWindow;

  Browser();
  ~Browser();
  WebPage *webPage();
  void _open();
  void _close();
  bool isOpen();
  void emitEvent(Local<Object> infos);
};

#endif  // SRC_BROWSER_H_
