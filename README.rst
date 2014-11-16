====
booh
====

This project is a tiny proof of concept to make screenshots of websites with
nodejs and the QtWebKit webview.

The goal is to be able to make screenshots on a headless server from a rich
scriptable environment.

Example
=======

.. code:: js

    var booh = require('booh');

    var browser = new booh.Browser();
    browser.load('http://github.com');
    browser.on('loadFinished', function () {
        browser.screenshot('github.png');
    });

Requirements
============

* Linux only
* Qt5, Qt5WebKit and Qt development utilities (essentially the `moc` command)

Installation
============

.. code::

    npm install git://github.com/BenoitZugmeyer/booh.git

.. pull-quote::

    To ArchLinux users: you may want to prepend :code:`PYTHON=python2` to this
    command.

Documentation
=============

All this API is in development and may change at any time.

:code:`booh.Browser`
    The main class. Each browser are independent. It implements the node
    `EventEmitter` class.

:code:`booh.Browser` methods
----------------------------

:code:`close()`
    This will free the underlying view. You have to close all the browsers to
    stop the node event loop (and gracefully stop the node process).

:code:`load(url)`
    Load a given URL in the browser.

:code:`screenshot(outputFile)`
    Takes a screenshot and store it as a PNG file.

:code:`setSize(size)`
    Set the size of the browser. Without argument, the browser will be resized
    to fit its current content. Else, it should be an object with optional
    properties `width` and `height` describing a size in pixels. Any missing
    property will be replaced by the current content metric.

    Note: the default size is 1024 x 768.

:code:`show()`
    Show the browser. An interactive window will be opened. You can use this to
    check what's wrong with the website. There is no `hide()` yet.

:code:`setUserAgent(useragent)`
    Set the user agent.


:code:`booh.Browser` events
---------------------------

You can register events with the node `events api`_ methods. Each event will
have one object as argument.

:code:`loadProgress`
    Emitted when a page is being loaded.

    :code:`event.progress` number from 0 to 100: the current load status

:code:`loadFinished`
    Emitted when the page is loaded. This does not handle asynchronous assets
    loading.

    :code:`event.success` boolean: the global success of the page load.

:code:`requestFinished`
    An HTTP request has finished.

    :code:`event.url` string: the URL of the response

    :code:`event.headers` array: all response headers

    :code:`event.request` object: the source request

    :code:`event.request.url` string: the requested URL

    :code:`event.request.headers` array: all request headers


Credits
=======

My C++ and Qt skills are a little rusty. I used this `qt-project example`_ as a
starting point.

The integration of Qt into nodejs is greatly inspired from `node-qt`_.

.. _events api: http://nodejs.org/docs/latest/api/events.html
.. _qt-project example: http://qt-project.org/doc/qt-5/qtwebkitexamples-webkitwidgets-framecapture-example.html
.. _node-qt: https://github.com/arturadib/node-qt
