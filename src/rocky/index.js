// Author: Ed Dam

var rocky  = require('rocky');

function drawText(ctx, text, color, align, font, width, height) {
  ctx.fillStyle = color;
  ctx.textAlign = align;
  ctx.font      = font;
  ctx.fillText(text, width, height);
}

var location;
rocky.on('message', function(event) {
  var message = event.data;
  if (message.location) {
    location = message.location;
    rocky.requestDraw();
  }
});

rocky.on('hourchange', function(event) {
  rocky.postMessage({'fetch': true});
});
rocky.on('minutechange', function(event) {
  rocky.requestDraw();
});

rocky.on('draw', function(event) {
  var ctx        = event.context;
  var width      = ctx.canvas.unobstructedWidth;
  var height     = ctx.canvas.unobstructedHeight;
  var dateDay    = new Date().toLocaleDateString(undefined, {day: '2-digit'});
  var dateMonth  = new Date().toLocaleDateString(undefined, {month: 'short'});
  var dateHour   = new Date().toLocaleTimeString(undefined, {hour: '2-digit'});
  var dateMinute = new Date().toLocaleTimeString(undefined, {minute: '2-digit'});
  var date       = dateDay + " " + dateMonth;
  var time       = dateHour + ":" + dateMinute;
  var city       = location.name;
  ctx.clearRect(0, 0, ctx.canvas.clientWidth, ctx.canvas.clientHeight);
  drawText(ctx, 'pebble',    'white', 'left',   '24px bold Gothic',    9,          0);
  drawText(ctx, 'edwarddam', 'white', 'right',  '14px bold Gothic',    width - 9,  9);
  drawText(ctx, date,        'white', 'center', '18px bold Gothic',    width / 2,  height / 2 - 30);
  drawText(ctx, time,        'white', 'center', '42px Bitham-numeric', width / 2,  height / 2 - 21);
  drawText(ctx, 'water -',   'white', 'left',   '14px bold Gothic',    15,         height - 20);
  drawText(ctx, 'WR',        'white', 'center', '18px bold Gothic',    width / 2,  height - 24);
  drawText(ctx, '- resist',  'white', 'right',  '14px bold Gothic',    width - 15, height - 20);
  drawText(ctx, city,        'white', 'center', '18px bold Gothic',    width / 2, height - 50);
});
