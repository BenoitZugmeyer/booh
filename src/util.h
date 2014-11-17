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

#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <node.h>
#include <node/node_internals.h> // For ARRAY_SIZE
#include <QString>

#define THROW(type, message) \
  v8::ThrowException(type(v8::String::New(message)));\
  return v8::Undefined();

#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(0, ## __VA_ARGS__, 5, 4, 3, 2, 1, 0)
#define VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, N, ...) N

#define CALL(_fct, _argv) \
    _fct->Call(v8::Context::GetCurrent()->Global(), ARRAY_SIZE(_argv), _argv)


using v8::Local;
using v8::Handle;
using v8::Value;
using v8::String;
using v8::Number;
using v8::Object;
using v8::Array;

/*
 * AsValue implementations
 */

inline Local<String> AsValue(QString str) {
  return String::New(str.utf16());
}

inline Local<String> AsValue(const char *str) {
  return String::New(str);
}

inline Local<Number> AsValue(double n) {
  return Number::New(n);
}

inline Local<Number> AsValue(int n) {
  return AsValue(static_cast<double>(n));
}

inline Local<String> AsValue(QUrl url) {
  return AsValue(url.toString());
}

inline Local<Object> AsValue(QSize size) {
  auto result = Object::New();
  result->Set(AsValue("width"), AsValue(size.width()));
  result->Set(AsValue("height"), AsValue(size.height()));
  return result;
}

template<typename T>
inline Local<Array> AsValue(QList<T> list) {
  auto result = Array::New(list.size());
  int index = 0;
  for (T item : list) {
    result->Set(AsValue(index), AsValue(item));
    index++;
  }
  return result;
}

template<typename T1, typename T2>
inline Local<Array> AsValue(QPair<T1, T2> pair) {
  auto result = Array::New(2);
  result->Set(AsValue(0), AsValue(pair.first));
  result->Set(AsValue(1), AsValue(pair.second));
  return result;
}

inline Local<String> AsValue(QByteArray bytes) {
  return AsValue(QString::fromUtf8(bytes));
}

inline Local<Object> AsValue(QNetworkRequest request) {
  auto result = Object::New();
  result->Set(AsValue("url"), AsValue(request.url()));

  auto list = QList<QPair<QByteArray, QByteArray>>();
  for (auto header : request.rawHeaderList()) {
    list.append(QPair<QByteArray, QByteArray>(
          header, request.rawHeader(header)));
  }
  result->Set(AsValue("headers"), AsValue(list));

  return result;
}


/*
 * FromValue implementations
 */

inline QString QStringFromValue(
    Local<Value> arg,
    QString default_ = QString()) {
  if (arg->IsString()) {
    return QString::fromUtf16(*String::Value(arg));
  } else {
    return default_;
  }
}

inline int IntFromValue(Local<Value> arg, int default_ = 0) {
  if (arg->IsNumber()) {
    return arg->NumberValue();
  } else {
    return default_;
  }
}

inline QSize QSizeFromValue(Local<Value> arg) {
  auto object = Handle<Object>::Cast(arg);
  auto width = object->Get(AsValue("width"));
  auto height = object->Get(AsValue("height"));
  return QSize(IntFromValue(width, -1), IntFromValue(height, -1));
}

inline QUrl QUrlFromValue(Local<Value> arg) {
  return QUrl::fromUserInput(QStringFromValue(arg));
}

/* template<typename T> */
/* inline QList<T> QListFromValue(Local<Value> v) { */
/*   auto arg = Handle<Array>::Cast(v); */
/*   return QList<T>(); */
/* } */

#endif  // SRC_UTIL_H_
