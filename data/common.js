

function makeRequest(url, onRes) {
	var xhr = new XMLHttpRequest();

	xhr.onreadystatechange = function () {
		if (this.readyState != 4) return;

		if (this.status == 200) {
			onRes(this.responseText);
		}
	};

	xhr.open('POST', url, true);
	xhr.send();
}


