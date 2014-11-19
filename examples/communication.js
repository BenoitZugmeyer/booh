var booh = require('../lib/index');
var util = require('util');
var express = require('express');

var app = express();

app.get('/', function (req, res) {
  res.send(
    '<script>' +
    // We send 1 and display the result. It will be 2, as the nodejs
    // receive() method returns its argument incremented by 1 (see [1])
    'console.log("Result of booh.send(1): " + booh.send(1));' +

    // We define a receive method, called when the nodejs send() method
    // (see [2]) will be called
    'booh.receive = function (arg) {' +
    '  console.log("Received from nodejs: " + (typeof arg) + " " + arg);' +
    '  return arg + 40;' +
    '};' +
    '</script>'
  );
});

var server = app.listen(3000, function () {
  var host = server.address().address;
  var port = server.address().port;

  var browser = new booh.Browser();
  browser.load(util.format('http://%s:%s', host, port));

  // [1] nodejs receive definition. Always increment the argument by 1
  browser.receive = function (arg) {
    console.log('Received from the browser:', arg);
    return arg + 1;
  };

  browser.on('loadFinished', function () {
    // [2] This will be 42 as the browser receive() method returns its argument
    // incremented by 40
    console.log('Result of browser.send(2):', browser.send(2));

    browser.close();
    server.close();
  });
});

