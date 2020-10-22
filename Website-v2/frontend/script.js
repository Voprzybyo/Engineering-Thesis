// Setup declaration and definition
var choice = 1;
var timeSpan= 1;
var howMany = 0;
var xAxe = 0;
var obj = "";
var isOnline = 1;	// Flag using to sygnalize status of network
var SensorDataChart = null;

// Parsing corresponding part of data from node.js server
function getChartData(timeSpanIn, paramChoice) {
    $.get("/get_data_chart_" + paramChoice,  { timeSpan: timeSpanIn } , function (data) {	
	
        if(!data){
            alert("NO DATA!");
        }		
		if(paramChoice == 'hum'){
			obj = JSON.parse(data);
			howMany = Object.keys(obj.Hum).length;
			DrawChartHum(data);	
		}
		if(paramChoice == 'temp'){
			obj = JSON.parse(data);
			howMany = Object.keys(obj.Temp).length;
			DrawChartTemp(data);	
		}
		if(paramChoice == 'light'){
			obj = JSON.parse(data);
			howMany = Object.keys(obj.Light).length;
			DrawChartLight(data);	
		}
		if(paramChoice == 'pollution'){
			obj = JSON.parse(data);
			howMany = Object.keys(obj.VOC).length;
			DrawChartPollution(data);	
		}
		if(paramChoice == 'humPtemp'){
			obj = JSON.parse(data);
			howMany = Object.keys(obj.Hum).length;
			DrawChartHumPTemp(data);	
		}
		if(paramChoice == 'lightPtemp'){
			obj = JSON.parse(data);
			howMany = Object.keys(obj.Light).length;
			DrawChartLightPTemp(data);	
		}
		if(paramChoice == 'lightPlightState'){
			obj = JSON.parse(data);
			howMany = Object.keys(obj.Light).length;
			DrawChartLightPLightState(data);	
		}
		if(paramChoice == 'RSSIChart'){
			obj = JSON.parse(data);
			howMany = Object.keys(obj.RSSI_Leaf).length;
			DrawChartRSSI(data);	
		}
    })
}

// Parse data from cloud, format objects and put into certain cells in html file
function getDataFromCloud(choiceIn) {
    $.get("/get_data", function (data) {
		
	//Parse and format all available data
	var obj = JSON.parse(data);
	var temp1 = obj.Hum[0];
	var temp2 = obj.Temp[0];
	var temp3 = obj.Light[0];
	var temp4 = obj.VOC[0];
	var temp5 = obj.Button[0];
	var temp6 = obj.LightState[0];
	var temp7 = obj.RSSI_Leaf[0];
	var temp8 = obj.RSSI_MeshR[0];
	var temp9 = obj.NumNodesInNetwork[0];
	var temp10 = obj.addr_V4_Border[0];
	var temp11 = obj.IP_V6_Border[0];
	var temp12 = obj.IP_V6_MeshR[0];
	var temp13 = obj.IP_V6_Leaf[0];
	
	
	
	// Checking if network is online
	var timestamp = Date.now();	
	var ts_back = timestamp - (3600*0.15*1000);
			
	if(temp8.ts > ts_back){
		isOnline = 1;
	}else{
		isOnline = 0;
	}
	
	//Insert data to certain cells in HTML file	
	switch(choiceIn){
		
		// Main_Page.html and All_Sensors.html
		case 1:	
			document.getElementById("temperature").innerHTML = temp2.value + " °C</br>";					
			document.getElementById("humidity").innerHTML = temp1.value + " %</br>";				
			document.getElementById("light").innerHTML = temp3.value + " mV</br>";			
			document.getElementById("voc").innerHTML = temp4.value + " (VOC)</br>";
		
			if(temp5.value == 0){
				document.getElementById("boardButton").innerHTML = "Wciśnięty ";
			}else{
				document.getElementById("boardButton").innerHTML = "Nie wciśnięty ";		
			}
			
			if(temp6.value == 0){
				document.getElementById("lightState").innerHTML = "Wyłączone";
				document.getElementById("bulb").innerHTML = "<img src=bulb_off2.png>";		
			}else{
				document.getElementById("lightState").innerHTML = "Włączone";
				document.getElementById("bulb").innerHTML = "<img src=bulb_on2.png>";	
			}
			break;
		
		// Get timestamp
		case 2:	
			var s1 = new Date(temp1.ts).toLocaleDateString("en-GB")
			var s2 = new Date(temp1.ts).toLocaleTimeString("nb-NO")
			document.getElementById("timest").innerHTML = s2 + "</br>" + s1 +"</br>";
			break;

		// Dashboard.html
		case 3:		
			var timestamp = Date.now();
			var ts_back = timestamp - (3600*0.15*1000);
			
			if(temp8.ts > ts_back){				
				document.getElementById("RSSI1").innerHTML = temp8.value + " (RSSI)</br>";		
				document.getElementById("RSSI2").innerHTML = temp7.value + " (RSSI)</br>";
			}else{				
				document.getElementById("RSSI1").innerHTML = "Nie połączono</br>";
				document.getElementById("RSSI2").innerHTML = "Nie połączono</br>";
			}
			break;
			
		// Dashboard_detailed.html
		case 4:		
			var timestamp = Date.now();
			var ts_back = timestamp - (3600*0.15*1000);
			
			//Border Router
			if(temp1.ts > ts_back){
				document.getElementById("NetStat").innerHTML = "ONLINE</br>";
				document.getElementById("NetStatB").innerHTML = "ONLINE</br>";
				document.getElementById("BorderRSSI").innerHTML = temp8.value;
			}else{
				document.getElementById("NetStat").innerHTML = "OFFLINE</br>";
				document.getElementById("NetStatB").innerHTML = "OFFLINE</br>";
				document.getElementById("BorderRSSI").innerHTML = "Nie połączono</br>";
			}
			
			//Mesh Router
			if(temp8.ts > ts_back){
				document.getElementById("NetStatM").innerHTML = "ONLINE</br>";
				document.getElementById("MeshRSSI1").innerHTML = temp8.value;
				document.getElementById("MeshRSSI2").innerHTML = temp7.value;
			}else{
				document.getElementById("NetStatM").innerHTML = "OFFLINE</br>";
				document.getElementById("MeshRSSI1").innerHTML = "Nie połączono</br>";
				document.getElementById("MeshRSSI2").innerHTML = "Nie połączono</br>";
			}
			
			//Leaf Node
			if(temp8.ts > ts_back){
				document.getElementById("NetStatL").innerHTML = "ONLINE</br>";
				document.getElementById("LeafRSSI").innerHTML = temp7.value;
			}else{
				document.getElementById("NetStatL").innerHTML = "OFFLINE</br>";
				document.getElementById("LeafRSSI").innerHTML = "Nie połączono</br>";
			}
			
			if(temp1.ts > ts_back){
				document.getElementById("NumDev").innerHTML = temp9.value  ;
			}else{
				document.getElementById("NumDev").innerHTML = "0";
			}
			document.getElementById("BorderIPv4").innerHTML = temp10.value  ;
			document.getElementById("BorderIPv6").innerHTML = temp11.value  ;
			document.getElementById("BorderIPv6").innerHTML = temp11.value  ;
			document.getElementById("MeshRIPv6").innerHTML = temp12.value  ;
			document.getElementById("LeafRIPv6").innerHTML = temp13.value  ;
			break;
		default:

			break;
	}
		
    })
	setTimeout("getDataAll()",30000);
}

/*************DRAW CHARTS****************/

// Draw humidity chart in given timespan
function DrawChartHum(data) {
	
	var obj = JSON.parse(data);
	var tem = obj.Hum[0];
	
	var lebels = [];
    var data_chart = [];
	
	// Clear background -> remove previous chart
    if(SensorDataChart!=null){
        SensorDataChart.destroy();
    }
	
	//Check if network is online
	if(isOnline){
	
		// 1h
		if( howMany < 200 ){
			xAxe = 60;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				var tem = obj.Hum[i];
				lebels.push(-xAxe);
				data_chart.push(tem.value);
				xAxe--;
			}
		}
		
		// 3h
		if( howMany > 200 && howMany <700 ){
			xAxe = 180;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				
				var tem = obj.Hum[i];			
				lebels.push(-xAxe);
				data_chart.push(tem.value);
				xAxe--;
			}	
		}
		
		// 6h
		if( howMany > 700 && howMany <1400 ){
			xAxe = 6; 
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var tem = obj.Hum[i];			
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chart.push(tem.value);
				xAxe-=0.05;
			}
		}
		
		// 12h
		if( howMany > 1400 &&  howMany <3000 ){
			xAxe = 12;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var tem = obj.Hum[i];			
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chart.push(tem.value);
				xAxe-=0.05;
			}
		
		}
		
		// 24h
		if( howMany > 3000 &&  howMany <7000){
			xAxe = 24;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var tem = obj.Hum[i];			
				lebels.push(-xAxe);
				data_chart.push(tem.value);
				xAxe-=0.25;
			}
		}

		// 48h
		if( howMany > 7000 ){
			xAxe = 48;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var tem = obj.Hum[i];			
				lebels.push(-xAxe);
				data_chart.push(tem.value);
				xAxe-=0.25;
			}
		}
	
	}else{
		alert("Network is offline!");
	}
	
    var ctx = document.getElementById("ChartHum");
    SensorDataChart = new Chart(ctx, {
		
        type: 'line',

        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#00ccff',
                label: 'Wilgotność',
                lineTension: 0.5,
                data: data_chart,
				fill:false,						
            }]
        },
        options: {
            legend: {
                display: false
            },
            title: {
                display: true,
				fontColor: 'white',
				fontSize: 32
            },
			 scales: {
           	xAxes: [ {
            scaleLabel: {
			display: true,
            labelString: '[min] / [h]'
          },
          ticks: {
            major: {
              fontStyle: 'bold',
              fontColor: '#FF0000'
            }
          }
        } ],
            yAxes: [{				
                ticks: {
					suggestedMin: 25,		
					suggestedMax: 75						
                },
				scaleLabel: {
				display: true,
				labelString: '[%]'
				}				
            }]
        }
        }
    });		
	setTimeout("DrawChartHum()",30000);
}

// Draw temperature chart in given timespan
function DrawChartTemp(data) {
  var obj = JSON.parse(data);
	var tem = obj.Temp[0];
	
	var lebels = [];
    var data_chart = [];
	
	// Clear background -> remove previous chart
    if( SensorDataChart!=null ){
        SensorDataChart.destroy();
    }
	
	//Check if network is online
	if( isOnline ){
	
		// 1h
		if( howMany < 200 ){
		xAxe = 60;
			for(var i = howMany-1; i > 0 && xAxe > 0; i-=3){
				var tem = obj.Temp[i];
				lebels.push(-xAxe);
				data_chart.push(tem.value);
				xAxe--;
			}
		}
		
		// 3h
		if( howMany > 200 &&  howMany < 700 ){
		xAxe = 180;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				
				var tem = obj.Temp[i];			
				lebels.push(-xAxe);
				data_chart.push(tem.value);
				xAxe--;
			}
		}
		
		// 6h
		if( howMany > 700 &&  howMany <1400 ){
		xAxe = 6; 
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var tem = obj.Temp[i];			
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chart.push(tem.value);
				xAxe-=0.05;
			}	
		}
		
		// 12h
		if( howMany > 1400 &&  howMany <3000 ){
		xAxe = 12;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var tem = obj.Temp[i];			
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chart.push(tem.value);
				xAxe-=0.05;
			}
		}
		
		// 24h
		if( howMany > 3000 &&  howMany <7000 ){
		xAxe = 24;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var tem = obj.Temp[i];			
				lebels.push(-xAxe);
				data_chart.push(tem.value);
				xAxe-=0.25;
			}
		}
		
		// 48h
		if( howMany > 7000 ){
		xAxe = 48;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var tem = obj.Temp[i];			
				lebels.push(-xAxe);
				data_chart.push(tem.value);
				xAxe-=0.25;
			}
		}
	
	}else{
		alert("Network is offline!");
	}
	
    var ctx = document.getElementById("ChartTemp");
    SensorDataChart = new Chart(ctx, {
		
        type: 'line',

        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: 'red',
                label: 'Temperatura',
                lineTension: 0.5,
                data: data_chart,
				fill:false,		
				
            }]
        },
        options: {
            legend: {
                display: false

            },
            title: {
                display: true,
				fontColor: 'white',
				fontSize: 32
            },
			 scales: {
	xAxes: [ {
            scaleLabel: {
			display: true,
            labelString: '[min]'
          },
          ticks: {
            major: {
              fontStyle: 'bold',
              fontColor: '#FF0000'
            }
          }
        } ],
            yAxes: [{				
                ticks: {
					suggestedMin: 0,
					suggestedMax: 40,						
                },
				scaleLabel: {
				display: true,
				labelString: '[°C]'
				}
				
            }]
        }
        }
    });
			
	setTimeout("DrawChartTemp()",30000);
}

// Draw light chart in given timespan
function DrawChartLight(data) {
	
    var obj = JSON.parse(data);
	var tem = obj.Light[0];
	
	var lebels = [];
    var data_chart = [];
	
	// Clear background -> remove previous chart
    if( SensorDataChart!=null ){
        SensorDataChart.destroy();
    }
	
	//Check if network is online
	if( isOnline ){
	
		// 1h
		if( howMany < 200 ){
			xAxe = 60;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				var tem = obj.Light[i];
				lebels.push(-xAxe);
				data_chart.push(tem.value);
				xAxe--;
			}
		}
		
		// 3h
		if( howMany > 200 &&  howMany <700 ){
			xAxe = 180;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				
				var tem = obj.Light[i];			
				lebels.push(-xAxe);
				data_chart.push(tem.value);
				xAxe--;
			}
		}
		
		// 6h
		if( howMany > 700 &&  howMany <1400 ){
			xAxe = 6; 
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var tem = obj.Light[i];			
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chart.push(tem.value);
				xAxe-=0.05;
			}		
		}
		
		// 12h
		if( howMany > 1400 &&  howMany <3000 ){
			xAxe = 12;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var tem = obj.Light[i];			
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chart.push(tem.value);
				xAxe-=0.05;
			}		
		}
		
		// 24h
		if( howMany > 3000 &&  howMany <7000 ){
			xAxe = 24;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var tem = obj.Light[i];			
				lebels.push(-xAxe);
				data_chart.push(tem.value);
				xAxe-=0.25;
			}
		}
		
		// 48h
		if( howMany > 7000 ){
			xAxe = 48;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var tem = obj.Light[i];			
				lebels.push(-xAxe);
				data_chart.push(tem.value);
				xAxe-=0.25;
			}
		}
	
	}else{
		alert("Network is offline!");
	}
	
    var ctx = document.getElementById("ChartLight");
    SensorDataChart = new Chart(ctx, {
        type: 'line',

        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#FF8900',
                label: 'Światło',
                lineTension: 0.5,
                data: data_chart,
				fill:false,		
				
            }]
        },
        options: {
            legend: {
                display: false
            },
            title: {
                display: true,
				fontColor: 'white',
				fontSize: 32
            },	
			 scales: {
				xAxes: [ {
            scaleLabel: {
			display: true,
            labelString: '[min] / [h]'
          },
        } ],
            yAxes: [{				
                ticks: {
					suggestedMin: 0,					
                },
				scaleLabel: {
				display: true,
				labelString: '[mV]'
				}			
            }]
        }
        }
    });		
	setTimeout("DrawChartLight()",30000);
}

// Draw pollution chart in given timespan
function DrawChartPollution(data) {
    
	var obj = JSON.parse(data);
	var tem = obj.VOC[0];
	
	var lebels = [];
    var data_chart = [];
	
	// Clear background -> remove previous chart
    if( SensorDataChart!=null ){
        SensorDataChart.destroy();
    }
	
	//Check if network is online
	if( isOnline ){
		
			// 1h
			if( howMany < 200 ){
				xAxe = 60;
				for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
					var tem = obj.VOC[i];
					lebels.push(-xAxe);
					data_chart.push(tem.value);
					xAxe--;
				}
			}
			
			// 3h
			if( howMany > 200 &&  howMany <700 ){
				xAxe = 180;
				for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
					
					var tem = obj.VOC[i];			
					lebels.push(-xAxe);
					data_chart.push(tem.value);
					xAxe--;
				}
			}
			
			// 6h
			if( howMany > 700 &&  howMany <1400 ){
				xAxe = 6;
				for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
					
					var tem = obj.VOC[i];			
					lebels.push(Math.round((-xAxe)* 100) / 100);
					data_chart.push(tem.value);
					xAxe-=0.05;
				}		
			}
			
			// 12h
			if( howMany > 1400 &&  howMany <3000 ){
				xAxe = 12;
				for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
					
					var tem = obj.VOC[i];			
					lebels.push(Math.round((-xAxe)* 100) / 100);
					data_chart.push(tem.value);
					xAxe-=0.05;
				}			
			}
			
			// 24h
			if( howMany > 3000 &&  howMany <7000 ){
				xAxe = 24;
				for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
					
					var tem = obj.VOC[i];			
					lebels.push(-xAxe);
					data_chart.push(tem.value);
					xAxe-=0.25;
				}
			}
			
			// 48h
			if( howMany > 7000 ){
				xAxe = 48;
				for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
					
					var tem = obj.VOC[i];			
					lebels.push(-xAxe);
					data_chart.push(tem.value);
					xAxe-=0.25;
				}
			}
	}else{
		alert("Network is offline!");
	}
	
    var ctx = document.getElementById("ChartPollution");
    SensorDataChart = new Chart(ctx, {

        type: 'line',

        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#DCDCDC',
                label: 'Zanieczyszczenie',
                lineTension: 0.5,
                data: data_chart,
				fill:false,		
				
            }]
        },
        options: {
            legend: {
                display: false
            },
            title: {
                display: true,
				fontColor: 'white',
				fontSize: 32
            },
			 scales: {
				xAxes: [ {
            scaleLabel: {
			display: true,
            labelString: '[min] / [h]'
          },
        } ],
            yAxes: [{				
                ticks: {
					suggestedMin: 0,					
                },
				scaleLabel: {
				display: true,
				labelString: '[VOC]'
				}		
            }]
        }
        }
    });		
	setTimeout("DrawChartLight()",30000);
}

// Draw temperature and humidity chart in given timespan
function DrawChartHumPTemp(data) {
	
	var obj = JSON.parse(data);
	var tem1 = obj.Hum[0];
	var tem2 = obj.Temp[0];
	
	var lebels = [];
    var data_chartHum = [];
	var data_chartTemp = [];
	
	// Clear background -> remove previous chart
    if( SensorDataChart!=null ){
        SensorDataChart.destroy();
    }
	
	//Check if network is online
	if( isOnline ){
	
		// 1h
		if( howMany < 300 ){
		xAxe = 60;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				var tem1 = obj.Hum[i];
				var tem2 = obj.Temp[i];
				lebels.push(-xAxe);
				data_chartHum.push(tem1.value);
				data_chartTemp.push(tem2.value);
				xAxe--;
			}
		}
		
		// 3h
		if( howMany > 300 &&  howMany <700 ){
		xAxe = 180;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				
				var tem1 = obj.Hum[i];
				var tem2 = obj.Temp[i];
				lebels.push(-xAxe);
				data_chartHum.push(tem1.value);
				data_chartTemp.push(tem2.value);
				xAxe--;
			}	
		}
		
		// 6h
		if( howMany > 700 &&  howMany < 1400 ){
			xAxe = 6;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var tem1 = obj.Hum[i];
				var tem2 = obj.Temp[i];
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chartHum.push(tem1.value);
				data_chartTemp.push(tem2.value);
				xAxe-=0.05;
			}		
		}
		
		// 12h
		if( howMany > 1400 &&  howMany < 3000 ){
			xAxe = 12;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
					
				var tem1 = obj.Hum[i];
				var tem2 = obj.Temp[i];
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chartHum.push(tem1.value);
				data_chartTemp.push(tem2.value);
				xAxe-=0.05;
			}	
		}
		
		// 24h
		if( howMany > 3000 &&  howMany < 7000 ){
			xAxe = 24;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var tem1 = obj.Hum[i];
				var tem2 = obj.Temp[i];
				lebels.push(-xAxe);
				data_chartHum.push(tem1.value);
				data_chartTemp.push(tem2.value);
				xAxe-=0.25;
			}		
		}
		
		// 48h
		if( howMany > 7000 ){
			xAxe = 48;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var tem1 = obj.Hum[i];
				var tem2 = obj.Temp[i];
				lebels.push(-xAxe);
				data_chartHum.push(tem1.value);
				data_chartTemp.push(tem2.value);
				xAxe-=0.25;
			}
		}
		
	}else{
		alert("Network is offline!");
	}

    var ctx = document.getElementById("ChartHumPTemp");
    SensorDataChart = new Chart(ctx, {
		
        type: 'line',

        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#00ccff',
                label: 'Wilgotność',
                lineTension: 0.5,
                data: data_chartHum,
				fill:false,						
            },{
				backgroundColor: '#DCDCDC',
                borderColor: 'red',
                label: 'Temperatura',
                lineTension: 0.5,
                data: data_chartTemp,
				fill:false,				
				
			}]
        },
        options: {
            legend: {
                display: false
            },
            title: {
                display: true,
				fontColor: 'white',
				fontSize: 32
            },
			 scales: {
           	xAxes: [ {
            scaleLabel: {
			display: true,
            labelString: '[min] / [h]'
          },
          ticks: {
            major: {
              fontStyle: 'bold',
              fontColor: '#FF0000'
            }
          }
        } ],
            yAxes: [{				
                ticks: {
					suggestedMin: 25,		
					suggestedMax: 75						
                },
				scaleLabel: {
				display: true,
				labelString: '[%] / [°C]'
				}				
            }]
        }
        }
    });
		
	setTimeout("DrawChartHumPTemp()",30000);
}

// Draw temperature and light chart in given timespan
function DrawChartLightPTemp(data) {
	
	var obj = JSON.parse(data);
	var tem1 = obj.Light[0];
	var tem2 = obj.Temp[0];
	
	var lebels = [];
    var data_chartLight = [];
	var data_chartTemp = [];
	
	// Clear background -> remove previous chart
    if( SensorDataChart!=null ){
        SensorDataChart.destroy();
    }
	
	//Check if network is online
	if( isOnline ){
		
		// 1h
		if( howMany < 200 ){
			xAxe = 60;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				var tem1 = obj.Light[i];
				var tem2 = obj.Temp[i];
				lebels.push(-xAxe);
				data_chartLight.push(tem1.value);
				data_chartTemp.push(tem2.value*10);
				xAxe--;
			}
		}
		
		// 3h
		if( howMany > 200 && howMany <700 ){
			xAxe = 180;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				
				var tem1 = obj.Light[i];
				var tem2 = obj.Temp[i];
				lebels.push(-xAxe);
				data_chartLight.push(tem1.value);
				data_chartTemp.push(tem2.value*10);
				xAxe--;
			}
		}
		
		// 6h
		if( howMany > 700 &&  howMany <1400 ){
			xAxe = 6;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var tem1 = obj.Light[i];
				var tem2 = obj.Temp[i];
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chartLight.push(tem1.value);
				data_chartTemp.push(tem2.value*10);
				xAxe-=0.05;
			}	
		}
		
		// 12h
		if( howMany > 1400 &&  howMany <3000 ){
			xAxe = 12;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var tem1 = obj.Light[i];
				var tem2 = obj.Temp[i];
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chartLight.push(tem1.value);
				data_chartTemp.push(tem2.value*10);
				xAxe-=0.05;
			}
		
		}
		
		// 24h
		if( howMany > 3000 &&  howMany < 7000 ){
			xAxe = 24;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var tem1 = obj.Light[i];
				var tem2 = obj.Temp[i];
				lebels.push(-xAxe);
				data_chartLight.push(tem1.value);
				data_chartTemp.push(tem2.value*10);
				xAxe-=0.25;
			}		
		}
		
		// 48h
		if( howMany > 7000 ){
			xAxe = 48;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var tem1 = obj.Light[i];
				var tem2 = obj.Temp[i];
				lebels.push(-xAxe);
				data_chartLight.push(tem1.value);
				data_chartTemp.push(tem2.value*10);
				xAxe-=0.25;
			}
		}
	}else{
		alert("Network is offline!");
	}
	

    var ctx = document.getElementById("ChartLightPTemp");
    SensorDataChart = new Chart(ctx, {
        
        type: 'line',

        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#FF8900',
                label: 'Światło',
                lineTension: 0.5,
                data: data_chartLight,
				fill:false,						
            },{
				backgroundColor: '#DCDCDC',
                borderColor: 'red',
                label: 'Temperatura',
                lineTension: 0.5,
                data: data_chartTemp,
				fill:false,				
				
			}]
        },
        options: {
            legend: {
                display: false
            },
            title: {
                display: true,
				fontColor: 'white',
				fontSize: 32
            },
			 scales: {
           	xAxes: [ {
            scaleLabel: {
			display: true,
            labelString: '[min] / [h]'
          },
          ticks: {
            major: {
              fontStyle: 'bold',
              fontColor: '#FF0000'
            }
          }
        } ],
            yAxes: [{				
                ticks: {
					suggestedMin: 25,		
					suggestedMax: 75						
                },
				scaleLabel: {
				display: true,
				labelString: '[mV] / [°C]'
				}				
            }]
        }
        }
    });
		
	setTimeout("DrawChartLightPTemp()",30000);
}

// Draw light and light state chart in given timespan
function DrawChartLightPLightState(data) {
	
	var obj = JSON.parse(data);
	var tem1 = obj.Light[0];
	var tem2 = obj.LightState[0];
	
	var lebels = [];
    var data_chartLight = [];
	var data_chartLightState = [];
	
	// Clear background -> remove previous chart
    if( SensorDataChart!=null ){
        SensorDataChart.destroy();
    }
	
	//Check if network is online
	if( isOnline ){
		
		// 1h
		if( howMany < 300 ){
			xAxe = 60;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				var tem1 = obj.Light[i];
				var tem2 = obj.LightState[i];
				lebels.push(-xAxe);
				data_chartLight.push(tem1.value);
				data_chartLightState.push(tem2.value*100);
				xAxe--;
			}
		}
		
		// 3h
		if( howMany > 300 && howMany <700 ){
			xAxe = 180;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				
				var tem1 = obj.Light[i];
				var tem2 = obj.LightState[i];
				lebels.push(-xAxe);
				data_chartLight.push(tem1.value);
				data_chartLightState.push(tem2.value*100);
				xAxe--;
			}		
		}
		
		// 6h
		if( howMany > 700 && howMany <1400 ){
			xAxe = 6;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var tem1 = obj.Light[i];
				var tem2 = obj.LightState[i];
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chartLight.push(tem1.value);
				data_chartLightState.push(tem2.value*100);
				xAxe-=0.05;
			}
		}
		
		// 12h
		if( howMany > 1400 && howMany <3000 ){
			xAxe = 12;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var tem1 = obj.Light[i];
				var tem2 = obj.LightState[i];
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chartLight.push(tem1.value);
				data_chartLightState.push(tem2.value*100);
				xAxe-=0.05;
			}
		}
		
		// 24h
		if( howMany > 3000 &&  howMany < 7000 ){
			xAxe = 24;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var tem1 = obj.Light[i];
				var tem2 = obj.LightState[i];
				lebels.push(-xAxe);
				data_chartLight.push(tem1.value);
				data_chartLightState.push(tem2.value*100);
				xAxe-=0.25;
			}	
		}
		
		// 48h
		if( howMany > 7000 ){
			xAxe = 48;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var tem1 = obj.Light[i];
				var tem2 = obj.LightState[i];
				lebels.push(-xAxe);
				data_chartLight.push(tem1.value);
				data_chartLightState.push(tem2.value*100);
				xAxe-=0.25;
			}	
		}
		
	}else{
	alert("Network is offline!");
	}

    var ctx = document.getElementById("ChartLightPlightState");
    SensorDataChart = new Chart(ctx, {

        type: 'line',


        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#FF8900',
                label: 'Światło',
                lineTension: 0.5,
                data: data_chartLight,
				fill:false,						
            },{
				backgroundColor: '#DCDCDC',
                borderColor: '#DCDCDC',
                label: 'Oświetlenie',
                lineTension: 0.5,
                data: data_chartLightState,
				fill:false,				
				
			}]
        },
        options: {
            legend: {
                display: false
            },
            title: {
                display: true,
				fontColor: 'white',
				fontSize: 32
            },
			 scales: {
           	xAxes: [ {
            scaleLabel: {
			display: true,
            labelString: '[min] / [h]'
          },
          ticks: {
            major: {
              fontStyle: 'bold',
              fontColor: '#FF0000'
            }
          }
        } ],
            yAxes: [{				
                ticks: {
					suggestedMin: 25,		
					suggestedMax: 75						
                },
				scaleLabel: {
				display: true,
				labelString: '[mV] / [ON/OFF]'
				}				
            }]
        }
        }
    });		
	setTimeout("DrawChartLightPLightState()",30000);
}

// Draw light and light state chart in given timespan
function DrawChartRSSI(data) {
	
	var obj = JSON.parse(data);
	var ChartRSSI1 = obj.RSSI_MeshR[0];
	var ChartRSSI2 = obj.RSSI_Leaf[0];
	
	var lebels = [];
    var data_chartRSSI_MeshR = [];
	var data_chartRSSI_Leaf = [];
	
	// Clear background -> remove previous chart
    if(SensorDataChart!=null){
        SensorDataChart.destroy();
    }
	
	//Check if network is online
	if( isOnline ){
		
		// 1h
		if( howMany < 300 ){
			xAxe = 60;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				var ChartRSSI1 = obj.RSSI_MeshR[i];
				var ChartRSSI2 = obj.RSSI_Leaf[i];
				lebels.push(-xAxe);
				data_chartRSSI_MeshR.push(ChartRSSI1.value);
				data_chartRSSI_Leaf.push(ChartRSSI2.value);
				xAxe--;
			}
		}
		
		// 3h
		if( howMany > 300 &&  howMany <700 ){
			xAxe = 180;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=3 ){
				
				var ChartRSSI1 = obj.RSSI_MeshR[i];
				var ChartRSSI2 = obj.RSSI_Leaf[i];
				lebels.push(-xAxe);
				data_chartRSSI_MeshR.push(ChartRSSI1.value);
				data_chartRSSI_Leaf.push(ChartRSSI2.value);
				xAxe--;
			}		
		}
		
		// 6h
		if( howMany > 700 &&  howMany <1400 ){
			xAxe = 6;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var ChartRSSI1 = obj.RSSI_MeshR[i];
				var ChartRSSI2 = obj.RSSI_Leaf[i];
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chartRSSI_MeshR.push(ChartRSSI1.value);
				data_chartRSSI_Leaf.push(ChartRSSI2.value);
				xAxe-=0.05;
			}
		}
		
		// 12h
		if( howMany > 1400 &&  howMany <3000 ){
			xAxe = 12;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=9 ){
				
				var ChartRSSI1 = obj.RSSI_MeshR[i];
				var ChartRSSI2 = obj.RSSI_Leaf[i];
				lebels.push(Math.round((-xAxe)* 100) / 100);
				data_chartRSSI_MeshR.push(ChartRSSI1.value);
				data_chartRSSI_Leaf.push(ChartRSSI2.value);
				xAxe-=0.05;
			}
		}
		
		// 24h
		if( howMany > 3000 &&  howMany < 7000 ){
			xAxe = 24;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var ChartRSSI1 = obj.RSSI_MeshR[i];
				var ChartRSSI2 = obj.RSSI_Leaf[i];
				lebels.push(-xAxe);
				data_chartRSSI_MeshR.push(ChartRSSI1.value);
				data_chartRSSI_Leaf.push(ChartRSSI2.value);
				xAxe-=0.25;
			}	
		}
		
		// 48h
		if( howMany > 7000 ){
			xAxe = 48;
			for( var i = howMany-1; i > 0 && xAxe > 0; i-=45 ){
				
				var ChartRSSI1 = obj.RSSI_MeshR[i];
				var ChartRSSI2 = obj.RSSI_Leaf[i];
				lebels.push(-xAxe);
				data_chartRSSI_MeshR.push(ChartRSSI1.value);
				data_chartRSSI_Leaf.push(ChartRSSI2.value);
				xAxe-=0.25;
			}	
		}
	}else{
		alert("Network is ofline!");
	}

    var ctx = document.getElementById("ChartRSSI");
    SensorDataChart = new Chart(ctx, {
		
        type: 'line',

        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#FF6C00',
                label: 'Mesh Router (RSSI)',
                lineTension: 0.5,
                data: data_chartRSSI_MeshR,
				fill:false,						
            },{
				backgroundColor: '#DCDCDC',
                borderColor: '#00A4FF',
                label: 'Leaf Node (RSSI)',
                lineTension: 0.5,
                data: data_chartRSSI_Leaf,
				fill:false,				
				
			}]
        },
        options: {
            legend: {
                display: false
            },
            title: {
                display: true,
				fontColor: 'white',
				fontSize: 32
            },
			 scales: {
           	xAxes: [ {
            scaleLabel: {
			display: true,
            labelString: '[min] / [h]'
          },
          ticks: {
            major: {
              fontStyle: 'bold',
              fontColor: '#FF0000'
            }
          }
        } ],
            yAxes: [{				
                ticks: {
					suggestedMin: 100,		
					suggestedMax: 0						
                },
				scaleLabel: {
				display: true,
				labelString: '[RSSI]'
				}				
            }]
        }
        }
    });
		
	setTimeout("DrawChartRSSI()",30000);
}

/**************PARSE DATA***************/

// "Main_Page.html" and "All_Sensors.html" 
function getAllSensors() {
choice=1;
getDataFromCloud(choice);
setTimeout("getAllSensors()",30000);
}

// Timestamp -> last data update
function getTimeSt() {
choice=2;
getDataFromCloud(choice);
setTimeout("getTimeSt()",30000);
}

// "Dashboard.html" and "Dashboard_detailed.html"
function getRSSI() {
choice=3;
getDataFromCloud(choice);
setTimeout("getRSSI()",30000);
}

// "Main_Page.html" , "Dashboard.html" and "Dashboard_detailed.html"
function getNetworkStatus() {
choice=4;
getDataFromCloud(choice);
setTimeout("getNetworkStatus()",30000);
}

/**************SET TIMESPAN*************/

// The following functions set up time span and call "getChartData()" function
function getData1h(paramChoice) {
    timeSpan = 1;
    getChartData(timeSpan,paramChoice);
}
function getData3h(paramChoice) {
    timeSpan = 3;
    getChartData(timeSpan,paramChoice);
}
function getData6h(paramChoice) {
    timeSpan = 6;
    getChartData(timeSpan,paramChoice);
}
function getData12h(paramChoice) {
    timeSpan = 12;
    getChartData(timeSpan,paramChoice);
}
function getData24h(paramChoice) {
    timeSpan = 24;
    getChartData(timeSpan,paramChoice);
}
function getData48h(paramChoice) {
    timeSpan = 48;
    getChartData(timeSpan,paramChoice);
}
