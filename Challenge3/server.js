var app = require('express')();
var SerialPort = require("serialport");
var xbee = require("xbee-api");
var xbeeApi = xbee.XBeeAPI({
  api_mode: 2,
  module: "Any",
  raw_frames: false
});
var xbeeConst = xbee.constants;

var sp = new SerialPort.SerialPort(process.argv[2], {
  baudrate:9600,
  parser:xbeeApi.rawParser()
});

// sensor object
function sensor(id){
  var status;
  var alive;
  this.id = id;
}

var server = app.listen(3000, '0.0.0.0', function(){
  console.log("listening on *:3000");
});

app.get('/', function(req, res){
  res.send("hello");
});

app.post('/test', function(req, res){
  res.json({"test":"huh"});
});

xbeeApi.on("frame_object", function(frame){

});
