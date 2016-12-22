// Author: Ed Dam

// Rocky.js
var rocky = require('rocky');

// Objects draw
var name;
var three;
var nine;
var location;

function fractionToRadian(fraction) {
  return fraction * 2 * Math.PI;
}

function drawHand(ctx, cx, cy, angle, length, color) {
  // Find the end points
  var x2 = cx + Math.sin(angle) * length;
  var y2 = cy - Math.cos(angle) * length;
  // Configure how we want to draw the hand
  ctx.lineWidth = 8;
  ctx.strokeStyle = color;
  // Begin drawing
  ctx.beginPath();
  // Move to the center point, then draw the line
  ctx.moveTo(cx, cy);
  ctx.lineTo(x2, y2);
  // Stroke the line (output to display)
  ctx.stroke();
}

function drawName(ctx, name) {
  // Create a string describing the name
  var nameString = 'Pebble     Ed Dam';
  // Draw the text, top center
  ctx.fillStyle = 'white';
  ctx.textAlign = 'center';
  ctx.font = '18px bold Gothic';
  ctx.fillText(nameString, ctx.canvas.unobstructedWidth / 2, 0);
}

function drawThree(ctx, three) {
  // Create a string describing the number
  var threeString = '3';
  // Draw the text, right
  ctx.fillStyle = 'white';
  ctx.textAlign = 'right';
  ctx.font = '20px bold Leco-numbers';
  ctx.fillText(threeString, ctx.canvas.unobstructedWidth - 2, ctx.canvas.unobstructedHeight / 2 - 12);
}

function drawNine(ctx, nine) {
  // Create a string describing the number
  var nineString = '9';
  // Draw the text, left
  ctx.fillStyle = 'white';
  ctx.textAlign = 'left';
  ctx.font = '20px bold Leco-numbers';
  ctx.fillText(nineString, 2, ctx.canvas.unobstructedHeight / 2 - 12);
}

function drawLocation(ctx, location) {
  // Create a string describing the name
  var locationString = location.name;
  // Draw the text, bottom center
  ctx.fillStyle = 'white';
  ctx.textAlign = 'center';
  ctx.font = '14px bold Gothic';
  ctx.fillText(locationString, ctx.canvas.unobstructedWidth / 2, ctx.canvas.unobstructedHeight - 20);
}

rocky.on('minutechange', function(event) {
  // Request the screen to be redrawn on next minute
  rocky.requestDraw();
});

rocky.on('hourchange', function(event) {
  // Send message to fetch location every hour
  rocky.postMessage({'fetch': true});
});

rocky.on('message', function(event) {
  // Receive a message from the mobile device (pkjs)
  var message = event.data;
  if (message.location) {
    // Save the location
    location = message.location;
    // Request a redraw so we see the location
    rocky.requestDraw();
  }
});

rocky.on('draw', function(event) {
  var ctx = event.context;
  var d = new Date();

  // Clear the screen
  ctx.clearRect(0, 0, ctx.canvas.clientWidth, ctx.canvas.clientHeight);

  // Draw the name and numbers
  drawName(ctx, name);
  drawThree(ctx, three);
  drawNine(ctx, nine);
 
  // Determine the width and height of the display
  var w = ctx.canvas.unobstructedWidth;
  var h = ctx.canvas.unobstructedHeight;

  // Determine the center point of the display
  // and the max size of watch hands
  var cx = w / 2;
  var cy = h / 2;

  // -20 so we're inset 10px on each side
  var maxLength = (Math.min(w, h) - 20) / 2;

  // Calculate the minute hand angle
  var minuteFraction = (d.getMinutes()) / 60;
  var minuteAngle = fractionToRadian(minuteFraction);

  // Draw the minute hand
  drawHand(ctx, cx, cy, minuteAngle, maxLength * 0.8, "white");

  // Calculate the hour hand angle
  var hourFraction = (d.getHours() % 12 + minuteFraction) / 12;
  var hourAngle = fractionToRadian(hourFraction);

  // Draw the hour hand
  drawHand(ctx, cx, cy, hourAngle, maxLength * 0.5, "red");
    
  // Draw the location
  if (location) {
    drawLocation(ctx, location);
  }
});