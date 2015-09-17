var SerialPort = require("serialport");

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
    console.log("New sensor " + JsonData.id + " has come");
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
  console.log("New sensor " + JsonData.id + " has come");
}

function printAverage(){
  var sen;
  var average = 0;
  var i = 0;
  while(i < array.length){
    sen = array[i];
    // we are giving our sensors 3 tries before counting them dead
    if((gcounter - sen.counter) >= 3){
      console.log("sensor " + sen.id + " has died\n");
      // remove dead sensor from list
      array.splice(i, 1);
    } else {
      console.log("sensor " + sen.id + " temp: " + sen.temp + "*C\n");
      average += sen.temp;
      i++;
    }
  }
  average /= array.length;
  console.log("Average temp: " + average + "*C\n");
  console.log("Number of devices: " + array.length + "\n");
  gcounter++;
}

var portName = process.argv[2],
portConfig = {
  baudRate: 9600,
  parser: SerialPort.parsers.readline("\n")
};

var sp;  //serial Port

sp = new SerialPort.SerialPort(portName, portConfig);

sp.on("open", function () {
  console.log('open');
  sp.on('data', function(data) {
    recordData(data);
  });
  // every 15 seconds we'll read from our list of sensors
  setInterval(printAverage, 15000);
});
