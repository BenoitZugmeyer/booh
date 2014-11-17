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


#include "./webpage.h"
#include "./util.h"

using v8::Function;
using v8::Handle;
using v8::Local;
using v8::Object;
using v8::Value;

WebPage::WebPage(Handle<Object> browser, QObject* parent)
  : QWebPage(parent), _browser(browser) {
}

bool WebPage::extension(
    Extension extension,
    const ExtensionOption* option,
    ExtensionReturn *output) {
  // std::cerr << "EXTENSION" << std::endl;
  // if (extension != QWebPage::ErrorPageExtension)
  //   return false;

  // ErrorPageExtensionOption *errorOption = (ErrorPageExtensionOption*) option;
  // std::cerr << "Error loading " << qPrintable(errorOption->url.toString())
  // << std::endl;
  // if(errorOption->domain == QWebPage::QtNetwork)
  //   std::cerr << "Network error (" << errorOption->error << "): ";
  // else if(errorOption->domain == QWebPage::Http)
  //   std::cerr << "HTTP error (" << errorOption->error << "): ";
  // else if(errorOption->domain == QWebPage::WebKit)
  //   std::cerr << "WebKit error (" << errorOption->error << "): ";

  // std::cerr << qPrintable(errorOption->errorString) << std::endl;

  return false;
}

bool WebPage::supportsExtension(Extension extension) const {
  return QWebPage::ErrorPageExtension == extension;
}

QString WebPage::userAgentForUrl(const QUrl& url) const {
  auto fn = _browser->Get(AsValue("userAgentForUrl"));

  if (fn->IsFunction()) {
    Local<Value> argv[] = { AsValue(url) };
    auto value = CALL(Local<Function>::Cast(fn), argv);

    auto result = QStringFromValue(value);
    if (result.size() > 0) {
      return result;
    }
  }

  return QWebPage::userAgentForUrl(url);
}
