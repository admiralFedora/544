var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);


/*Four sensors' data   Initialization */
var id1 = {temp:0,sum:0,count:0};
var id2 = {temp:0,sum:0,count:0};
var id3 = {temp:0,sum:0,count:0};
var id4 = {temp:0,sum:0,count:0};

function SaveData(data){
    var result = ""; 
    var JsonData = JSON.parse(data);  //parse the current data
    /*need calibration*/
    switch(JsonData.id) {
    	case 1:
        	id1.temp = JsonData.temp;
		id1.sum += JsonData.temp;
		id1.count += 1;
        	break;
    	case 2:
        	id2.temp = JsonData.temp;
		id2.sum += JsonData.temp;
		id2.count += 1;
        	break;
        case 3:
        	id3.temp = JsonData.temp;
		id3.sum += JsonData.temp;
		id3.count += 1;
        	break;
	case 4:
        	id4.temp = JsonData.temp;
		id4.sum += JsonData.temp;
		id4.count += 1;
        	break;
    }
    result += "\n"+ "id 1:  CurTemp: " + id1.temp + " AveTemp: " + (id1.sum/id1.count).toFixed(2)  + "\n" +
    	     "id 2:  Curtemp: " + id2.temp + " AveTemp: " + (id2.sum/id2.count).toFixed(2)  + "\n" +
	     "id 3:  Curtemp: " + id3.temp + " AveTemp: " + (id3.sum/id3.count).toFixed(2)  + "\n" +
	     "id 4:  Curtemp: " + id4.temp + " AveTemp: " + (id4.sum/id4.count).toFixed(2)  + "\n";
    return result;    
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
    var RecData = SaveData(data);  //String for printing received Data
    console.log('data received:' + RecData );
    io.emit("chat message",  "Received Data:" + RecData);
  });
});

