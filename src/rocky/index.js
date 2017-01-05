// Author: Ed Dam

// Rocky.js

var rocky  = require('rocky');

// Draw Line

function drawLine(ctx, linewidth, color, position, width) {
  ctx.lineWidth   = linewidth;
  ctx.strokeStyle = color;
  ctx.strokeRect(0, position, width, 0);
}

// Draw Box

function drawBox(ctx, color, x, y, width, height) {
  ctx.fillStyle = color;
  ctx.fillRect(x, y, width, height);
}

// Draw Text

function drawText(ctx, text, color, align, font, width, height) {
  ctx.fillStyle = color;
  ctx.textAlign = align;
  ctx.font      = font;
  ctx.fillText(text, width, height);
}

// Collect API Message

var api;

rocky.on('hourchange', function(event) {
  rocky.postMessage({'fetch': true});
});

rocky.on('message', function(event) {
  var message = event.data;
  if (message.api) {
    api = message.api;
    rocky.requestDraw();
  }
});

// Redraw Every Minute

rocky.on('minutechange', function(event) {
  rocky.requestDraw();
});

// Draw Watchface

rocky.on('draw', function(event) {
  var ctx = event.context;
  
  // Define Screen Size
  var mx = ctx.canvas.unobstructedWidth;
  var my = ctx.canvas.unobstructedHeight;
  
  // Clear Canvas
  ctx.clearRect(0, 0, mx, my);
  
  // Define Centre of Screen
  var cx = mx / 2;
  var cy = my / 2;
  
  // Draw Lines
  drawLine(ctx, 2, 'white', cy - 45, mx);
  drawLine(ctx, 2, 'white', cy + 57, mx);
  
  // Draw Box
  drawBox(ctx, 'white', 0,       cy - 19, mx, 50);
  drawBox(ctx, 'gray',  cx - 5,  cy - 75, 69, 19);
  drawBox(ctx, 'gray',  cx - 12, cy + 64, 24, 17);
  drawBox(ctx, 'gray',  cx - 17, cy + 64, 2,  17);
  drawBox(ctx, 'gray',  cx + 15, cy + 64, 2,  17);

  // Draw Fixed Text
  drawText(ctx, 'pebble',       'white', 'right',  '24px bold Gothic', cx - 13, cy - 84);
  drawText(ctx, 'digital v1.5', 'black', 'left',   '14px bold Gothic', cx - 4,  cy - 75);
  drawText(ctx, 'water',        'gray',  'right',  '14px bold Gothic', cx - 23, cy + 64);
  drawText(ctx, 'WR',           'black', 'center', '18px bold Gothic', cx,      cy + 60);
  drawText(ctx, 'resist',       'gray',  'left',   '14px bold Gothic', cx + 23, cy + 64);
  
  // Draw Time
  var dateHour   = new Date().toLocaleTimeString(undefined, {hour:   '2-digit'});
  var dateMinute = new Date().toLocaleTimeString(undefined, {minute: '2-digit'});
  var time       = dateHour + ":" + dateMinute;
  drawText(ctx, time, 'black', 'center', '49px Roboto-subset', cx, cy - 26);

  // Draw Date
  var dateDay   = new Date().toLocaleDateString(undefined, {day:   'long'});
  var dateDate  = new Date().toLocaleDateString(undefined, {day:   'numeric'});
  var dateMonth = new Date().toLocaleDateString(undefined, {month: 'short'});
  var date      = dateDay + " " + dateDate + " " + dateMonth;
  drawText(ctx, date, 'white', 'center', '18px bold Gothic', cx,  cy - 44);
  
  // Draw Template
  drawText(ctx, 'Location ?°C', 'white', 'center', '18px bold Gothic', cx, cy + 31);
  
  // Draw Weather
  if (api) {
    ctx.clearRect(0, cy + 37, mx, 12);
    var city        = api.location;
    var temperature = api.temperature;
    drawText(ctx, city + ' ' + temperature + '°C', 'white', 'center', '18px bold Gothic', cx, cy + 31);
  }
});
