var fs = require('fs');
var app = require('express')();
var GPIO = require('onoff').Gpio


//--------------------------------------------------------------------
//set of pin numbers

var pinUp = new GPIO(5, 'out');
var pinDown = new GPIO(6, 'out');
var pinLeft = new GPIO(13, 'out');
var pinRight = new GPIO(19, 'out');
var pinStop = new GPIO(26, 'out');
var pinSpeed = new GPIO(17, 'in', 'rising');
var pinTurn = new GPIO(27, 'in', 'rising');

var steps = 0;
var stepsArray = [];

var currDirection = 1;

var globalAngle = 0;

function stepsObject(){
  var angle;
  var steps;
}

pinSpeed.watch(function(err, value){
  if(err){
    throw err;
  }

  console.log("change");
  steps++
});

pinTurn.watch(function(err, value){
  if(err){
    throw err;
  }

  var temp = new stepsObject();
  temp.angle = globalAngle;
  temp.steps = steps*currDirection;
  stepsArray.push(temp);
  
  steps = 0;
  globalAngle += 90;
});


function demandControl(){
  var temp = new stepsObject();
  temp.angle = globalAngle;
  temp.steps = steps;
  steps = 0;
  stepsArray.push(temp);
  
  currDirection = 0;
  writeOut(0,0,0,0);
  pinStop.writeSync(1);
}

function stopControl(){
  var temp = new stepsObject();
  temp.angle = globalAngle;
  temps.steps = steps*currDirection;
  steps = 0;
  currDirection = 1;
  stepsArray.push(temp);
  
  writeOut(0,0,0,0);
  pinStop.writeSync(0);
}

function writeOut(up, down, left, right){
  var temp = new stepsObject();
  temp.angle = globalAngle;
  temp.steps = steps*currDirection;
  steps =  0;
  stepsArray.push(temp);
  
  pinUp.writeSync(up);
  pinDown.writeSync(down);
  pinLeft.writeSync(left);
  pinRight.writeSync(right);
  
  if(up){
    currDirection = 1;
  } else if(down){
    currDirection = -1;
  }
  
  if(left){
    globalAngle += 20;
  } else if(right) {
    globalAngle += -20;
  }
}

function updatePosition(){
  var temp = new stepsObject();
  temp.angle = globalAngle;
  temp.steps = steps*currDirection;
  steps = 0;
  stepsArray.push(temp);
}

function closePins() {
    pinUp.unexport();
    pinDown.unexport();
    pinLeft.unexport();
    pinRight.unexport();
    pinStop.unexport();
    pinTurn.unexport();
    pinSpeed.unexport();
    process.exit();
}

/*Ajax requests*/
app.get('/', function(req, res){
  res.sendfile("default.html"); //return the default page
})

app.get('/up', function(req, res){writeOut(1, 0, 0, 0);res.json({"msg":"Drive forward;"});console.log("Car status: Drive forward;")});
app.get('/down', function(req, res){writeOut(0, 1, 0, 0);res.json({"msg":"Drive backward;"});console.log("Car status: Drive backward;")});
app.get('/right', function(req, res){writeOut(0, 0, 0, 1);res.json({"msg":"Turn right;"});console.log("Car status: Turn right")});
app.get('/left', function(req, res){writeOut(0, 0, 1, 0);res.json({"msg":"Turn left;"});console.log("Car status: Turn left")});
app.get('/start', function(req, res){demandControl();res.json({"msg":"Car control start;"});console.log("Car status: Car control begin")});
app.get('/stop', function(req, res){stopControl();res.json({"msg":"Car control stopped;"});console.log("Car status: Car control stopped")});

//still need to read the speed

function mapData(){
}

var server = app.listen(3000, '0.0.0.0', function(){
  console.log("listening on *:3000");
});

process.on('SIGINT', closePins);
