	<h2>Versions dels sistemes operatius</h2>
	<div id="os_versions" style="height: 600px; margin: 0 auto"></div>
	
	<script type="text/javascript">

	$(document).ready(function() {
			$('#action_results_button').click(function(){
				if($(this).html() == '+') {
					$(this).html('-');
					$('#action_results').fadeIn('slow');
				} else {
					$(this).html('+');
					$('#action_results').fadeOut('slow');
				}
			});
			
			var chartOS;
			$(document).ready(function() {
				var colors = Highcharts.getOptions().colors,
					categories = [<?php
						$os_count = get_os_stats();
						$noFirst = false;
						foreach($os_count as $k => $subtotal) {
							if($noFirst) echo ",";
							echo "'$k'";
							$noFirst = true;
						}
					?>],
					name = 'Sistemes operatius',
					data = [ <?php print_os_data($os_count); ?>	];


				// Build the data arrays
				var browserData = [];
				var versionsData = [];
				for (var i = 0; i < data.length; i++) {

					// add browser data
					browserData.push({
						name: categories[i],
						y: data[i].y,
						color: data[i].color
					});

					// add version data
					for (var j = 0; j < data[i].drilldown.data.length; j++) {
						var brightness = 0.2 - (j / data[i].drilldown.data.length) / 5 ;
						versionsData.push({
							name: data[i].drilldown.categories[j],
							y: data[i].drilldown.data[j],
							color: Highcharts.Color(data[i].color).brighten(brightness).get()
						});
					}
				}

				// Create the chart
				chartOS = new Highcharts.Chart({
					chart: {
						renderTo: 'os_versions',
						type: 'pie'
					},
					title: {
						text: 'Distribució dels sistemes operatius'
					},
					yAxis: {
						title: {
							text: 'Total percent market share'
						}
					},
					plotOptions: {
						pie: {
							shadow: false
						}
					},
					tooltip: {
						formatter: function() {
							return '<b>'+ this.point.name +'</b>: '+ this.y.toFixed(2) +' %';
						}
					},
					series: [{
						name: 'Browsers',
						data: browserData,
							size: '60%',
						dataLabels: {
							formatter: function() {
								return this.y > 5 ? this.point.name : null;
							},
							color: 'white',
							distance: -30
						}
					}, {
						name: 'Versions',
						data: versionsData,
						innerSize: '60%',
						dataLabels: {
							formatter: function() {
								// display only if larger than 1
								return this.y > 1 ? '<b>'+ this.point.name +':</b> '+ this.y.toFixed(2) +'%'  : null;
							}
						}
					}]
				});
			});
		});	
	</script>
	<h2>Versions del Microsoft Office</h2>
	<div id="office_versions" style="height: 300px; margin: 0 auto"></div>
	<script type="text/javascript">
		<?php print_char(2,'office'," l'Office"); ?>
	</script>
	<h2>Versions de l'Internet Explorer</h2>
	<div id="ie_versions" style="height: 300px; margin: 0 auto"></div>
	<script type="text/javascript">
		<?php print_char(4,'ie'," l'Internet Explorer"); ?>
	</script>
	<h2>Versions del Google Chrome</h2>
	<div id="chrome_versions" style="height: 300px; margin: 0 auto"></div>
	<script type="text/javascript">
		<?php print_char(7,'chrome',"l Chrome"); ?>
	</script>
	<h2>Versions del Mozilla Firefox</h2>
	<div id="firefox_versions" style="height: 300px; margin: 0 auto"></div>
	<script type="text/javascript">
		<?php print_char(8,'firefox',"l Firefox"); ?>
	</script>
	<h2>Versions de l'OpenOffice</h2>
	<div id="ooo_versions" style="height: 300px; margin: 0 auto"></div>
	<script type="text/javascript">
		<?php print_char(9,'ooo'," l'OpenOffice"); ?>
	</script>
	<h2>Versions de l'Adobe Reader</h2>
	<div id="adobereader_versions" style="height: 300px; margin: 0 auto"></div>
	<script type="text/javascript">
		<?php print_char(10,'adobereader'," l'Adobe Reader"); ?>
	</script>
	<h2>Versions del Windows Live Essential</h2>
	<div id="windowslive_versions" style="height: 300px; margin: 0 auto"></div>
	<script type="text/javascript">
		<?php print_char(11,'windowslive',"l Windows Live"); ?>
	</script>
        <h2>Versions del Catalanitzador</h2>
	<div id="catalanitzador_versions" style="height: 300px; margin: 0 auto"></div>
	<script type="text/javascript">
		<?php print_char(12,'catalanitzador',"l Catalanitzador"); ?>
	</script>
