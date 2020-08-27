
var choice = 1;
var timeSpan= 1;
var howMany = 0;
var xAxe = 0;
var obj2 = "";


function getChartData(timeSpanIn, paramChoice) {
    $.get("/get_data_chart_" + paramChoice,  { timeSpan: timeSpanIn } , function (data) {	
	
console.log(paramChoice);
	
        if(!data){
            alert("NO DATA!");
        }
		
		
		if(paramChoice == 'hum'){
				obj2 = JSON.parse(data);
				howMany = Object.keys(obj2.Hum).length;
        DrawChartHum(data);	
		}
		if(paramChoice == 'temp'){
				obj2 = JSON.parse(data);
				howMany = Object.keys(obj2.Temp).length;
        DrawChartTemp(data);	
		}
		if(paramChoice == 'light'){
				obj2 = JSON.parse(data);
				howMany = Object.keys(obj2.Light).length;
        DrawChartLight(data);	
		}
		if(paramChoice == 'pollution'){
				obj2 = JSON.parse(data);
				howMany = Object.keys(obj2.VOC).length;
        DrawChartPollution(data);	
		}
		if(paramChoice == 'humPtemp'){
				obj2 = JSON.parse(data);
				howMany = Object.keys(obj2.Hum).length;
        DrawChartHumPTemp(data);	
		}
		if(paramChoice == 'lightPtemp'){
				obj2 = JSON.parse(data);
				howMany = Object.keys(obj2.Light).length;
        DrawChartLightPTemp(data);	
		}
		if(paramChoice == 'lightPlightState'){
				obj2 = JSON.parse(data);
				howMany = Object.keys(obj2.Light).length;
        DrawChartLightPLightState(data);	
		}

    })
}


function getDataAll(choiceIn) {
    $.get("/get_data", function (data) {
	var obj = JSON.parse(data);
	var temp1 = obj.Hum[0];
	var temp2 = obj.Temp[0];
	var temp3 = obj.Light[0];
	var temp4 = obj.VOC[0];
	var temp5 = obj.Button[0];
	var temp6 = obj.LightState[0];
	
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
			var s1 = new Date(temp1.ts).toLocaleDateString("en-US")
			var s2 = new Date(temp1.ts).toLocaleTimeString("nb-NO")
			document.getElementById("timest").innerHTML = s2 + "</br>" + s1 +"</br>";
			break;
		case 7:	
			if(temp6.value == 0){
			document.getElementById("lightState").innerHTML = "Wyłączone";
			document.getElementById("bulb").innerHTML = "<img src=bulb_off2.png>";		
				}else{
			document.getElementById("lightState").innerHTML = "Włączone";
			document.getElementById("bulb").innerHTML = "<img src=bulb_on2.png>";	
				}
			
			break;
		
		default:

			break;
	}
		
    })
	setTimeout("getDataAll()",30000);
}

function DrawChartHum(data) {
	
	var obj2 = JSON.parse(data);
	var tem = obj2.Hum[0];
	
	var lebels = [];
    var data_chart = [];
	
		if(howMany < 200){
	xAxe = 60;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=3){
			var tem = obj2.Hum[i];
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe--;
		}
	}
	if(howMany > 200 &&  howMany <700){
	xAxe = 180;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=9){
			
			var tem = obj2.Hum[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=3;
		}
	
	}
	if( howMany > 700 &&  howMany <1400){
	xAxe = 6; //co 15 min
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem = obj2.Hum[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	
	}
	
	if( howMany > 1400 &&  howMany <3000){
	xAxe = 12;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem = obj2.Hum[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	
	}
	
	if( howMany > 3000 &&  howMany <7000){
	xAxe = 24;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem = obj2.Hum[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	}
	
		if( howMany > 7000 ){
	xAxe = 48;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=35){
			
			var tem = obj2.Hum[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	}
	
	

    var ctx = document.getElementById("ChartHum");
    LAeqChart = new Chart(ctx, {
        // The type of chart we want to create
        type: 'line',


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

function DrawChartTemp(data) {
  var obj2 = JSON.parse(data);
	var tem = obj2.Temp[0];
	
	var lebels = [];
    var data_chart = [];
	
		if(howMany < 200){
	xAxe = 60;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=3){
			var tem = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe--;
		}
	}
	if(howMany > 200 &&  howMany <700){
	xAxe = 180;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=9){
			
			var tem = obj2.Temp[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=3;
		}
	
	}
	if( howMany > 700 &&  howMany <1400){
	xAxe = 6; //co 15 min
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem = obj2.Temp[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	
	}
	
	if( howMany > 1400 &&  howMany <3000){
	xAxe = 12;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem = obj2.Temp[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	
	}
	
	if( howMany > 3000 &&  howMany <7000){
	xAxe = 24;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem = obj2.Temp[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	}
	
		if( howMany > 7000 ){
	xAxe = 48;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=35){
			
			var tem = obj2.Temp[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	}
	
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
			
	setTimeout("DrawChartTemp()",30000);
}

function DrawChartLight(data) {
    var obj2 = JSON.parse(data);
	var tem = obj2.Light[0];
	
	var lebels = [];
    var data_chart = [];
	
	if(howMany < 200){
	xAxe = 60;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=3){
			var tem = obj2.Light[i];
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe--;
		}
	}
	if(howMany > 200 &&  howMany <700){
	xAxe = 180;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=9){
			
			var tem = obj2.Light[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=3;
		}
	
	}
	if( howMany > 700 &&  howMany <1400){
	xAxe = 6; //co 15 min
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem = obj2.Light[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	
	}
	
	if( howMany > 1400 &&  howMany <3000){
	xAxe = 12;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem = obj2.Light[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	
	}
	
	if( howMany > 3000 &&  howMany <7000){
	xAxe = 24;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem = obj2.Light[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	}
	
		if( howMany > 7000 ){
	xAxe = 48;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=35){
			
			var tem = obj2.Light[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	}
	
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

function DrawChartPollution(data) {
    var obj2 = JSON.parse(data);
	var tem = obj2.VOC[0];
	
	var lebels = [];
    var data_chart = [];
	
		if(howMany < 200){
	xAxe = 60;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=3){
			var tem = obj2.VOC[i];
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe--;
		}
	}
	if(howMany > 200 &&  howMany <700){
	xAxe = 180;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=9){
			
			var tem = obj2.VOC[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=3;
		}
	
	}
	if( howMany > 700 &&  howMany <1400){
	xAxe = 6; //co 15 min
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem = obj2.VOC[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	
	}
	
	if( howMany > 1400 &&  howMany <3000){
	xAxe = 12;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem = obj2.VOC[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	
	}
	
	if( howMany > 3000 &&  howMany <7000){
	xAxe = 24;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem = obj2.VOC[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	}
	
		if( howMany > 7000 ){
	xAxe = 48;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=35){
			
			var tem = obj2.VOC[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=0.25;
		}
	}
	
    var ctx = document.getElementById("ChartPollution");
    LAeqChart = new Chart(ctx, {
        // The type of chart we want to create
        type: 'line',

        // The data for our dataset
        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#DCDCDC',
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

function DrawChartHumPTemp(data) {
	
	var obj2 = JSON.parse(data);
	var tem1 = obj2.Hum[0];
	var tem2 = obj2.Temp[0];
	
	var lebels = [];
    var data_chartHum = [];
	var data_chartTemp = [];
	
	if(howMany < 300){
	xAxe = 60;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=3){
			var tem1 = obj2.Hum[i];
			var tem2 = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chartHum.push(tem1.value);
			data_chartTemp.push(tem2.value);
			xAxe--;
		}
	}
	
	if(howMany > 300 &&  howMany <700){
	xAxe = 180;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=9){
			
			var tem1 = obj2.Hum[i];
			var tem2 = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chartHum.push(tem1.value);
			data_chartTemp.push(tem2.value);
			xAxe-=3;
		}
	
	}
	if(howMany > 700 &&  howMany < 1400){
	xAxe = 6;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem1 = obj2.Hum[i];
			var tem2 = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chartHum.push(tem1.value);
			data_chartTemp.push(tem2.value);
			xAxe-=0.25;
		}
	
	}
	
		if(howMany > 1400 &&  howMany < 3000){
	xAxe = 12;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem1 = obj2.Hum[i];
			var tem2 = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chartHum.push(tem1.value);
			data_chartTemp.push(tem2.value);
			xAxe-=0.25;
		}
	
	}
	
	if(howMany > 3000 &&  howMany < 7000){
			xAxe = 24;
			for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem1 = obj2.Hum[i];
			var tem2 = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chartHum.push(tem1.value);
			data_chartTemp.push(tem2.value);
			xAxe-=0.25;
		}
	
	}
	
		if(howMany > 7000){
			xAxe = 48;
			for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem1 = obj2.Hum[i];
			var tem2 = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chartHum.push(tem1.value);
			data_chartTemp.push(tem2.value);
			xAxe-=0.25;
		}
	
	}


    var ctx = document.getElementById("ChartHumPTemp");
    LAeqChart = new Chart(ctx, {
        // The type of chart we want to create
        type: 'line',


        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#00ccff',
                label: 'test',
                lineTension: 0.5,
                data: data_chartHum,
				fill:false,						
            },{
				backgroundColor: '#DCDCDC',
                borderColor: 'red',
                label: 'test',
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
				labelString: '[%]'
				}				
            }]
        }
        }
    });
		
	setTimeout("DrawChartHumPTemp()",30000);
}

function DrawChartLightPTemp(data) {
	
	var obj2 = JSON.parse(data);
	var tem1 = obj2.Light[0];
	var tem2 = obj2.Temp[0];
	
	var lebels = [];
    var data_chartLight = [];
	var data_chartTemp = [];
	
	if(howMany < 200){
	xAxe = 60;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=3){
			var tem1 = obj2.Light[i];
			var tem2 = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chartLight.push(tem1.value);
			data_chartTemp.push(tem2.value);
			xAxe--;
		}
	}
	
	if(howMany > 200 &&  howMany <700){
	xAxe = 180;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=9){
			
			var tem1 = obj2.Light[i];
			var tem2 = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chartLight.push(tem1.value);
			data_chartTemp.push(tem2.value);
			xAxe-=3;
		}
	
	}
	if(howMany > 700 &&  howMany <1400){
	xAxe = 6;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem1 = obj2.Light[i];
			var tem2 = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chartLight.push(tem1.value);
			data_chartTemp.push(tem2.value);
			xAxe-=0.25;
		}
	
	}
	
		if(howMany > 1400 &&  howMany <3000){
	xAxe = 12;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem1 = obj2.Light[i];
			var tem2 = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chartLight.push(tem1.value);
			data_chartTemp.push(tem2.value);
			xAxe-=0.25;
		}
	
	}
		if(howMany > 3000 &&  howMany < 7000){
			xAxe = 24;
			for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem1 = obj2.Light[i];
			var tem2 = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chartLight.push(tem1.value);
			data_chartTemp.push(tem2.value);
			xAxe-=0.25;
		}
	
	}
	
		if(howMany > 7000){
			xAxe = 48;
			for(var i = howMany-1; i > 0 && xAxe > 0; i-=35){
			
			var tem1 = obj2.Light[i];
			var tem2 = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chartLight.push(tem1.value);
			data_chartTemp.push(tem2.value);
			xAxe-=0.25;
		}
	
	}
	

    var ctx = document.getElementById("ChartLightPTemp");
    LAeqChart = new Chart(ctx, {
        // The type of chart we want to create
        type: 'line',


        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#FF8900',
                label: 'test',
                lineTension: 0.5,
                data: data_chartLight,
				fill:false,						
            },{
				backgroundColor: '#DCDCDC',
                borderColor: 'red',
                label: 'test',
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
				labelString: '[%]'
				}				
            }]
        }
        }
    });
		
	setTimeout("DrawChartLightPTemp()",30000);
}

function DrawChartLightPLightState(data) {
	
	var obj2 = JSON.parse(data);
	var tem1 = obj2.Light[0];
	var tem2 = obj2.LightState[0];
	
	var lebels = [];
    var data_chartLight = [];
	var data_chartLightState = [];
	
	if(howMany < 300){
	xAxe = 60;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=3){
			var tem1 = obj2.Light[i];
			var tem2 = obj2.LightState[i];
			lebels.push(-xAxe);
			data_chartLight.push(tem1.value);
			data_chartLightState.push(tem2.value*100);
			xAxe--;
		}
	}
	
	if(howMany > 300 &&  howMany <700){
	xAxe = 180;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=9){
			
			var tem1 = obj2.Light[i];
			var tem2 = obj2.LightState[i];
			lebels.push(-xAxe);
			data_chartLight.push(tem1.value);
			data_chartLightState.push(tem2.value*100);
			xAxe-=3;
		}
	
	}
	if(howMany > 700 &&  howMany <1400){
	xAxe = 6;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem1 = obj2.Light[i];
			var tem2 = obj2.LightState[i];
			lebels.push(-xAxe);
			data_chartLight.push(tem1.value);
			data_chartLightState.push(tem2.value*100);
			xAxe-=0.25;
		}
	}
	
		if(howMany > 1400 &&  howMany <3000){
	xAxe = 12;
		for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem1 = obj2.Light[i];
			var tem2 = obj2.LightState[i];
			lebels.push(-xAxe);
			data_chartLight.push(tem1.value);
			data_chartLightState.push(tem2.value*100);
			xAxe-=0.25;
		}
	}
	
		if(howMany > 3000 &&  howMany < 7000){
			xAxe = 24;
			for(var i = howMany-1; i > 0 && xAxe > 0; i-=45){
			
			var tem1 = obj2.Light[i];
			var tem2 = obj2.LightState[i];
			lebels.push(-xAxe);
			data_chartLight.push(tem1.value);
			data_chartLightState.push(tem2.value);
			xAxe-=0.25;
		}
	
	}
	
		if(howMany > 7000){
			xAxe = 48;
			for(var i = howMany-1; i > 0 && xAxe > 0; i-=35){
			
			var tem1 = obj2.Light[i];
			var tem2 = obj2.LightState[i];
			lebels.push(-xAxe);
			data_chartLight.push(tem1.value);
			data_chartLightState.push(tem2.value);
			xAxe-=0.25;
		}
	
	}
	

    var ctx = document.getElementById("ChartLightPlightState");
    LAeqChart = new Chart(ctx, {
        // The type of chart we want to create
        type: 'line',


        data: {
            labels: lebels,
            datasets: [{
                backgroundColor: '#DCDCDC',
                borderColor: '#FF8900',
                label: 'test',
                lineTension: 0.5,
                data: data_chartLight,
				fill:false,						
            },{
				backgroundColor: '#DCDCDC',
                borderColor: '#DCDCDC',
                label: 'test',
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
				labelString: '[%]'
				}				
            }]
        }
        }
    });
		
	setTimeout("DrawChartLightPLightState()",30000);
}



function getTemperature() {
choice=1;
getDataAll(choice);
setTimeout("getTemperature()",30000);
}
function getHumidity() {
choice=2;
getDataAll(choice);
setTimeout("getHumidity()",30000);
}
function getLight() {
choice=3;
getDataAll(choice);
setTimeout("getLight()",30000);
}
function getVoc() {
choice=4;
getDataAll(choice);
setTimeout("getVoc()",30000);
}
function getButton() {
choice=5;
getDataAll(choice);
setTimeout("getButton()",30000);
}
function getTimeSt() {
choice=6;
getDataAll(choice);
setTimeout("getTimeSt()",30000);

}
function getLightState() {
choice=7;
getDataAll(choice);
setTimeout("getLightState()",30000);
}

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
