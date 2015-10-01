var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var array = [];
var gcounter = 0;
var colorSet = ["rgba(19,78,15,0.8)","rgba(252,227,15,0.8)","rgba(252,43,15,0.8)","rgba(58,188,18,0.8)","rgba(255,176,24,0.8)","rgba(28,164,251,0.8)","rgba(140,15,176,0.8)","rgba(176,15,114,0.8)",
"rgba(127,2,3,0.8)","rgba(72,72,72,0.8)","rgba(168,255,0,0.8)","rgba(0,36,85,0.8)","rgba(3,119,82,0.8)"]// 12 main colors;

var dataSet = [{"type":"Update"}];// most recently 20 temp records for all sensors ************
var GdataReturn;

var mysql = require('mysql');
var connection = mysql.createConnection({
  host  : 'localhost',
	port	:	'3306',
  user  : 'root',
  password  : 'root',
  database  : 'mydb'
});

function printD(){
	console.log("\n\n"+"----------------------------------------------------------");
	for(i= 0 ; i< dataSet.length; i++){
		console.log(dataSet[i].id +"  | "+ dataSet[i].temp + " | "+ dataSet[i].rgba+ " | "+dataSet[i].startTime)
	}
	console.log("----------------------------------------------------------------"+"\n\n");
}

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
	 var rgba = "rgba(19,143,150,0.8)";

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
		    default:
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
		if(id == dataSet[i].id){
			dataSet[i].temp.push(temp);  //add a new temp
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
	addTemperature(JsonData.id,JsonData.temp,getTimeStamp());
  for(var i = 0; i < array.length; i++){
    sen = array[i];
    if(sen.id == JsonData.id){ //update
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
	insertAverage(average.toFixed(2));
  gcounter++;
}

function getTimeStamp() {

    var date = new Date();

    var hour = date.getHours();
    hour = (hour < 10 ? "0" : "") + hour;

    var min  = date.getMinutes();
    min = (min < 10 ? "0" : "") + min;

    var sec  = date.getSeconds();
    sec = (sec < 10 ? "0" : "") + sec;

    var year = date.getFullYear();

    var month = date.getMonth() + 1;
    month = (month < 10 ? "0" : "") + month;

    var day  = date.getDate();
    day = (day < 10 ? "0" : "") + day;

    return year + "-" + month + "-" + day + " " + hour + ":" + min + ":" + sec;
}

function addSensor(id, x, y, temp, time){
  connection.query('INSERT INTO `sensors` VALUES (?, ?, ?)', [id, x, y],function(error, row, fields){
    if(!error){
      console.log("new sensor was added\n");
      addTemperature(id, temp, time);
    } else {
      console.log('oh noes\n');
      return false;
    }
  });
}

// time is formated as YYYY-MM-DD HH:MM:SS
function addTemperature(id, temp, time){
  connection.query('INSERT INTO `temperatures` (time, temp, sensors_id) VALUES (?, ?, ?)', [time, temp, id], function(error, row, fields){
    if(!error){
      //added
    } else {
      console.log("oh noes\n");
      console.log(error);
      addSensor(id, 0, 0, temp, time);
    }
  });
}

function insertAverage(value){
  connection.query('INSERT INTO `average` (average, time) VALUES (?, ?)', [value, getTimeStamp()], function(error, row, fields){
    if(!error){
      // do stuff here
    } else {
      console.log("error inserting average\n");
      console.log(error);
    }
  });
}

// time is formated as YYYY-MM-DD HH:MM:SS
function addTemperature(id, temp, time){
  connection.query('INSERT INTO `temperatures` (time, temp, sensors_id) VALUES (?, ?, ?)', [getTimeStamp(), temp, id], function(error, row, fields){
    if(!error){
      //added temp
    } else {
      console.log("oh noes\n");
      console.log(error);
      addSensor(id, 0, 0, temp, time);
    }
  });
}

function querySensors(){
  connection.query('SELECT id FROM `sensors`', function(error, row, fields){
    if(!error){
      var resSet = [{"type":"QueryIdReturn"}];
      resSet.push.apply(resSet, row)
      io.emit("chat message", resSet); //send back the ids;
    } else {
      console.log("error querying sensors\n");
      console.log(error);
    }
  })
};

function queryTempBySensor(id){
  connection.query('SELECT * FROM `temperatures` WHERE `sensors_id`=? ORDER BY `time` ASC', [id], function(error, row, fields){
    if(!error){
			console.log(row);
    } else {
      console.log("oh noes\n");
      console.log(error);
    }
  });
}

function queryAverage(){
  connection.query('SELECT * FROM `average` ORDER BY `time` ASC', function(error, row, fields){
    if(!error){
      // do stuff here
    } else {
      console.log("oh noes\n");
    }
  });
}

function queryTempBySensorRange(id, time1, time2){
  console.log(id);
  connection.query('SELECT * FROM `temperatures` WHERE `sensors_id`=? AND `time` BETWEEN ? AND ?', [id, time1, time2], function(error, row, fields){
    console.log("queryTempBySensorRange");
    if(!error){
      console.log(row);
      GdataReturn = [{"type":"QueryRangeReturn"}];
      row = limitResult(row);
      GdataReturn.push.apply(GdataReturn, row);
      console.log(GdataReturn);
      io.emit("chat message", GdataReturn); //send back the temps;
      //queryAverageRange(time1, time2);
    } else {
      console.log("error encountered\n");
      console.log(error);
    }
  })
}


function queryAverageRange(time1, time2){
  connection.query('SELECT * FROM `average` WHERE `time` BETWEEN ? AND ?', [time1, time2], function(error, row, fields){
    if(!error){
      GdataReturn = [{"type":"QueryRangeReturn"}];
      row = limitResult(row);
      GdataReturn.push.apply(GdataReturn, row);
      io.emit("chat message", GdataReturn); //send back the temps;
    } else {
      console.log("error occured");
      console.log(error);
    }
  });
}

function limitResult(ReturnData){
  var repeatData = [];
  if (ReturnData.length > 500){
    for(i = 1; i < ReturnData.length; i +=20){
      repeatData.push(ReturnData[i]);
    };
  }else if(ReturnData.length > 100){
    for(i = 1; i < ReturnData.length; i +=10){
      repeatData.push(ReturnData[i]);
    };
  }else if(ReturnData.length > 50){
    for(i = 1; i < ReturnData.length; i +=5){
      repeatData.push(ReturnData[i]);
    };
  }else{
    for(i = 1; i < ReturnData.length; i +=2){
      repeatData.push(ReturnData[i]);
    };
  };
  return repeatData;
}

connection.connect();

app.get('/', function(req, res){
  res.sendfile('historical.html');
});

io.on('connection', function(socket){
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
    /*Responses*/
    if(msg[0].type == "queryId"){querySensors(); return;}
    else if(msg[0].type == "queryRange"){  //add to GdataReturn
      /*for(i =0; i < msg[0].id.length; i++){
        console.log("queryRange");
        GdataReturn = [];
        queryTempBySensorRange(msg[0].id[i], msg[0].time1.toString(), msg[0].time2.toString());
      }*/
      GdataReturn = [];
      queryTempBySensorRange(msg[0].id, msg[0].time1.toString(), msg[0].time2.toString());
      return;
    }
    else if(msg[0].type == "queryId"){querySensors(); return;}
    /*End*/
  });
});


http.listen(3001, function(){
  console.log('listening on *:3001');
});
