var serialPort = require('serialport');
var mysql = require('mysql');
var connection = mysql.createConnection({
  host  : 'localhost',
  user  : 'root',
  password  : 'root',
  database  : 'mydb'
});

connection.connect();

function sensorExist(id, x, y){
  connection.query('SELECT * FROM `sensors` WHERE `id`=?', [id],function(error, row, fields){
    if(!error){
      if(row.length != 0){
        // do nothing if sensor is found
      } else {
        addSensor(id, x, y);
      }
    } else {
      console.log('oh noes\n');
      return false;
    }
  });
}

function addSensor(id, x, y){
  connection.query('INSERT INTO `sensors` VALUES (?, ?, ?)', [id, x, y],function(error, row, fields){
    if(!error){
      console.log("new sensor was added\n");
    } else {
      console.log('oh noes\n');
      return false;
    }
  });
}
