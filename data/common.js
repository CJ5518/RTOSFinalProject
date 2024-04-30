
function emptyFunc() {}
function makeRequest(url, onRes, data) {
	var xhr = new XMLHttpRequest();

	xhr.onreadystatechange = function () {
		if (this.readyState != 4) return;

		if (this.status == 200) {
			onRes(this.responseText);
		}
	};

	xhr.open('POST', url, true);
	xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
	xhr.send(data);
}


