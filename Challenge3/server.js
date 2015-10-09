var app = require('express')();
var SerialPort = require("serialport");

app.get('/', function(req, res){
  res.send("hello");
});

app.post('/test', function(req, res){
  res.json({"test":"huh"});
})

var server = app.listen(3000, '0.0.0.0', function(){
  console.log("listening on *:3000");
});
