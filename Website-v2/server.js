	const express = require('express');
	const app = express();
	const fetch = require('node-fetch');
	var fs = require('fs');
	var PORT = process.env.PORT || 8080;
	var token = "Bearer eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJ3by5wcnp5YnlvQGdtYWlsLmNvbSIsInNjb3BlcyI6WyJURU5BTlRfQURNSU4iXSwidXNlcklkIjoiN2RjNmQ0OTAtZDFjMC0xMWVhLThjMzYtODdhODUyZTM5NTJjIiwiZmlyc3ROYW1lIjoiV29qY2llY2giLCJsYXN0TmFtZSI6IlByenliecWCbyIsImVuYWJsZWQiOnRydWUsInByaXZhY3lQb2xpY3lBY2NlcHRlZCI6dHJ1ZSwiaXNQdWJsaWMiOmZhbHNlLCJ0ZW5hbnRJZCI6IjdjMDgwZDkwLWQxYzAtMTFlYS04YzM2LTg3YTg1MmUzOTUyYyIsImN1c3RvbWVySWQiOiIxMzgxNDAwMC0xZGQyLTExYjItODA4MC04MDgwODA4MDgwODAiLCJpc3MiOiJ0aGluZ3Nib2FyZC5pbyIsImlhdCI6MTU5Njk2OTQ0MywiZXhwIjoxNTk4NzY5NDQzfQ.P7tZrkFXNf50BCEkBC674J36TZCUh_LRGxOtHd2oIjliy7K9BKoMj6_wSxAQ3bd4scF4ByZYgPcTQZl7_zn2eQ";
	
	
app.use(express.static(__dirname ) )

app.get('/', function (request, response) {
    response.writeHead(200, {'Content-Type': 'text/html'});
    fs.readFile('/frontend/AllSensors.html', null, function (err, data) {
       if(err){
           response.write("Error occured during loading html file");
       }else{
           response.write(data);
       }
       response.end();
    });
});
	

app.get('/get_data', function (request, response) {

    console.log("Pozyskiwanie danych z chmury");

    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?keys=Hum,Temp,Light,Button,VOC" , 
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


app.get('/get_data_chart_hum', function (request, response) {
    console.log("Pozyskiwanie danych z chmury");
	var timestamp = Date.now();
	
	
	var ileOdjac = request.query.timeSpan;
	
	var tsWindow = timestamp - (3600*ileOdjac*1000);
	
	
    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?interval=1000&limit=3600&agg=NONE&useStrictDataTypes=false&keys=Hum&startTs="+tsWindow+"&endTs="+timestamp ,
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


app.get('/get_data_chart_temp', function (request, response) {

    console.log("Pozyskiwanie danych z chmury");
	var timestamp = Date.now();
	
	var ileOdjac = request.query.timeSpan;
	
	var tsWindow = timestamp - (3600*ileOdjac*1000);

    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?interval=1000&limit=3600&agg=NONE&useStrictDataTypes=false&keys=Temp&startTs="+tsWindow+"&endTs="+timestamp ,
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


app.get('/get_data_chart_light', function (request, response) {

    console.log("Pozyskiwanie danych z chmury");
	var timestamp = Date.now();
	
	
	var ileOdjac = request.query.timeSpan;
	
	var tsWindow = timestamp - (3600*ileOdjac*1000);
    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?interval=1000&limit=3600&agg=NONE&useStrictDataTypes=false&keys=Light&startTs="+tsWindow+"&endTs="+timestamp ,
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

app.get('/get_data_chart_pollution', function (request, response) {

    console.log("Pozyskiwanie danych z chmury");
	var timestamp = Date.now();
		
	var ileOdjac = request.query.timeSpan;
	
	var tsWindow = timestamp - (3600*ileOdjac*1000);
    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?interval=1000&limit=3600&agg=NONE&useStrictDataTypes=false&keys=VOC&startTs="+tsWindow+"&endTs="+timestamp ,
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


app.listen(PORT, function(){

    console.log("Serwer lokalny został uruchomiony na porcie 8080");
});