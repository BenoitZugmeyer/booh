#include <node.h>
#include <v8.h>

#include <iostream>
#include <QtWidgets>

#include "framecapture.h"

using namespace v8;

Local<String> FromQString(QString str) {
    return String::New(str.utf16());
}

QString ToQString(Local<String> str) {
    return QString::fromUtf16( *String::Value(str) );
}

QString ToQString(Local<Value> str) {
    return ToQString(str->ToString());
}

Handle<Value> Screenshot(const Arguments& args) {
    HandleScope scope;

    int argc = 0;
    char **argv = NULL;
    QApplication a(argc, argv);
    FrameCapture capture;

    QObject::connect(&capture, SIGNAL(finished()), QApplication::instance(), SLOT(quit()));

    capture.load(
            QUrl::fromUserInput(ToQString(args[0])),
            ToQString(args[1]));

    a.exec();

    return scope.Close(Undefined());
}

void init(Handle<Object> exports) {
    exports->Set(String::NewSymbol("screenshot"),
            FunctionTemplate::New(Screenshot)->GetFunction());
}

NODE_MODULE(booh, init)
