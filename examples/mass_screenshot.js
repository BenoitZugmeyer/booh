var booh = require('../lib/index');

var top100websites = [
  'google.com', 'facebook.com', 'youtube.com', 'yahoo.com', 'www.baidu.com',
  'wikipedia.org', 'www.qq.com', 'taobao.com', 'twitter.com', 'live.com',
  'amazon.com', 'linkedin.com', 'google.co.in', 'www.sina.com.cn', 'hao123.com',
  'weibo.com', 'blogspot.com', 'tmall.com', 'sohu.com', 'yahoo.co.jp',
  'vk.com', 'yandex.ru', 'wordpress.com', 'bing.com', 'ebay.com', 'google.de',
  'pinterest.com', '360.cn', 'google.co.uk', 'google.fr', 'instagram.com',
  'google.co.jp', '163.com', 'soso.com', 'ask.com', 'msn.com', 'tumblr.com',
  'google.com.br', 'mail.ru', 'xvideos.com', 'microsoft.com', 'google.ru',
  'PayPal.com', 'google.it', 'google.es', 'apple.com', 'imdb.com',
  'adcash.com', 'imgur.com', 'craigslist.org', 'neobux.com', 'amazon.co.jp',
  't.co', 'reddit.com', 'xhamster.com', 'google.com.mx', 'stackoverflow.com',
  'fc2.com', 'google.ca', 'go.com', 'bbc.co.uk', 'cnn.com', 'ifeng.com',
  'aliexpress.com', 'people.com.cn', 'xinhuanet.com', 'blogger.com',
  'adobe.com', 'vube.com', 'alibaba.com', 'google.com.tr', 'odnoklassniki.ru',
  'godaddy.com', 'googleusercontent.com', 'google.com.hk',
  'huffingtonpost.com', 'pornhub.com', 'wordpress.org', 'kickass.to',
  'google.co.id', 'thepiratebay.se', 'amazon.de', 'google.com.au', 'youku.com',
  'Chinadaily.com.cn', 'netflix.com', 'espn.go.com', 'ebay.de', 'google.pl',
  'gmw.cn', 'dailymotion.com', 'Akamaihd.net', 'Clkmon.com', 'alipay.com',
  'about.com', 'bp.blogspot.com', 'dailymail.co.uk', 'rakuten.co.jp',
  'xnxx.com', 'indiatimes.com',
];


var processing = [];

function processNext(browser) {
  var website = top100websites.shift();
  var waitTimeout, timeoutTimeout;

  if (!website) {
    browser.close();
    return;
  }

  console.log('loading', website);
  processing.push(website);
  browser.load('http://' + website);

  function next() {
    clearTimeout(waitTimeout);
    clearTimeout(timeoutTimeout);
    browser.removeListener('loadFinished', finished);

    processing.splice(processing.indexOf(website), 1);
    console.log('waiting for:', processing.join(', '));

    processNext(browser);
  }

  function finished() {
    waitTimeout = setTimeout(function () {
      try {
        browser.screenshot('images/' + website + '.png');
        console.log('finished', website);
      } catch (e) {
        console.log('screenshot failed for', website, e.message);
      }
      next();
    }, 10000);
  }

  browser.on('loadFinished', finished);

  setTimeout(function () {
    console.log('timeout', website);
    next();
  }, 30000);
}

var i;
for (i = 0; i < 10; i++) {
  processNext(new booh.Browser());
}
