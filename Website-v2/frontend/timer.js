	function counting(){
	
	var today = new Date();
	
	var day = today.getDate();
		if(day<10){
	day = "0"+day;
	}
	var month = today.getMonth()+1;
		if(month<10){
	month = "0"+month;
	}
	var year = today.getFullYear();
		if(year<10){
	year = "0"+year;
	}
	var hour = today.getHours();
	if(hour<10){
	hour = "0"+hour;
	}
	var minute = today.getMinutes();
	if(minute<10){
	minute = "0"+minute;
	}
	var second = today.getSeconds();
	if(second<10){
	second = "0"+second;
	}
	
	document.getElementById("clock").innerHTML = 
	day+"/"+month+"/"+year+"</br>"+hour+":"+minute+":"+second; 
	
	setTimeout("counting()",1000);
	}