var deviceName = "agh-noise-measurement-unit-1";
var timeSpan = "1h";


function getDataAll() {
	
    $.get("/get_data", function (data) {
	var obj = JSON.parse(data);
	var temp1 = obj.Hum[0];
	var temp2 = obj.Temp[0];
document.getElementById("tresc").innerHTML = "Wilgotnosc: " + temp1.value + "%</br>"
+"Temperatura: " + temp2.value + "st.";;

		
    })
	setTimeout("getDataAll()",1000);
}

function getData3h() {
    timeSpan = "3h";
    getSoundData(deviceName, timeSpan);
}

function getData6h() {
    timeSpan = "6h";
    getSoundData(deviceName, timeSpan);
}

function destroyCharts() {
    var ctx = document.getElementById("soundDev1");
    ctx.destroy();
}