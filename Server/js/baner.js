$(document).ready(function(){

	var pathname = window.location.pathname;

	if (pathname == "/baner.php") {
			$(".enllaceu").addClass("enllaceu-actiu");
	}
	
	$("[name=mida]").click(function() {
		var tick = "#tick-"+$(this).val();
		$(".fake-action div").removeClass("fake-on");
		$(tick).addClass("fake-on");
		var url = "http://catalanitzador.softcatala.org/?baner="+$(this).val();
		var img = "http://catalanitzador.softcatala.org/images/baner/Catalanitzador_baner"+$(this).val()+"_winOSX.gif";

		var addimg = '<a href="'+url+'"><img alt="Catalanitzador de Softcatalà" src="'+img+'" /></a>';
		var addcode = '&lt;a href="'+url+'"&gt;&lt;img alt="Catalanitzador de Softcatalà" src="'+img+'" /&gt;&lt;/a&gt;';

		$(".outcome").children().remove();
		$(".outcome").append(addimg);
		
		$(".step-script code").empty();
		$(".step-script code").append(addcode);

	});


});
