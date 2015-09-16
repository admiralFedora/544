var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var array = [];
var gcounter = 0;

// sensor object
function sensor(id, temp, counter){
  this.temp = temp // int
  this.success = 0; // boolean
  this.alive = 1; // boolean
  this.id = id; // long
  this.counter = 0;
}

// function to record data on a per object basis
function recordData(data){
  var JsonData = JSON.parse(data);
  var sen;
  if(array.length == 0){
    array.push(new sensor(JsonData.id, JsonData.temp, gcounter));
    return;
  }
  for(var i = 0; i < array.length; i++){
    sen = array[i];
    if(sen.id == JsonData.id){
      sen.temp = JsonData.temp;
      sen.counter = gcounter;
      return;
    }
  }
  array.push(new sensor(JsonData.id, JsonData.temp, gcounter));
}

function printAverage(){
  var sen;
  var average = 0;
  for(var i = 0; i < array.length; i++){
    sen = array[i];
    if((gcounter - sen.counter) <= 3){
      array = array.splice(i, 1);
      console.log("sensor " + i + " has died\n");
    } else {
      average += sen.temp;
    }
  }
  average /= array.length;
  console.log("Average temp: " + average);
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
    recordData(data);
  });
  setInterval(printAverage, 15000);
});
