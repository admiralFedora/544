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

var sensors[];

// sensor object
function sensor(id){
  var status;
  var frameId;
  this.id = id;
}

function addSensor(id){
  sensors.push(new sensor(id));
}

function checkStatus(frame){
  var sen;
  for(var i = 0; i < sensors.length; i++){
    sen = sensors[i];
    if(frame.remote16 == sen.id
        && frame.id == sen.frameId && frame.deliveryStatus == 0){
        // remove dead sensor
        sensors.splice(i,1);
    }
  }
}

var server = app.listen(3000, '0.0.0.0', function(){
  console.log("listening on *:3000");
});

app.get('/', function(req, res){
  res.send("hello");
});

// this end point returns the array of active sensors
app.get('/sensors', function(req, res){
  res.json(JSON.stringify(sensors));
});

app.post('/test', function(req, res){
  res.json({"test":"huh"});
});

xbeeApi.on("frame_object", function(frame){
  if(frame.type == xbeeConst.FRAME_TYPE.NODE_IDENTIFICATION){
    addSensor(frame.sender16);
  } else if (frame.type == xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_STATUS){
    checkStatus(frame);
  }
});
