
var http = require('http')
const hostname = '192.168.7.2';
const PORT_NUMBER = 8088;
var indexRoute = require('./routes/indexRoute');
var checkRoute = require('./routes/checkRoute');


/* 
 * Create the static web app
 */
const express = require('express');
var app = express();
var server = http.createServer(app).listen(PORT_NUMBER);
app.use('/', indexRoute);
app.use('/nodeCheck', checkRoute);
/*
 * Create the Userver to listen for the websocket
 */
var udpServer = require('./lib/udp_server');
udpServer.listen(server);

