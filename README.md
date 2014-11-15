booh
====

This project is a tiny proof of concept to make screenshots of websites with
nodejs and the QtWebKit webview.

The goal is to be able to make screenshots on a headless server from a rich
scriptable environment.

Example
-------

```js
var booh = require('booh');

booh.screenshot('http://github.com', 'github.png');
```

Requirements
------------

* Linux only
* Qt5, Qt5WebKit and Qt development utilities (essentially the `moc` command)

Installation
------------

```
npm install git://github.com/BenoitZugmeyer/booh.git
```

Credits
-------

My C++ and Qt skills are a little rusty. The main code comes from [this qt-project example](http://qt-project.org/doc/qt-5/qtwebkitexamples-webkitwidgets-framecapture-example.html).

The integration of Qt into nodejs is greatly inspired from [node-qt](https://github.com/arturadib/node-qt).

A usefull page is the [nodejs addons documentation](http://nodejs.org/api/addons.html)