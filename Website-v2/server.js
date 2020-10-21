	const express = require('express');
	const app = express();
	const fetch = require('node-fetch');
	var fs = require('fs');
	var PORT = process.env.PORT || 8080;
	
	//Access token -> it has to be changed from time to time
	var token = "Bearer eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJ3by5wcnp5YnlvQGdtYWlsLmNvbSIsInNjb3BlcyI6WyJURU5BTlRfQURNSU4iXSwidXNlcklkIjoiN2RjNmQ0OTAtZDFjMC0xMWVhLThjMzYtODdhODUyZTM5NTJjIiwiZmlyc3ROYW1lIjoiV29qY2llY2giLCJsYXN0TmFtZSI6IlByenliecWCbyIsImVuYWJsZWQiOnRydWUsInByaXZhY3lQb2xpY3lBY2NlcHRlZCI6dHJ1ZSwiaXNQdWJsaWMiOmZhbHNlLCJ0ZW5hbnRJZCI6IjdjMDgwZDkwLWQxYzAtMTFlYS04YzM2LTg3YTg1MmUzOTUyYyIsImN1c3RvbWVySWQiOiIxMzgxNDAwMC0xZGQyLTExYjItODA4MC04MDgwODA4MDgwODAiLCJpc3MiOiJ0aGluZ3Nib2FyZC5pbyIsImlhdCI6MTYwMzE4ODE4OCwiZXhwIjoxNjA0OTg4MTg4fQ.3bLyfWfzDp6Ub6KhfAd9bqPlJBxtYF-wtiHZcaab6AS19JG7XZAw8PyjB3Mg9Jp3BHLi-Rxhj15Ul3KVITadow";
	
	
app.use(express.static(__dirname ) )

	
// Get all data now
app.get('/get_data', function (request, response) {

    console.log("Pozyskiwanie danych z chmury");

    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?keys=Hum,Temp,Light,Button,VOC,LightState,RSSI_Leaf,RSSI_MeshR,NumNodesInNetwork,addr_V4_Border,IP_V6_Border,IP_V6_MeshR,IP_V6_Leaf" , 
	{
        headers: 
		{
            "Accept": "application/json",
            "X-Authorization": token
        }
    })
  .then(function (response) {
            return response.json();
        })
  .then(function (data) {
	  console.log(data);
           return response.send(JSON.stringify(data));
        });
  
})

// Get humidity data from cloud
app.get('/get_data_chart_hum', function (request, response) {
    console.log("Pozyskiwanie danych z chmury");
	var timestamp = Date.now();
	
	
	var ileOdjac = request.query.timeSpan;
	
	var tsWindow = timestamp - (3600*ileOdjac*1000);
	
	
    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?interval=60000&limit=100000&agg=NONE&useStrictDataTypes=false&keys=Hum&startTs="+tsWindow+"&endTs="+timestamp ,
	{
        headers: 
		{
            "Accept": "application/json",
            "X-Authorization": token
        }
    })
  .then(function (response) {
            return response.json();
        })
  .then(function (data) {
	  	  console.log(data);

           return response.send(JSON.stringify(data));
        });
  
})

// Get temperature data from cloud
app.get('/get_data_chart_temp', function (request, response) {

    console.log("Pozyskiwanie danych z chmury");
	var timestamp = Date.now();
	
	var ileOdjac = request.query.timeSpan;
	
	var tsWindow = timestamp - (3600*ileOdjac*1000);

    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?interval=60000&limit=100000&agg=NONE&useStrictDataTypes=false&keys=Temp&startTs="+tsWindow+"&endTs="+timestamp ,
	{
        headers: 
		{
            "Accept": "application/json",
            "X-Authorization": token
        }
    })
  .then(function (response) {
            return response.json();
        })
  .then(function (data) {
	  console.log(data);

           return response.send(JSON.stringify(data));
        });
  
})

// Get light data from cloud
app.get('/get_data_chart_light', function (request, response) {

    console.log("Pozyskiwanie danych z chmury");
	var timestamp = Date.now();
	
	
	var ileOdjac = request.query.timeSpan;
	
	var tsWindow = timestamp - (3600*ileOdjac*1000);
    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?interval=60000&limit=100000&agg=NONE&useStrictDataTypes=false&keys=Light&startTs="+tsWindow+"&endTs="+timestamp ,
	{
        headers: 
		{
            "Accept": "application/json",
            "X-Authorization": token
        }
    })
  .then(function (response) {
            return response.json();
        })
  .then(function (data) {
	  console.log(data);
	  console.log(timestamp);
           return response.send(JSON.stringify(data));
        });
  
})

// Get pollution data from cloud
app.get('/get_data_chart_pollution', function (request, response) {

    console.log("Pozyskiwanie danych z chmury");
	var timestamp = Date.now();
		
	var ileOdjac = request.query.timeSpan;
	
	var tsWindow = timestamp - (3600*ileOdjac*1000);
    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?interval=60000&limit=100000&agg=NONE&useStrictDataTypes=false&keys=VOC&startTs="+tsWindow+"&endTs="+timestamp ,
	{
        headers: 
		{
            "Accept": "application/json",
            "X-Authorization": token
        }
    })
  .then(function (response) {
            return response.json();
        })
  .then(function (data) {
	  console.log(data);
	  console.log(timestamp);
           return response.send(JSON.stringify(data));
        });
  
})

// Get humidity and temperature data from cloud
app.get('/get_data_chart_humPtemp', function (request, response) {
    console.log("Pozyskiwanie danych z chmury");
	var timestamp = Date.now();
	
	
	var ileOdjac = request.query.timeSpan;
	
	var tsWindow = timestamp - (3600*ileOdjac*1000);
	
	
    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?interval=60000&limit=100000&agg=NONE&useStrictDataTypes=false&keys=Hum,Temp&startTs="+tsWindow+"&endTs="+timestamp ,
	{
        headers: 
		{
            "Accept": "application/json",
            "X-Authorization": token
        }
    })
  .then(function (response) {
            return response.json();
        })
  .then(function (data) {
	  	  console.log(data);

           return response.send(JSON.stringify(data));
        });
  
})

// Get temperature and light data from cloud
app.get('/get_data_chart_lightPtemp', function (request, response) {
    console.log("Pozyskiwanie danych z chmury");
	var timestamp = Date.now();
	
	
	var ileOdjac = request.query.timeSpan;
	
	var tsWindow = timestamp - (3600*ileOdjac*1000);
	
	
    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?interval=60000&limit=100000&agg=NONE&useStrictDataTypes=false&keys=Light,Temp&startTs="+tsWindow+"&endTs="+timestamp ,
	{
        headers: 
		{
            "Accept": "application/json",
            "X-Authorization": token
        }
    })
  .then(function (response) {
            return response.json();
        })
  .then(function (data) {
	  	  console.log(data);

           return response.send(JSON.stringify(data));
        });
  
})

// Get light and light state data from cloud
app.get('/get_data_chart_lightPlightState', function (request, response) {
    console.log("Pozyskiwanie danych z chmury");
	var timestamp = Date.now();
	
	
	var ileOdjac = request.query.timeSpan;
	
	var tsWindow = timestamp - (3600*ileOdjac*1000);
	
	
    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?interval=60000&limit=100000&agg=NONE&useStrictDataTypes=false&keys=Light,LightState&startTs="+tsWindow+"&endTs="+timestamp ,
	{
        headers: 
		{
            "Accept": "application/json",
            "X-Authorization": token
        }
    })
  .then(function (response) {
            return response.json();
        })
  .then(function (data) {
	  	  console.log(data);

           return response.send(JSON.stringify(data));
        });
  
})

// Get RSSI data from cloud
app.get('/get_data_chart_RSSIChart', function (request, response) {
    console.log("Pozyskiwanie danych z chmury");
	var timestamp = Date.now();
	
	
	var ileOdjac = request.query.timeSpan;
	
	var tsWindow = timestamp - (3600*ileOdjac*1000);
	
	
    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?interval=60000&limit=100000&agg=NONE&useStrictDataTypes=false&keys=RSSI_MeshR,RSSI_Leaf&startTs="+tsWindow+"&endTs="+timestamp ,
	{
        headers: 
		{
            "Accept": "application/json",
            "X-Authorization": token
        }
    })
  .then(function (response) {
            return response.json();
        })
  .then(function (data) {
	  	  console.log(data);

           return response.send(JSON.stringify(data));
        });
  
})



// Put some info to localhost server (console) to inform that everything is OK -> debugging purposes
app.listen(PORT, function(){
    console.log("Serwer lokalny został uruchomiony na porcie 8080");
});