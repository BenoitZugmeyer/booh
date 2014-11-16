var booh = require('../lib/index');
var util = require('util');
var express = require('express');

var app = express();

app.get('/', function (req, res) {
  res.send('<iframe src="/frame"></iframe>');
});

app.get('/frame', function (req, res) {
  res.send('Hi');
});

var server = app.listen(3000, function () {
  var host = server.address().address;
  var port = server.address().port;

  var browser = new booh.Browser();
  browser.load(util.format('http://%s:%s', host, port));
  browser.on('loadFinished', function () {
    browser.screenshot('frame.png');
    browser.close();
    server.close();
  });
});
