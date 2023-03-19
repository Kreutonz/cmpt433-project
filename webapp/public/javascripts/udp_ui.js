"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
var timeoutError;
var timeoutUDP;
$(document).ready(function() {

	setInterval(checkSecond, 1000);

	$('#def1').click(function(){
		sendCommand("playDefault1\n");
	});
	
	$('#def2').click(function(){
		sendCommand("playDefault2\n");
	});
	
	$('#def3').click(function(){
		sendCommand("playDefault3\n");
	});
	$('#stop').click(function(){
		sendCommand("playStop\n");
	});
	$('#cs1').click(function(){
		sendCommand("playCustom1\n");
	});
	$('#cs2').click(function(){
		sendCommand("playCustom2\n");
	});
	$('#setAlarm').click(function(){
		sendAlarm("setAlarmTime,"+$('#hourInput').val() +","+$('#minuteInput').val()+","+$('#secInput').val());
		console.log("setAlarmTime,"+$('#hourInput').val() +","+$('#minuteInput').val()+","+$('#secInput').val());
	});
	
	socket.on('commandReply', function(result) {
		var commasep = result.split(',');
		if (commasep[0] === "bbgdata") {
			clearTimeout(timeoutUDP);
			updateStats(commasep);
		}
	});
	
});



function showError(reason) {
	clearTimeout(timeoutError);
	if (reason === "node") {
		var ebox = document.getElementById("error-box");
		ebox.style.display = "block";
		ebox.innerHTML = "<h1>Server Error!</h1><br><h2>Server error detected - unable to reach NodeJS Server.</h2>";
		timeoutError = setTimeout(function () {
			ebox.innerHTML = "";
			ebox.style.display = "none";
		}, 10000);
	}
	else {
		var ebox = document.getElementById("error-box");
		ebox.style.display = "block";
		if (ebox.innerHTML === "<h1>Server Error!</h1><br><h2>Server error detected - unable to reach NodeJS Server.</h2>") {
			ebox.innerHTML = "<h1>Server Error!</h1><br><h2>Server error detected - unable to reach NodeJS Server.</h2>";
		}
		else {
			ebox.innerHTML = "<h1>Server Error!</h1><br><h2>Server error detected - unable to reach Beatbox C program.</h2>";
		}
		timeoutError = setTimeout(function () {
			ebox.innerHTML = "";
			ebox.style.display = "none";
		}, 10000);
	}
}

function checkSecond() {
	sendCommand("check");
	$.ajax({
		url: "/nodeCheck",
		type: 'GET',
		dataType: 'json',
		error : function(err) {
		  console.log('Error!', err);
		  //showError("node");

		  $('#cstatus').text("Connection Status: Not Connected");
		  $('#cstatus').css('color','red')
		},
		success: function(data) {
		  console.log('Node Server still going!');
		}
	  });
	  timeoutUDP = setTimeout(function () {
		//showError("udp");
		$('#cstatus').text("Connection Status: Not Connected");
		$('#cstatus').css('color','red')
	}, 1000);
}

function updateStats(commasep) {


	$('#cstatus').text("Connection Status: Connected");
	$('#cstatus').css('color','green')
	var beat = document.getElementById("modeid");
}

function sendCommand(message) {
	socket.emit('prime', message);
};

function sendAlarm(message) {
	socket.emit('prime', message);
};