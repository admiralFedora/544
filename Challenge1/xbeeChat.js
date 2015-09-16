var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var REQUEST = "REQUEST";
var IDREQUEST = "IDREQUEST";

var gindex = 0;
var arrayLength;
var array = [];
// 0 = arduino search; 1 = arduino request
var mode = 0;

function sensor(id){
  var temp; // int
  this.alive = 1; // boolean
  this.id = id; // long
}

function swapMode(){
  mode = mode + 1;
  arrayLength = array.length;
}

function addNewSensor(data){
  var JsonData = JSON.parse(data);
  array.push(new sensor(JsonData.id));
  console.log("Added ID: " + JsonData.id)
}

// the start of the sendRequest chain
function requestData(){
/*  var buffer = "";
  var sen;
  for(var i = 0; i < array.length; i++){
    sen = array[i];
    buffer = sen.id + "";
    console.log(buffer);
    buffer = REQUEST.concat(buffer);

    sp.write(buffer);
    console.log(buffer);
    setTimeout(function(){healthUpdate(sen)}, 2000);
  }
*/
  gindex = 0;
  sendRequest(gindex);
}

// this function sends a request out to a specific sensor
function sendRequest(index){
  // if requested index is beyond the array don't do anything
  if(index >= arrayLength){
    return;
  }
  var sen = array[index];

  // is this sensor alive?
  if(sen.alive == 0){
    console.log("sensor" + index + " has died\n");

    // remove object from our array
    array = array.splice(index,1);
    // update the length
    arrayLength = array.length;
    // send request for the "same" index though it'll be the next over in the old structure
    sendRequest(gindex);
    return;
  }
  // building our request phrase
  buffer = sen.id + "";
  buffer = REQUEST.concat(buffer);
  sp.write(buffer);
  console.log(buffer);

  // we are giving each sensor 1.5 seconds to send back data
  setTimeout(function(){
    // gindex should be different than the local index as recordData increments gindex
    if(gindex != index){
      return;
    } else { // if recordData never ran then gindex = index and therefore we will need to retry
      //gindex = index + 1;
      sendRequest(gindex);
      //sen.alive = 0;
    }
  }, 1500);
}

// parse and record the received data
function recordData(data){
  var JsonData = JSON.parse(data);
  var sen;
  for(var i = 0; i < array.length; i++){
    sen = array[i];
    if(sen.id == JsonData.id){
      sen.temp = JsonData.temp;
      console.log("id: "+ sen.id + " temp: " + sen.temp);

      gindex = gindex + 1;

      // we don't want to flood the network, so we wait for 1 second before the next sensor request
      setTimeout(function(){sendRequest(gindex)},1000);
      //sendRequest(gindex);
      return;
    }
  }
}

function healthUpdate(sen){
  sen.alive = 0;
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
