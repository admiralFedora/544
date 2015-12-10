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

var currDirection = 1; // 1 for forward and -1 for backward
var isDriving = true;//
var globalAngle = 0; // 20 degrees
var lastAdjust = 0;

function stepsObject(){//an object for each action
  var angle;
  var steps;
}

pinSpeed.watch(function(err, value){
  if(err){
    throw err;
  }

  //console.log("change");
  if(isDriving) steps++;
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
  temp.steps = steps*currDirection;

  // we're driving straight again so set it to a straight angle
  globalAngle = Math.floor(globalAngle/90) * 90;
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
  if(steps == 0){
    globalAngle -= lastAdjust;
  }
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
    lastAdjust = 20;
  } else if(right) {
    globalAngle += -20;
    lastAdjust = -20;
  } else {
    lastAdjust = 0;
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

app.get('/start', function(req, res){demandControl();res.json({"msg":"Car control start."});console.log("Car status: Car control begin");isDriving = false;});
app.get('/stop', function(req, res){stopControl();res.json({"msg":"Car control stopped."});console.log("Car status: Car control stopped");isDriving = true;});

app.get('/drive', function(req, res){
  var valUp = parseInt(req.query.u);
  var valDown = parseInt(req.query.d);
  var valLeft = parseInt(req.query.l);
  var valRight = parseInt(req.query.r);

  isDriving = (valUp == 1 || valDown == 1)? true:false;//check if it is driving
  writeOut(valUp, valDown, valLeft, valRight);
  console.log("Received U: "+valUp+" | D: "+valDown+" | L: "+valLeft+" | R: "+valRight);
  var status = "Drive";
  if(valUp == 1) status +=" forward ";
  if(valDown == 1) status +=" backward ";
  if(valLeft == 1) status +=" left ";
  if(valRight == 1) status +=" right ";
  res.json({"msg":status});
  console.log("Car status: "+ status);
});

app.get('/update', function(req, res){
  res.json({"data":"something"});
  printOutJson(stepsArray);
})


var server = app.listen(3000, '0.0.0.0', function(){
  console.log("listening on *:3000");
});

process.on('SIGINT', closePins);


function printOutJson(jsonData){
    var s1 = "";

    for(i = 0 ; i < jsonData.length ; i++){
      s1 += "Angle: "+ jsonData[i].angle+" | steps: "+jsonData[i].steps;
      console.log(s1);
    }
}
