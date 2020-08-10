
var choice = 1;
var LAeqChart = null;

function getDataAll(choiceIn) {
    $.get("/get_data", function (data) {
	var obj = JSON.parse(data);
	var temp1 = obj.Hum[0];
	var temp2 = obj.Temp[0];
	var temp3 = obj.Light[0];
	var temp4 = obj.VOC[0];
	var temp5 = obj.Button[0];
	
	switch(choiceIn){
		case 1:	
			document.getElementById("temperature").innerHTML = temp2.value + " °C</br>";		
			break;
		case 2:		
			document.getElementById("humidity").innerHTML = temp1.value + " %</br>";
			break;
		case 3:		
			document.getElementById("light").innerHTML = temp3.value + " mV</br>";
			break;
		case 4:		
			document.getElementById("voc").innerHTML = temp4.value + " (VOC)</br>";
			break;
		case 5:	
				if(temp5.value == 0){
			document.getElementById("boardButton").innerHTML = "Wciśnięty ";
				}else{
			document.getElementById("boardButton").innerHTML = "Nie wciśnięty ";		
				}
			break;
		case 6:	
		var s = new Date(temp1.ts).toLocaleTimeString("nb-NO")
		document.getElementById("timest").innerHTML = s + "</br>";
		default:

			break;
	}
		
    })
	setTimeout("getDataAll()",30000);
}


function DrawChartHum() {
    $.get("/get_data_chart_hum", function (data) {
		
	var obj2 = JSON.parse(data);
	var tem = obj2.Hum[0];
	var lebels = [];
    var data_chart = [];
    /*extract data form raw input */
    for(var i = 120; i >0; i-=2){
		var tem = obj2.Hum[i];
        lebels.push(-i/2);
        data_chart.push(tem.value);
    }

    /*Destorying previous chart */
    if(LAeqChart!=null){
        LAeqChart.destroy();
    }

    /*making a chart*/
    var ctx = document.getElementById("ChartHum");
    LAeqChart = new Chart(ctx, {
        // The type of chart we want to create
        type: 'line',

        // The data for our dataset
        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#00ccff',
                label: 'test',
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
		
		
    })
	setTimeout("DrawChartHum()",20000);
}

function DrawChartTemp() {
    $.get("/get_data_chart_temp", function (data) {
		
	var obj2 = JSON.parse(data);
	var tem = obj2.Temp[0];
	var lebels = [];
    var data_chart = [];
    /*extract data form raw input */
    for(var i = 180; i >0; i-=2){
		var tem = obj2.Temp[i];
        lebels.push(-i/2+30);
        data_chart.push(tem.value);
    }

    /*Destorying previous chart */
    if(LAeqChart!=null){
        LAeqChart.destroy();
    }

    /*making a chart*/
    var ctx = document.getElementById("ChartTemp");
    LAeqChart = new Chart(ctx, {
        // The type of chart we want to create
        type: 'line',

        // The data for our dataset
        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: 'red',
                label: 'test',
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
			
    })
	setTimeout("DrawChartTemp()",20000);
}


function DrawChartLight() {
    $.get("/get_data_chart_light", function (data) {
		
	var obj2 = JSON.parse(data);
	var tem = obj2.Light[0];
	var lebels = [];
    var data_chart = [];
    /*extract data form raw input */
    for(var i = 120; i >0; i-=2){
		var tem = obj2.Light[i];
        lebels.push(-i/2);
        data_chart.push(tem.value);
    }


    /*making a chart*/
    var ctx = document.getElementById("ChartLight");
    LAeqChart = new Chart(ctx, {
        // The type of chart we want to create
        type: 'line',

        // The data for our dataset
        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#FF8900',
                label: 'test',
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
			
    })
	setTimeout("DrawChartLight()",20000);
}



function getTemperature() {
choice=1;
getDataAll(choice);
}

function getHumidity() {
choice=2;
getDataAll(choice);
}

function getLight() {
choice=3;
getDataAll(choice);
}

function getVoc() {
choice=4;
getDataAll(choice);
}
function getButton() {
choice=5;
getDataAll(choice);
}
function getTimeSt() {
choice=6;
getDataAll(choice);
setTimeout("getTimeSt()",30000);

}

function getData6h() {
    timeSpan = "6h";
    getSoundData(deviceName, timeSpan);
}

function destroyCharts() {
    var ctx = document.getElementById("soundDev1");
    ctx.destroy();
}