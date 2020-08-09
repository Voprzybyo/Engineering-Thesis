	const express = require('express');
	const app = express();
	const fetch = require('node-fetch');
	var fs = require('fs');
	
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
            "X-Authorization": "Bearer eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJ3by5wcnp5YnlvQGdtYWlsLmNvbSIsInNjb3BlcyI6WyJURU5BTlRfQURNSU4iXSwidXNlcklkIjoiN2RjNmQ0OTAtZDFjMC0xMWVhLThjMzYtODdhODUyZTM5NTJjIiwiZmlyc3ROYW1lIjoiV29qY2llY2giLCJsYXN0TmFtZSI6IlByenliecWCbyIsImVuYWJsZWQiOnRydWUsInByaXZhY3lQb2xpY3lBY2NlcHRlZCI6dHJ1ZSwiaXNQdWJsaWMiOmZhbHNlLCJ0ZW5hbnRJZCI6IjdjMDgwZDkwLWQxYzAtMTFlYS04YzM2LTg3YTg1MmUzOTUyYyIsImN1c3RvbWVySWQiOiIxMzgxNDAwMC0xZGQyLTExYjItODA4MC04MDgwODA4MDgwODAiLCJpc3MiOiJ0aGluZ3Nib2FyZC5pbyIsImlhdCI6MTU5Njk2OTQ0MywiZXhwIjoxNTk4NzY5NDQzfQ.P7tZrkFXNf50BCEkBC674J36TZCUh_LRGxOtHd2oIjliy7K9BKoMj6_wSxAQ3bd4scF4ByZYgPcTQZl7_zn2eQ"
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


app.get('/get_data_chart', function (request, response) {

    console.log("Pozyskiwanie danych z chmury");

    fetch("https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/4d320510-d892-11ea-a1c4-b9c8a6e6378a/values/timeseries?limit=100&agg=NONE&useStrictDataTypes=false&keys=Hum&startTs=1596979174956&endTs=1596980502084" ,
	{
        headers: 
		{
            "Accept": "application/json",
            "X-Authorization": "Bearer eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJ3by5wcnp5YnlvQGdtYWlsLmNvbSIsInNjb3BlcyI6WyJURU5BTlRfQURNSU4iXSwidXNlcklkIjoiN2RjNmQ0OTAtZDFjMC0xMWVhLThjMzYtODdhODUyZTM5NTJjIiwiZmlyc3ROYW1lIjoiV29qY2llY2giLCJsYXN0TmFtZSI6IlByenliecWCbyIsImVuYWJsZWQiOnRydWUsInByaXZhY3lQb2xpY3lBY2NlcHRlZCI6dHJ1ZSwiaXNQdWJsaWMiOmZhbHNlLCJ0ZW5hbnRJZCI6IjdjMDgwZDkwLWQxYzAtMTFlYS04YzM2LTg3YTg1MmUzOTUyYyIsImN1c3RvbWVySWQiOiIxMzgxNDAwMC0xZGQyLTExYjItODA4MC04MDgwODA4MDgwODAiLCJpc3MiOiJ0aGluZ3Nib2FyZC5pbyIsImlhdCI6MTU5Njk2OTQ0MywiZXhwIjoxNTk4NzY5NDQzfQ.P7tZrkFXNf50BCEkBC674J36TZCUh_LRGxOtHd2oIjliy7K9BKoMj6_wSxAQ3bd4scF4ByZYgPcTQZl7_zn2eQ"
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

app.listen(8080, function(){

    /*DZIALA TEGO KURWA NIE RUSZAJ */
    /*
    var json_out;
    var response = fetch("https://agh-noise-measurement-network.data.thethingsnetwork.org/api/v2/query?last=130h", {
        headers: {
            Accept: "application/json",
            Authorization: "key ttn-account-v2.4YL9PaQB7xMWYq3cKqkeTGL0XXi6FiJ8GiR5lxYT60I"
        }
    })
        .then(res => res.json())
        .then(data =>console.log(data[0]));
     */

    console.log("Serwer lokalny został uruchomiony na porcie 8080");
});