$(document).ready(function(){
    $('#slider').bxSlider({
                    mode: 'fade',
                    auto: true,
                    controls: true
                    });
        
    $('#llegiu_aixo').click(function() {
		  $('#internet8_advice').fadeIn();
		});

    $('#close_ie8advice').click(function() {
		  $('#internet8_advice').hide();
		});

  });
