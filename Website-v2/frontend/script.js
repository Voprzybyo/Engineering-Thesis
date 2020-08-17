
var choice = 1;
var timeSpan= 1;
var howMany = 0;
getSoundData(timeSpan);
var xAxe = 0;
var obj2 = "";


function getSoundData(timeSpanIn, paramChoice) {
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
	
	if(howMany < 300){
	xAxe = 60;
		for(var i = howMany-1; i > 0; i-=4){
			var tem = obj2.Hum[i];
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe--;
		}
	}
	if(howMany > 300 &&  howMany <1000){
	xAxe = 180;
		for(var i = howMany-1; i > 0; i-=12){
			
			var tem = obj2.Hum[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=3;
		}
	
	}
	if( howMany > 1000 ){
	xAxe = 24;
		for(var i = howMany-24; i > 0; i-=28){
			
			var tem = obj2.Hum[i];			
			lebels.push(-xAxe/4);
			data_chart.push(tem.value);
			xAxe-=0.5;
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
	
	if(howMany < 300){
	xAxe = 60;
		for(var i = howMany-1; i > 0; i-=4){
			var tem = obj2.Temp[i];
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe--;
		}
	}
	if(howMany > 300 &&  howMany <1000){
	xAxe = 180;
		for(var i = howMany-1; i > 0; i-=12){
			
			var tem = obj2.Temp[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=3;
		}
	
	}
	if( howMany > 1000 ){
	xAxe = 24;
		for(var i = howMany-24; i > 0; i-=28){
			
			var tem = obj2.Temp[i];			
			lebels.push(-xAxe/4);
			data_chart.push(tem.value);
			xAxe-=0.5;
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
	
	if(howMany < 300){
	xAxe = 60;
		for(var i = howMany-1; i > 0; i-=4){
			var tem = obj2.Light[i];
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe--;
		}
	}
	if(howMany > 300 &&  howMany <1000){
	xAxe = 180;
		for(var i = howMany-1; i > 0; i-=12){
			
			var tem = obj2.Light[i];			
			lebels.push(-xAxe);
			data_chart.push(tem.value);
			xAxe-=3;
		}
	
	}
	if( howMany > 1000 ){
	xAxe = 24;
		for(var i = howMany-24; i > 0; i-=28){
			
			var tem = obj2.Light[i];			
			lebels.push(-xAxe/4);
			data_chart.push(tem.value);
			xAxe-=0.5;
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
			
	setTimeout("DrawChartLight()",30000);
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

function getData1h(paramChoice) {
    timeSpan = 1;
    getSoundData(timeSpan,paramChoice);
}

function getData3h(paramChoice) {
    timeSpan = 3;
    getSoundData(timeSpan,paramChoice);
}

function getData6h(paramChoice) {
    timeSpan = 6;
    getSoundData(timeSpan,paramChoice);
}

function destroyCharts() {
    var ctx = document.getElementById("soundDev1");
    ctx.destroy();
}