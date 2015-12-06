var fs = require('fs');
var app = require('express')();
var SerialPort = require("serialport");
var Q = require("q");
var knn = require("alike");
var gpio = require('rpi-gpio');


//--------------------------------------------------------------------
//set of pin numbers
var pinUp = 1;
var pinDown = 2;
var pinLeft = 3;
var pinRight = 4;
var pinStop = 0;
var pinSpeed = 5;


//Setup and read the value of a pin
gpio.setup(7, gpio.DIR_IN, readInput);

function readInput() {
    gpio.read(7, function(err, value) {
        console.log('The value is ' + value);
    });
}

//Setup and write to a pin
gpio.setup(7, gpio.DIR_OUT, write);

function write() {
    gpio.write(7, true, function(err) {
        if (err) throw err;
        console.log('Written to pin 7');
    });
}



gpio.on('change', function(channel, value) {
    console.log('Channel ' + channel + ' value is now ' + value);
});
gpio.setup(7, gpio.DIR_IN, gpio.EDGE_BOTH);

//Unexport pins opened by the module when finished
/*var gpio = require('../rpi-gpio');

gpio.on('export', function(channel) {
    console.log('Channel set: ' + channel);
});

gpio.setup(7, gpio.DIR_OUT);
gpio.setup(15, gpio.DIR_OUT);
gpio.setup(16, gpio.DIR_OUT, pause);

function pause() {
    setTimeout(closePins, 2000);
}
*/
function closePins() {
    gpio.destroy(function() {
        console.log('All pins unexported');
    });
}

//-------------------------------------------------------------------------

var options = {
  k:5,
  weights:{
    beacon1:0.33,
    beacon2:0.33,
    beacon3:0.33
  }
}

var curLocation;

var sensors = [];
var mapDatas;

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

function mapData(){
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

// tells the sensor node to start sending back data to us
function startMapping(sensor_num, count, res, msg){
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
    data: [0x1]
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
    sen.message.msg += "Sensor " + sen.id + " has died;";
    if(sen.counter.num == 0 && sensor.length > 0){
      res.json({"msg":sen.message.msg});
    } else {
      res.json({"msg":"All sensors have died"})
    }
    sensors.splice(sensor_num, 1);
  });
}

function getData(frame){
  var sen = "";
  var idLong = frame.remote64;
  var data = frame.data[0];
  for(var i = 0; i < sensors.length; i++){
    sen = sensors[i];
    if(idLong == sen.idLong){
      sen.counter.num--;
      var dataItem = new mapData();
  	  for(var i = 1; i < (frame.data.length); i += 2){
        dataItem['beacon'+frame.data[i]] = frame.data[i+1];
  		  //mapData.set('beacon'+frame.data[i], frame.data[i+1]);
  	  }
      console.log(dataItem);
      console.log(knn(dataItem, mapDatas, options));

      var chosenData = knn(dataItem, mapDatas, options);

      sen.message.msg += JSON.stringify(dataItem);
      if(sen.counter.num == 0){
        console.log("about to send message");
        sen.res.json(chosenData);
      }
      break;
    }
  }
}

var server = app.listen(3000, '0.0.0.0', function(){
  console.log("listening on *:3000");
});

app.get('/', function(req, res){
  res.sendfile("default.html"); //return the mapping page
});

app.get('/startMapping', function(req, res){
  if(sensors.length == 0){
    res.json({"msg":"No sensors to start mapping with;"});
  } else {
    var count = new counter(sensors.length);
    var msg = new message("");
    for(var i = 0; i < sensors.length; i++){
      startMapping(i, count, res, msg);
    }
  }
});

xbeeAPI.on("frame_object", function(frame){
  console.log(frame); // for debug purposes
  if(frame.type == xbeeConst.FRAME_TYPE.NODE_IDENTIFICATION){
    console.log(frame.sender64);
    addSensor(frame.sender16, frame.sender64);
  } else if (frame.type == xbeeConst.FRAME_TYPE.ZIGBEE_TRANSMIT_STATUS){
    checkStatus(frame);
  } else if (frame.type == xbeeConst.FRAME_TYPE.ZIGBEE_RECEIVE_PACKET){
    console.log("received mapping data");
    console.log(frame.data);
	  if(frame.data[0] == 2)
	  {
		  getData(frame);
	  }
  }
});

sp.on("open",function(){
});

// load map data from our previous mapping activity
mapDatas = JSON.parse(fs.readFileSync(process.argv[3], 'utf-8'));
