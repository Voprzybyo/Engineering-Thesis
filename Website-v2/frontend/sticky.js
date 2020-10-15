	function sticky(){
	
	$(document).ready(function() {
	var NavY = $('.allSensorsRightSide2').offset().top;
	 
	var stickyNav = function(){
	var ScrollY = $(window).scrollTop();
		  
	if (ScrollY > NavY) { 
		$('.allSensorsRightSide2').addClass('sticky');
	} else {
		$('.allSensorsRightSide2').removeClass('sticky'); 
	}
	};
	 
	stickyNav();
	 
	$(window).scroll(function() {
		stickyNav();
	});
	});	

	}