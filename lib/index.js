var EventEmitter = require('events').EventEmitter;
var bindings = require('../build/Debug/booh.node');

function Browser() {
  EventEmitter.call(this);
  this._bindings = new bindings.Browser(this._processEvent.bind(this));
}

Browser.prototype = Object.create(EventEmitter.prototype);

function bind(name) {
  Browser.prototype[name] = function () {
    this._bindings[name].apply(this._bindings, arguments);
  };
}

bind('load');
bind('close');
bind('screenshot');

Browser.prototype._processEvent = function (event) {
  this.emit(event);
};

exports.Browser = Browser;
