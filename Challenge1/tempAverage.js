var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var REQUEST = "REQUEST";
var IDREQUEST = "IDREQUEST";

var array = [];
// 0 = arduino search; 1 = arduino request
var mode = 0;

// sensor object
function sensor(id){
  var temp; // int
  this.success = 0; // boolean
  this.alive = 1; // boolean
  this.id = id; // long
}

// change data read mode
function swapMode(){
  mode = mode + 1;
  arrayLength = array.length;
}

// add in a new sensor to our array
function addNewSensor(data){
  var JsonData = JSON.parse(data);
  array.push(new sensor(JsonData.id));
  console.log("Added ID: " + JsonData.id)
}

// function to start the data request
function requestData(){
  // talking to all sensors at once
  sp.write(REQUEST);
  // give sensors five seconds to report
  setTimeout(finishRequest, 5000);
}

// function to record data on a per object basis
function recordData(data){
  var JsonData = JSON.parse(data);
  var sen;
  for(var i = 0; i < array.length; i++){
    sen = array[i];
    if(sen.id == JsonData.id){
      sen.temp = JsonData.temp;
      sen.success = 1;
      console.log("id: "+ sen.id + " temp: " + sen.temp);
      return;
    }
  }
}

// our timeout function, removes the dead devices from the array
function finishRequest(){
  var sen;
  var average;
  for(var index = 0; index < array.length; index++){
    sen = array[index];
    if(sen.success == 1){
      average += sen.temp;
    } else if(sen.success == 0){
      array = array.splice(index, 1);
      console.log("Sensor " + sen.id + " is dead\n");
    }
  }
  average /= array.length;
  console.log("Average: " + average + "*C\n");
}

var portName = process.argv[2],
portConfig = {
  baudRate: 9600,
  parser: SerialPort.parsers.readline("\n")
};

var sp;  //serial Port

sp = new SerialPort.SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg){
    io.emit('chat message', msg );
    sp.write(msg + "\n");
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

sp.on("open", function () {
  console.log('open');
  sp.on('data', function(data) {
    switch(mode){
      case 0:
        addNewSensor(data);
        break;
      case 1:
        recordData(data);
        break;
    }
  });
  sp.write(IDREQUEST);
  // give 3 seconds to search for all arduino devices
  setTimeout(swapMode, 3000);
  // request data from the sensors every 10 seconds
  setInterval(requestData, 10000);
});
