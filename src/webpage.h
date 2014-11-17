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

#ifndef SRC_WEBPAGE_H_
#define SRC_WEBPAGE_H_

#include <QtWebKitWidgets>
#include <v8.h>

using v8::Handle;
using v8::Object;

class WebPage : public QWebPage {
  Q_OBJECT

 public:
  explicit WebPage(Handle<Object> browser, QObject* parent = 0);

  bool extension(
      Extension extension,
      const ExtensionOption* option = 0,
      ExtensionReturn *output = 0);

  bool supportsExtension(Extension extension) const;

 protected:
  QString userAgentForUrl(const QUrl& url) const;

 private:
  Handle<Object> _browser;
};

#endif  // SRC_WEBPAGE_H_
