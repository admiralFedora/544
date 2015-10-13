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

// counter object
function counter(num){
  this.num = num;
}

// sensor object
function sensor(id, idLong){
  var status;
  var frameId;
  var deferred;
  var counter;
  this.id = id;
  this.idLong = idLong;
}

function sendData(dest, data, id){
  var type = xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST;
  this.id = id;
  this.destination16 = dest;
  this.data = data;
}

function addSensor(id, idLong){
  console.log("new sensor", idLong);
  sensors.push(new sensor(id, idLong));
}

function checkStatus(frame){
  var sen;
  for(var i = 0; i < sensors.length; i++){
    sen = sensors[i];
    if(frame.id == sen.frameId
        && frame.remote16 == sen.id && frame.deliveryStatus == 0){
        console.log("resolving", frame.id);
        sen.deferred.resolve(frame);
    }
  }
}

function turnOnLights(sensor_num, count, res){
  var sen = sensors[sensor_num];
  sen.counter = count;
  sen.deferred = Q.defer();
  sen.frameId = xbeeAPI.nextFrameId();
  var sendingFrame = new sendData(sen.id, "F", sen.frameId);

  sp.write(xbeeAPI.buildFrame({
    type: xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
    id: sen.frameId,
    destination64: sen.idLong,
    data: [0x8]
  }));

  console.log("sending to ", sen.idLong);

  sen.deferred.promise.timeout(3000)
  .then(function(f){
    sen.counter.num--;
    console.log("command received correctly");
    if(sen.counter.num == 0){
      var msg = "";
      for(var i = 0; i < sensors.length; i++){
        msg += "sensor" + sen.id;
      }
      res.json({"msg":msg});
    }
  })
  .catch(function(e){
    sen.counter.num--;
    // remove dead sensor
    console.log("Removing dead sensor", sen.id);
    sensors.splice(sensor_num, 1);
    if(sen.counter.num == 0 && sensor.length > 0){
      var msg = "";
      for(var i = 0; i < sensors.length; i++){
        msg += "sensor" + sen.id;
      }
      res.json({"msg":msg});
    } else {
      res.json({"msg":"All sensors have died"})
    }
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

app.get('/turnOnAll', function(req, res){
  if(sensors.length == 0){
    res.status(400).json({"msg":"No sensors to turn on"});
  } else {
    var count = new counter(sensors.length);
    for(var i = 0; i < sensors.length; i++){
      turnOnLights(i, count, res);
    }
  }
});

app.post('/test', function(req, res){
  res.json({"test":"huh"});
});

xbeeAPI.on("frame_object", function(frame){
  console.log(frame); // for debug purposes
  if(frame.type == xbeeConst.FRAME_TYPE.NODE_IDENTIFICATION){
    console.log(frame.sender64);
    addSensor(frame.sender16, frame.sender64);
  } else if (frame.type == xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_STATUS){
    checkStatus(frame);
  }
});

//-------------------------------------------------------
function repeatSending(){ //send packet every 6 secs
  var sampleFrame = {
      type: xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST, // xbee_api.constants.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST
      id: 0x0F, // optional, nextFrameId() is called per default
      destination64: "0013a20040a1a153",
      //destination64: "0013a20040a1a153",
      //destination16: "97BF",
      data: "TxData0A" // Can either be string or byte array.
  };
  console.log(xbeeAPI.buildFrame(sampleFrame));
  sp.write(xbeeAPI.buildFrame(sampleFrame));
}
//-------------------------------------------------------

sp.on("open",function(){
  var sampleFrame = {
      type: xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST, // xbee_api.constants.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST
      id: 0x0F, // optional, nextFrameId() is called per default
      destination64: "0013a20040a1a178",
      data: "TxData0A" // Can either be string or byte array.
  };
  //console.log(xbeeAPI.buildFrame(sampleFrame));
  sp.write(xbeeAPI.buildFrame(sampleFrame));
  //setInterval(repeatSending, 6000);

});
