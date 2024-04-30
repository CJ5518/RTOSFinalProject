
//Read temp, humidity, and time from the server
var isC = true;
function onInterval() {
	makeRequest("./getTmp", function(res){
		var temp = parseFloat(res);
		if (!isC) {
			temp = (temp * (9/5)) + 32;
		}
		document.getElementById("temp").innerText = temp.toString() + " " + (isC ? "C" : "F");
	}, '');
	makeRequest("./getHumid", function(res){
		document.getElementById("humid").innerText = res;
	}, '');
	makeRequest("./getTime", function(res){
		document.getElementById("time").innerText = res;
	}, '');
}
onInterval();

function switchTempFormat() {
	isC = !isC;
	onInterval();
}

setInterval(onInterval, 1000);

