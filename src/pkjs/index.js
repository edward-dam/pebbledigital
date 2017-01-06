//Author: Ed Dam

var Clay       = require('./clay');
var clayConfig = require('./config');
var clay       = new Clay(clayConfig, null, { autoHandleEvents: false });

var myAPIKey   = '42efb17b4ad3d4a52025106d13376d96';

Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) {
    return;
  }
  var settings     = clay.getSettings(e.response, false);
  var settingsFlat = {};
  Object.keys(settings).forEach(function(key) {
    if (typeof settings[key] === 'object' && settings[key]) {
      settingsFlat[key] = settings[key].value;
    } else {
      settingsFlat[key] = settings[key];
    }
  });
  Pebble.postMessage(settingsFlat);
});

function restoreSettings() {
  var settings = JSON.parse(localStorage.getItem('clay-settings'));
  if (settings) {
    Pebble.postMessage(settings);
  }
}

Pebble.on('message', function(event) {
  if (event.data.command === 'settings') {
    restoreSettings();
  }
  var message = event.data;
  if (message.fetch) {
    navigator.geolocation.getCurrentPosition(function(pos) {
      var url = 'http://api.openweathermap.org/data/2.5/weather' +
        '?lat=' + pos.coords.latitude +
        '&lon=' + pos.coords.longitude +
        '&appid=' + myAPIKey;
      request(url, 'GET', function(respText) {
        var weatherData = JSON.parse(respText);
        Pebble.postMessage({
          'weather': {
            'location':   weatherData.name,
            'celsius':    Math.round(weatherData.main.temp - 273.15),
            'fahrenheit': Math.round((weatherData.main.temp - 273.15) * 9 / 5 + 32)
          }
        });
      });
    }, function(err) {
      console.error('Error getting location');
    },
    { timeout: 15000, maximumAge: 60000 });
  }
});

function request(url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function (e) {
    if (xhr.status >= 400 && xhr.status < 600) {
      console.error('Request failed with HTTP status ' + xhr.status + ', body: ' + this.responseText);
      return;
    }
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
}