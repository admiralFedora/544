var Q = require('q');
var app = require('express')();

var count = 0;
var objects = [];

function testObj(id){
  this.id = id;
  var deferred;
}

function findObj(obj){
  for(var i = 0; i < objects.length; i++){
    var j = objects[i];
    if(j.id = obj.id){
      return i;
    }
  }

  return -1;
}

function createPromise(obj){
  obj.deferred = Q.defer();

  obj.deferred.promise.timeout(15000)
    .then(function(){
      console.log("promise resolved", obj.id);
      var i = findObj(obj);
      objects.splice(i, 1);
    })
      .catch(function(e){
        console.log(e);
        var i = findObj(obj);
        objects.splice(i, 1);
      });
}

function stopPromise(obj){
  obj.deferred.resolve();
}

var server = app.listen(3000, '0.0.0.0', function(){
  console.log("listening on *:3000");
});

app.get('/start', function(req, res){
  console.log("starting promise");
  var obj = new testObj(count);
  objects.push(obj);
  createPromise(obj);
  count++;
  res.send("Promise created");
});

app.get('/stop', function(req, res){
  console.log("stopping promise");
  if(objects.length > 0){
    stopPromise(objects[0]);
    res.send("stopped a promise");
  } else {
    res.send("No promises to stop");
  }
});
