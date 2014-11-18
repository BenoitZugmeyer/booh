var booh = require('booh');

browser = new booh.Browser();

// Set the browser width to 1200 pixels for the initial page loading. Note that
// the height is not needed here, because the website won't care about the
// height of the browser. (meaning: the width is 100%, no horizontal scrollbar,
// whereas the height overflows and has a vertical scrollbar)
browser.setSize({width: 1200});

// Load an emerging, little known website
browser.load('http://github.com');

// This will delay (debounce) the makeScreenshot method one second after
// either:
// * the load is finished
// * a request is finished
var delayTimeout;
function delayedScreenshot() {
  clearTimeout(delayTimeout);
  delayTimeout = setTimeout(makeScreenshot, 1000);
}

// Make the actual screenshot
function makeScreenshot() {
  browser.screenshot('github.png');
  console.log('screenshot saved in github.png');
  browser.close();
}

browser.on('loadProgress', function (event) {
  console.log(event.progress + '%');
});

browser.on('loadFinished', delayedScreenshot);
browser.on('requestFinished', delayedScreenshot);
