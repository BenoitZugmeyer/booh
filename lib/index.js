var EventEmitter = require('events').EventEmitter;
var bindings;
try {
  bindings = require('../build/Release/booh.node');
} catch (e) {
  bindings = require('../build/Debug/booh.node');
}

function Browser() {
  EventEmitter.call(this);
  var this_ = this;
  this._bindings = new bindings.Browser();

  this._bindings.userAgentForUrl = function () {
    return this_._defaultUserAgent;
  };

  this._bindings.processEvent = function (event) {
    this_.emit(event.name, event);
  };

  this._bindings.receive = function (arg) {
    return this_.receive(arg);
  };
}

Browser.prototype = Object.create(EventEmitter.prototype);

Browser.prototype.setUserAgent = function (userAgent) {
  this._defaultUserAgent = userAgent;
};

Browser.prototype.receive = function () { };

function bind(name) {
  Browser.prototype[name] = function () {
    return this._bindings[name].apply(this._bindings, arguments);
  };
}

bind('load');
bind('close');
bind('screenshot');
bind('show');
bind('setSize');
bind('send');

exports.Browser = Browser;
