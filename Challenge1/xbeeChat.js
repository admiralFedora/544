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

function sendRequest(index){

  if(index >= arrayLength){
    return;
  }
  var sen = array[index];
  if(sen.alive == 0){
    console.log("sensor" + index + " has died\n");
    array = array.splice(index,1);
    arrayLength = array.length;
    sendRequest(gindex);
    return;
  }
  buffer = sen.id + "";
  console.log(buffer);
  buffer = REQUEST.concat(buffer);

  sp.write(buffer);
  console.log(buffer);

  setTimeout(function(){
    if(gindex != index){
      return;
    } else {
      //gindex = index + 1;
      sendRequest(gindex);
      //sen.alive = 0;
    }
  }, 1500);
}

function recordData(data){
  var JsonData = JSON.parse(data);
  var sen;
  var j = 0;
  for(var i = 0; i < array.length; i++){
    sen = array[i];
    if(sen.id == JsonData.id){
      sen.temp = JsonData.temp;
      console.log("id: "+ sen.id + " temp: " + sen.temp);
      
      gindex = gindex + 1;
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
