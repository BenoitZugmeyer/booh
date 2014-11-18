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

#ifndef SRC_MACROS_H_
#define SRC_MACROS_H_

#include <node/node_internals.h> // For ARRAY_SIZE

#define THROW(type, message) \
  v8::ThrowException(type(v8::String::New(message)));\
  return v8::Undefined();

#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(0, ## __VA_ARGS__, 5, 4, 3, 2, 1, 0)
#define VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, N, ...) N

#define CALL(_fct, _argv) \
    _fct->Call(v8::Context::GetCurrent()->Global(), ARRAY_SIZE(_argv), _argv)

#endif  // SRC_MACROS_H_
