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
  
  stepsArray.push(steps);
  steps = 0;
});


function demandControl(){
  pinStop.writeSync(1);
}

function writeOut(up, down, left, right){
  pinUp.writeSync(up);
  pinDown.writeSync(down);
  pinLeft.writeSync(left);
  pinRight.writeSync(right);
}

function closePins() {
    pinUp.unexport();
    pinDown.unexport();
    pinLeft.unexport();
    pinRight.unexport();
    pinStop.unexport();
    pinTurn.unexport();
    pinSpeed.unexport();
}

/*Ajax requests*/
app.get('/', function(req, res){
  res.sendfile("default.html"); //return the default page
})

app.get('/up', function(req, res){up(1);res.json({"msg":"Drive forward;"});console.log("Car status: Drive forward;")});
app.get('/down', function(req, res){down(1);res.json({"msg":"Drive backward;"});console.log("Car status: Drive backward;")});
app.get('/right', function(req, res){res.json({"msg":"Turn right;"});console.log("Car status: Turn right")});
app.get('/left', function(req, res){res.json({"msg":"Turn left;"});console.log("Car status: Turn left")});
app.get('/stop', function(req, res){demandControl();res.json({"msg":"Car stopped;"});console.log("Car status: Car stopped")});

//still need to read the speed

function mapData(){
}

var server = app.listen(3000, '0.0.0.0', function(){
  console.log("listening on *:3000");
});

process.on('SIGINT', closePins);
