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

#ifndef SRC_BRIDGE_H_
#define SRC_BRIDGE_H_

#include <QObject>
#include <QVariant>
#include <v8.h>

#include "./callable.h"

using v8::Handle;
using v8::Object;

class Bridge : public QObject {
  Q_OBJECT

 public:
  explicit Bridge(Callable *callable) :
    _callable(callable),
    _nextArgument(NULL) {}

  void setNextArgument(QVariant* nextArgument) { _nextArgument = nextArgument; }
  Q_INVOKABLE QVariant _getNextArgument() {
    if (_nextArgument == NULL) {
      return QVariant();
    }
    auto result = _nextArgument;
    _nextArgument = NULL;
    return *result;
  }
  Q_INVOKABLE QVariant send(QVariant);

 private:
  Callable* _callable;
  QVariant* _nextArgument;
};

#endif  // SRC_BRIDGE_H_
