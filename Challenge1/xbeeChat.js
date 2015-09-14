var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var JsonData; //global para for JASON Data
var Count = 0; // messagges received
var TotalTemp = 0; //Sum of Temp

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
    io.emit('chat message', msg+ "mark here");
    sp.write(msg + "\n");
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

sp.on("open", function () {
  console.log('open');
  sp.on('data', function(data) {
    JsonData = JSON.parse(data);
    Count ++;
    TotalTemp += JsonData.temp;
    console.log('data received: id:' + JsonData.id + "  temp: " + JsonData.temp + "   Ave: " + TotalTemp/Count);
    io.emit("chat message",  "Received Data: id:" + JsonData.id + "  temp: " + JsonData.temp + "    Ave: " + TotalTemp/Count);
  });
});

