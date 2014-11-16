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
#include <QString>

#define SELF(type) type *self = ObjectWrap::Unwrap<type>(args.This());

#define THROW(type, message) \
  v8::ThrowException(type(v8::String::New(message)));\
  return scope.Close(v8::Undefined());

#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(0, ## __VA_ARGS__, 5, 4, 3, 2, 1, 0)
#define VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, N, ...) N

#define CALL(_fct, ...) \
    const unsigned argc = VA_NUM_ARGS(__VA_ARGS__); \
    v8::Local<v8::Value> argv[argc] = { __VA_ARGS__ }; \
    _fct->Call(v8::Context::GetCurrent()->Global(), argc, argv)


/*
 * AsValue implementations
 */

inline v8::Local<v8::String> AsValue(QString str) {
  return v8::String::New(str.utf16());
}

inline v8::Local<v8::String> AsValue(const char *str) {
  return v8::String::New(str);
}

inline v8::Local<v8::Number> AsValue(double n) {
  return v8::Number::New(n);
}

inline v8::Local<v8::Number> AsValue(int n) {
  return AsValue(static_cast<double>(n));
}

inline v8::Local<v8::String> AsValue(QUrl url) {
  return AsValue(url.toString());
}

inline v8::Local<v8::Object> AsValue(QSize size) {
  auto result = v8::Object::New();
  result->Set(AsValue("width"), AsValue(size.width()));
  result->Set(AsValue("height"), AsValue(size.height()));
  return result;
}

template<typename T>
inline v8::Local<v8::Array> AsValue(QList<T> list) {
  auto result = v8::Array::New(list.size());
  int index = 0;
  for (T item : list) {
    result->Set(AsValue(index), AsValue(item));
    index++;
  }
  return result;
}

template<typename T1, typename T2>
inline v8::Local<v8::Array> AsValue(QPair<T1, T2> pair) {
  auto result = v8::Array::New(2);
  result->Set(AsValue(0), AsValue(pair.first));
  result->Set(AsValue(1), AsValue(pair.second));
  return result;
}

inline v8::Local<v8::String> AsValue(QByteArray bytes) {
  return AsValue(QString::fromUtf8(bytes));
}

inline v8::Local<v8::Object> AsValue(QNetworkRequest request) {
  auto result = v8::Object::New();
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
    v8::Local<v8::Value> arg,
    QString default_ = QString()) {
  if (arg->IsString()) {
    return QString::fromUtf16(*v8::String::Value(arg));
  } else {
    return default_;
  }
}

inline int IntFromValue(v8::Local<v8::Value> arg, int default_ = 0) {
  if (arg->IsNumber()) {
    return arg->NumberValue();
  } else {
    return default_;
  }
}

inline QSize QSizeFromValue(v8::Local<v8::Value> arg) {
  auto object = v8::Handle<v8::Object>::Cast(arg);
  auto width = object->Get(AsValue("width"));
  auto height = object->Get(AsValue("height"));
  return QSize(IntFromValue(width), IntFromValue(height));
}

inline QUrl QUrlFromValue(v8::Local<v8::Value> arg) {
  return QUrl::fromUserInput(QStringFromValue(arg));
}

/* template<typename T> */
/* inline QList<T> QListFromValue(v8::Local<v8::Value> v) { */
/*   auto arg = v8::Handle<v8::Array>::Cast(v); */
/*   return QList<T>(); */
/* } */

#endif  // SRC_UTIL_H_
