
var express = require('express');
var http = require('http');
var bodyParser = require('body-parser');
var ejs = require('ejs');
//var database = require('./database');
//var alert = require('alert-node');
var qs = require('querystring');

var app = express();
var expressWs = require('express-ws')(app);

//app.set('view engine', 'html');

let arduino_switch = "OFF"; 
let arduinoSocket;

app.get('/', function(req, res){
    //arduinoSocket.send(arduino_switch);
    res.sendfile('./views/home.html');
})
 
app.get('/arduino_switch/on', function(req,res){
    arduino_switch = true;
    arduinoSocket.send("ON");
    res.redirect('/');
})

app.get('/arduino_switch/off', function(req,res){
    arduino_switch = false;
    arduinoSocket.send("OFF");
    res.redirect('/');
})


app.ws('/arduino', function(ws, req) {
    arduinoSocket = ws;
    ws.on('message', function(msg) {
      console.log(msg);
    });
    console.log('socket', req.testing);
});
const activeConnections = [];
app.ws('/admin', function(ws, req) {
    activeConnections.push(ws);
    if(arduino_switch=="OFF")
      ws.send("ON");
    else if(arduino_switch=="ON")
      ws.send("OFF");
    ws.on('message', function(msg) {
      arduino_switch = msg;
      arduinoSocket.send(msg);
      console.log(arduino_switch);
      console.log(`sending to ${activeConnections.length} connections`);
      activeConnections.forEach((socket)=>{
        try{
            if(arduino_switch=="OFF")
            socket.send("ON");
            else if(arduino_switch=="ON")
            socket.send("OFF");
        }catch(e){}
      });
    });
    console.log('socket', req.testing);
});
   

app.listen(8000);
