/* How to make an alarm entry:
<div class="alarm-container4 deletMe">
<span type="" class="alarm-textinput input">
	9:04
</span>
</div>
*/

var alarmEntryString = "<div class=\"alarm-container4 deletMe\"><span class=\"alarm-textinput input\">";
var alarmEntryString2 = "</span></div>";

function createElementFromHTML(htmlString) {
	var div = document.createElement('div');
	div.innerHTML = htmlString.trim();
  
	// Change this to div.childNodes to support multiple top-level nodes.
	return div.firstChild;
  }

var alarmList = [];
var previousAlarmCount = 0;
//Update alarm list
function onInterval() {
	makeRequest("./getAlarms", function(ret){
		var arr = ret.split(",");
		if (arr.length != previousAlarmCount || true) {
			previousAlarmCount = arr.length;
			var container = document.getElementById("topContainer");
			var delets = document.querySelectorAll(".deletMe");
			for (var q = 0; q < delets.length; q++) {
				delets[q].remove();
			}
			for (var q = 0; q < arr.length; q++) {
				container.appendChild(createElementFromHTML(alarmEntryString + arr[q] + alarmEntryString2));
			}
		}
	});
}
onInterval();
setInterval(onInterval, 1000);

function getUserAlarmTimeString() {
	var timeArray = document.getElementById("userAlarm").value.split(":");
	var once = document.getElementById("onceCheck").checked;
	var ledCheck = document.getElementById("ledCheck").checked;
	var ringCheck = document.getElementById("buzzerCheck").checked;
	var ret = "hour=" + parseInt(timeArray[0]) + "&minute=" + parseInt(timeArray[1]);
	if (once) {
		ret = ret + "&once=1"
	}
	if (ledCheck) {
		ret = ret + "&light=1"
	}
	if (ringCheck) {
		ret = ret + "&ring=1"
	}
	return ret;
}

function deleteAlarmButtonClick() {
	makeRequest("./deleteAlarm", onInterval, getUserAlarmTimeString());
}

function createAlarmButtonClick() {
	makeRequest("./createAlarm", onInterval, getUserAlarmTimeString());
}


