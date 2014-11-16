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
  this._bindings = new bindings.Browser(this._processEvent.bind(this));

  this._bindings.userAgentForUrl = function () {
    return this_._defaultUserAgent;
  };
}

Browser.prototype = Object.create(EventEmitter.prototype);

Browser.prototype.setUserAgent = function (userAgent) {
  this._defaultUserAgent = userAgent;
};

function bind(name) {
  Browser.prototype[name] = function () {
    this._bindings[name].apply(this._bindings, arguments);
  };
}

bind('load');
bind('close');
bind('screenshot');
bind('show');
bind('setSize');

Browser.prototype._processEvent = function (event) {
  this.emit(event.name, event);
};

exports.Browser = Browser;
