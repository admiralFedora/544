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

// message object
function message(msg){
  this.msg = msg;
}

// sensor object
function sensor(id, idLong){
  var status;
  var frameId;
  var deferred;
  var counter;
  var message;
  var res;
  this.id = id;
  this.idLong = idLong;
}

// function for adding a sensors
// does not add a sensor if it already exists
function addSensor(id, idLong){
  var sen;
  var add = true;
  for(var i = 0; i < sensors.length; i++){
    sen = sensors[i];
    if(sen.id == id){
      add = false;
    }
  }

  if(add){
    console.log("new sensor", idLong);
    sensors.push(new sensor(id, idLong));
  }
}

// checks the delivery status and resolves the promise made during the initial send
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

// starts the send light off procedure which includes creating a promise
function turnOffLights(sensor_num, count, res, msg){
  var sen = sensors[sensor_num];
  sen.message = msg;
  sen.counter = count;
  sen.deferred = Q.defer();
  sen.frameId = xbeeAPI.nextFrameId();
  //var sendingFrame = new sendData(sen.id, "F", sen.frameId);

  sp.write(xbeeAPI.buildFrame({
    type: xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
    id: sen.frameId,
    destination64: sen.idLong,
    data: [0x0]
  }));

  console.log("sending to ", sen.idLong);

  sen.deferred.promise.timeout(3000)
  .then(function(f){
    sen.counter.num--;
    console.log("command received correctly");
    sen.message.msg += "sensor " + sen.id + " turned off;";
    if(sen.counter.num == 0){
      res.json({"msg":sen.message.msg});
    }
  })
  .catch(function(e){
    sen.counter.num--;
    // remove dead sensor
    console.log("Removing dead sensor", sen.id);
    sen.message.msg += "sensor " + sen.id + " has died;";
    if(sen.counter.num == 0 && sensor.length > 0){
      res.json({"msg":sen.message.msg});
    } else {
      res.json({"msg":"All sensors have died"})
    }
    sensors.splice(sensor_num, 1);
  });
}

// starts the send light on procedure which includes creating a promise
function turnOnLights(sensor_num, count, res, msg){
  var sen = sensors[sensor_num];
  sen.message = msg;
  sen.counter = count;
  sen.deferred = Q.defer();
  sen.frameId = xbeeAPI.nextFrameId();
  //var sendingFrame = new sendData(sen.id, "F", sen.frameId);

  sp.write(xbeeAPI.buildFrame({
    type: xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
    id: sen.frameId,
    destination64: sen.idLong,
    data: [0x1]
  }));

  console.log("sending to ", sen.idLong);

  sen.deferred.promise.timeout(3000)
  .then(function(f){
    sen.counter.num--;
    console.log("command received correctly");
    sen.message.msg += "sensor " + sen.id + " turned on;";
    if(sen.counter.num == 0){
      res.json({"msg":sen.message.msg});
    }
  })
  .catch(function(e){
    sen.counter.num--;
    // remove dead sensor
    console.log("Removing dead sensor", sen.id);
    sen.message.msg += "sensor " + sen.id + " has died;";
    if(sen.counter.num == 0 && sensor.length > 0){
      res.json({"msg":sen.message.msg});
    } else {
      res.json({"msg":"All sensors have died"})
    }
    sensors.splice(sensor_num, 1);
  });
}

// starts the send getStatus procedure which includes creating a promise
function sendStatusCheck(sensor_num, count, res, msg){
  var sen = sensors[sensor_num];
  sen.res = res;
  sen.message = msg;
  sen.counter = count;
  sen.deferred = Q.defer();
  sen.frameId = xbeeAPI.nextFrameId();
  //var sendingFrame = new sendData(sen.id, "F", sen.frameId);

  sp.write(xbeeAPI.buildFrame({
    type: xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
    id: sen.frameId,
    destination64: sen.idLong,
    data: [0x8]
  }));

  console.log("sending to ", sen.idLong);

  sen.deferred.promise.timeout(3000)
  .then(function(f){
    console.log("command received correctly");
  })
  .catch(function(e){
    sen.counter.num--;
    // remove dead sensor
    console.log("Removing dead sensor", sen.id);
    sen.message.msg += "sensor " + sen.id + " has died;";
    sensors.splice(sensor_num, 1);
  });
}

function getStatus(frame){
  var sen = "";
  var idLong = frame.remote64;
  var data = frame.data[0];
  for(var i = 0; i < sensors.length; i++){
    sen = sensors[i];
    if(idLong == sen.idLong){
      sen.counter.num--;
      switch(data){
        case 0x0:
          sen.message.msg += "sensor " + sen.id + " is off;";
          break;
        case 0x1:
          sen.message.msg += "sensor " + sen.id + " is on;";
          break;
      }

      if(sen.counter.num == 0){
        console.log("about to send message");
        sen.res.json({"msg":sen.message.msg});
      }
      break;
    }
  }
}

var server = app.listen(3000, '0.0.0.0', function(){
  console.log("listening on *:3000");
});

app.get('/', function(req, res){
  res.sendfile("default.html");
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
    var msg = new message("");
    for(var i = 0; i < sensors.length; i++){
      turnOnLights(i, count, res, msg);
    }
  }
});

app.get('/turnOffAll', function(req, res){
  if(sensors.length == 0){
    res.status(400).json({"msg":"No sensors to turn off"});
  } else {
    var count = new counter(sensors.length);
    var msg = new message("");
    for(var i = 0; i < sensors.length; i++){
      turnOffLights(i, count, res, msg);
    }
  }
});

app.get('/getStatus', function(req, res){
  if(sensors.length == 0){
    res.status(400).json({"msg":"No sensors to get status"});
  } else {
    var count = new counter(sensors.length);
    var msg = new message("");
    for(var i = 0; i < sensors.length; i++){
      sendStatusCheck(i, count, res, msg);
    }
  }
})

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
  } else if (frame.type == xbeeConst.FRAME_TYPE.ZIGBEE_RECEIVE_PACKET){
    console.log("received status");
    getStatus(frame);
  }
});

sp.on("open",function(){
});
