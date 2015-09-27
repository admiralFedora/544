var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var array = [];
var gcounter = 0;
var colorSet = ["rgba(19,78,15,0.8)","rgba(252,227,15,0.8)","rgba(252,43,15,0.8)","rgba(58,188,18,0.8)","rgba(255,176,24,0.8)","rgba(28,164,251,0.8)","rgba(140,15,176,0.8)","rgba(176,15,114,0.8)",
"rgba(127,2,3,0.8)","rgba(72,72,72,0.8)","rgba(168,255,0,0.8)","rgba(0,36,85,0.8)","rgba(3,119,82,0.8)"]// 12 main colors;

var dataSet = [];// most recently 20 temp records for all sensors; ************
/**/
function printD(){
	console.log("\n\n"+"----------------------------------------------------------");
	for(i= 0 ; i< dataSet.length; i++){
		console.log(dataSet[i].id +"  | "+ dataSet[i].temp + " | "+ dataSet[i].rgba+ " | "+dataSet[i].startTime)
	}
	console.log("----------------------------------------------------------------"+"\n\n");
}

/**/
// sensor object
function sensor(id, temp, counter){
  this.temp = temp // int
  this.success = 0; // boolean
  this.alive = 1; // boolean
  this.id = id; // long
  this.counter = 0;
}

/* add new sensor data to dataSet & generate a random rgb color*************/
function addSenData(id,temp){
 	 var d = new Date();
	 var n = d.getTime();
	 var ran = Math.floor( Math.random() * 12 ) + 1;
	 var rgba;

	 if( ran % colorSet.length < colorSet.length){ rgba = colorSet[ran-1]; colorSet.splice(ran-1,1);}//remove the color that has been used
	 else{
		 switch(ran % 5) { //randomly generate
			  case 0:
		 rgba = "rgba("+id % 10 + 130+"," + 130  + ","+ ran % 5*35  +",0.8)";
		 break;
		    case 1:
			rgba = "rgba("+id % 4 * ran +"," + ran % 6 * 40 + ","+ 25  +",0.8)";
			break;
		    case 2:
			rgba = "rgba("+ 230 +"," + id % 10 + 120  + "," +ran%5*25  +",0.8)";
			break;
		    case 3:
			rgba = "rgba("+28 +"," +ran % 5 *25  + ","+ id % 10 + 120  +",0.8)";
			break;
		    case 4:
			rgba = "rgba("+ran%7 *25 +"," + id % 10 + 130  + ","+45 +",0.8)";
			break;
	 };
  }

	 dataSet.push({"id":id,"temp":[temp],"rgba":rgba,"startTime":n});
}

function deleteSenData(id){
	for(i = 0; i < dataSet.length; i++){
		if(dataSet[i].id == id){
			dataSet.splice(i,1);
			return;
		}
	}
}

/*update existed sensor's new temp**************************************************/
function updateSenData(data){
  var curData = JSON.parse(data);
	var id = curData.id;
	var temp = curData.temp;
	for(i = 0; i < dataSet.length; i++){
		//console.log("i: "+ i + "  dataSet.length: "+dataSet.length);
		//console.log("id: "+id +"  dataid: "+dataSet[i].id);

		if(id == dataSet[i].id){
			dataSet[i].temp.push(temp);  //add a new temp
			//console.log("dataSet["+i+"].temp: "+dataSet[i].temp+"\n");
			if(dataSet[i].temp.length > 20){     //maxmum 20 values
				dataSet[i].temp.splice(0, 1);
			}
			return;
		}
	}
	addSenData(id,temp);

}

// function to record data on a per object basis
function recordData(data){
  var JsonData = JSON.parse(data);
  var sen;
  if(array.length == 0){
    array.push(new sensor(JsonData.id, JsonData.temp, gcounter));
    addSenData(JsonData.id, JsonData.temp);   //********************* add data that not exist
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
  /*update data that existed*************************************************************/

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
			deleteSenData(sen.id);
    } else {
      console.log("sensor " + sen.id + " temp: " + sen.temp + "*C\n");
      average += sen.temp;
      i++;
    }
  }
  average /= array.length;
  console.log("Average temp: " + average.toFixed(2) + "*C\n");
  console.log("Number of devices: " + array.length + "\n\n\n\n\n");
  gcounter++;
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
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
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
    updateSenData(data);
		printD();// for testing
    io.emit("chat message", dataSet);
  });
  // every 5 seconds we'll read from our list of sensors
  setInterval(printAverage, 10000);
  // send data to html every 5 sec ******************************************

});
