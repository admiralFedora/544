var fs = require('fs');
var app = require('express')();
var gpio = require('rpi-gpio');


//--------------------------------------------------------------------
//set of pin numbers
var pinUp = 1;
var pinDown = 2;
var pinLeft = 3;
var pinRight = 4;
var pinStop = 6;
var pinSpeed = 5;
var pinTurn = 7;

var steps = 0;
var stepsArray = [];

gpio.on('change', function(channel, value) {
    if(channel == pinSpeed){
      steps++;
    }

    if(channel == pinTurn){
      stepsArray.push(steps);
      steps = 0;
    }
});

gpio.setup(pinSpeed, gpio.DIR_IN, gpio.EDGE_RISING);
gpio.setup(pinTurn, gpio.DIR_IN, gpio.EDGE_RISING);
gpio.setup(pinStop, gpio.DIR_OUT);
gpio.setup(pinUp, gpio.DIR_OUT);
gpio.setup(pinDown, gpio.DIR_OUT);
gpio.setup(pinLeft, gpio.DIR_OUT);
gpio.setup(pinRight, gpio.DIR_OUT);

function demandControl(){
  gpio.write(pinStop, true);
}

function up(value){
  gpio.write(pinUp, value);
}

function down(value){
  gpio.write(pinDown, value);
}

function left(value){
  gpio.write(pinLeft, value);
}

function right(value){
  gpio.write(pinRight, value);
}

function writeOut(up, down, left, right){
  var input = (up << 4) | (down << 3) | (left << 2) | right;

  switch(input){
    case 0x0:
      up(false);
      down(false);
      left(false);
      right(false);
      break;
    case 0x1:
      up(false);
      down(false);
      left(false);
      right(true);
      break;
    case 0x2:
      up(false);
      down(false);
      left(true);
      right(false);
      break;
    case 0x3:
      up(false);
      down(false);
      left(true);
      right(true);
      break;
    case 0x4:
      up(false);
      down(true);
      left(false);
      right(false);
      break;
    case 0x5:
      up(false);
      down(true);
      left(false);
      right(true);
      break;
    case 0x6:
      up(false);
      down(true);
      left(true);
      right(false);
      break;
    case 0x7:
      up(false);
      down(true);
      left(true);
      right(true);
      break;
    case 0x8:
      up(true);
      down(false);
      left(false);
      right(false);
      break;
    case 0x9:
      up(true);
      down(false);
      left(false);
      right(true);
      break;
    case 0xA:
      up(true);
      down(false);
      left(true);
      right(false);
      break;
    case 0xB:
      up(true);
      down(false);
      left(true);
      right(true);
      break;
    case 0xC:
      up(true);
      down(true);
      left(false);
      right(false);
    case 0xD:
      up(true);
      down(true);
      left(false);
      right(true);
      break;
    case 0xE:
      up(true);
      down(true);
      left(true);
      right(false);
      break;
    case 0xF:
      up(true);
      down(true);
      left(true);
      right(true);
  }
}

function closePins() {
    gpio.write(pinStop, false);
    gpio.destroy(function() {
        console.log('All pins unexported');
    });
}

/*Ajax requests*/
app.get('/', function(req, res){
  res.sendfile("default.html"); //return the default page
});

app.get('/up', up(req));
app.get('/down', down(req));
app.get('/right', right(req));
app.get('/left', left(req));
app.get('/stop', demandControl(req));
//still need to read the speed

function mapData(){
}

var server = app.listen(3000, '0.0.0.0', function(){
  console.log("listening on *:3000");
});

process.on('SIGINT', closePins);
