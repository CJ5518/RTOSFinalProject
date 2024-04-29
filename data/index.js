
//Read temp, humidity, and time from the server
function onInterval() {
	makeRequest("./getTmp", function(){

	});
	makeRequest("./getHumid", function(){

	});
	makeRequest("./getTime", function(){

	});
}

setInterval(onInterval, 1000);

