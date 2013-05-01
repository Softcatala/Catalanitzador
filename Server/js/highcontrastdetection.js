jQuery.fn.highContrast = function(options) 
{
	var defaults = {
		divId: 'jQueryHighContrastDetection',
		image: '../images/conegueu.png',
		styleSheet: 'css/highcontrast.css'
	};

	//update from users options
	var options = $.extend(defaults, options);

	//build the quick detection div
	var hcDetect = jQuery('<div id="' + options.divId + '"></div>').css({
		background: 'url("' + options.image + '")', width: '0px', height: '0px' });
	hcDetect.appendTo(document.body);

	if(hcDetect.css('background-image') == 'none')
	{
		$('head').append('<link rel="stylesheet" href="' + options.styleSheet + '"type = "text/css" />');
	}

	//hide
	$('#' + options.divId.remove()); //remove the item
};

$(document).ready( function() {
	//determine if the site is runing in high contrast mode
	$(document).highContrast();

});
