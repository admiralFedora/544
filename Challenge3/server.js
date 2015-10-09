var app = require('express')();
var SerialPort = require("serialport");
var xbee = require("xbee-api");
var Q = require("q");
var xbeeAPI = new xbee.XBeeAPI({
  api_mode: 2
});
var xbeeConst = xbee.constants;

var sp = new SerialPort.SerialPort(process.argv[2], {
  baudrate: 9600,
  parser: xbeeAPI.rawParser()
});

var sensors = [];

// sensor object
function sensor(id){
  var status;
  var frameId;
  var deferred;
  this.id = id;
}

function sendData(dest, data, id){
  var type = xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST;
  this.id = id;
  this.destination16 = dest;
  this.data = data;
}

function addSensor(id){
  sensors.push(new sensor(id));
}

function checkStatus(frame){
  var sen;
  for(var i = 0; i < sensors.length; i++){
    sen = sensors[i];
    if(frame.id == sen.frameId){
        sen.deferred.resolve(frame);
    }
  }
}

function turnOnLights(sensor_num){
  var sen = sensors[sensor_num];
  sen.deferred = Q.defer();
  sen.frameId = xbeeAPI.nextFrameId();
  var sendingFrame = new sendingData(sen.id, "F", sen.frameId);

  sp.write(xbeeAPI.buildFrame(sendingFrame));

  sen.deferred.promise.timeout(3000)
  .then(function(f){
    console.log("command received correctly");
    console.log("Frame:", f);
  })
  .catch(function(e){
    // remove dead sensor
    console.log("Removing dead sensor", sen.id);
    sensors.splice(sensor_num, 1);
  });
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

app.post('/turnOnAll', function(req, res){
  if(sensors.length == 0){
    res.status(400).json({"msg":"No sensors to turn on"});
  } else {
    for(var i = 0; i < sensors.length; i++){
      turnOnLights(i);
    }
  }
});

app.post('/test', function(req, res){
  res.json({"test":"huh"});
});

xbeeAPI.on("frame_object", function(frame){
  console.log(frame); // for debug purposes
  if(frame.type == xbeeConst.FRAME_TYPE.NODE_IDENTIFICATION){
    addSensor(frame.sender16);
  } else if (frame.type == xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_STATUS){
    checkStatus(frame);
  }
});

sp.on("open",function(){
  var sampleFrame = {
      type: xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST, // xbee_api.constants.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST
      id: 0x0F, // optional, nextFrameId() is called per default
      destination16: "2941", // optional, "fffe" is default
      data: "TxData0A" // Can either be string or byte array.
  };

  sp.write(xbeeAPI.buildFrame(sampleFrame));
  console.log(xbeeAPI.buildFrame(sampleFrame));
});
