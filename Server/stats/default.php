					<h2>Dades totals d'ús</h2>
					<ul>
						<li><strong>Sessions:</strong> 
							<?php echo get_total_sessions(); ?>
							<span style="color:#666666">
							<?php echo " (",get_unique_sessions()," usuaris únics)"; ?>
							</span>
						<div id="stackedarea"></div>
						<script type="text/javascript">
	var stackedsessions;
	var colors = ['#4572A7','#AA4643','#89A54E','#80699B','#3D96AE','#DB843D','#92A8CD','#A47D7C','#B5CA92'];
	
	$(document).ready(function() {
		stackedsessions = new Highcharts.Chart({
			chart: {
				renderTo: 'stackedarea',
				type: 'area'
			},
			title: {
				text: 'Evolució històrica de sessions'
			},
			xAxis: {
				type: 'datetime',
				title: {
					enabled: "Data"
				}
			},
			yAxis: [{
				title: {
					text: 'Sessions acumulades'
				},
				labels: {
					formatter: function() {
						return this.value;
					},
					style : {
						color: colors[0]
					}
				}
			},{
				title: {
					text: 'Sessions',
                                        style: {
                                                color: colors[1]
                                        }
				},
				labels: {
					formatter: function() {
						return this.value;
					},
                                        style: {
                                                color: colors[1]
                                        }
				},
				opposite : true
			}],
			tooltip: {
				shared : true
			},
			plotOptions: {
				area: {
					marker: {
						enabled: false,
						symbol: 'circle',
						radius: 2,
						states: {
							hover: {
								enabled: true
							}
						}
					}
				}
			},
			series: [{
				type: 'area',
				name: 'Sessions acumulades',
				<?php if($vselected) { ?>
				visible: false,
				<?php } ?>
				data: [<?php
					$results = get_stacked_sessions();
					$noFirst = false;
					$total = 0;
					foreach($results as $result) {
						if($noFirst) echo ",";
						$dt = explode('-',$result->day);
						$noFirst = true;
						
						$noFirst = true;
						$total += $result->total;
						echo "[Date.UTC($dt[0],",($dt[1]-1),",$dt[2]),",$total,"]";
						
					}
				?>],
				yAxis: 0
			},{
				type: 'area',
				name: 'Sessions',
				data: [<?php
					$noFirst = false;
					foreach($results as $result) {
						if($noFirst) echo ",";
						$dt = explode('-',$result->day);
						$noFirst = true;
						
						echo "[Date.UTC($dt[0],",($dt[1]-1),",$dt[2]),",$result->total,"]";
					} 
				?>],
				yAxis:1			
			}]
		});
	});
						</script>
						
						</li>
						<li><strong>Accions:</strong> <span id="action_results_button">+</span>
						<table id="action_results">
								<thead>
									<tr><th></th>
						<?php
							echo '<th>TOTAL</th>';
							foreach($action_status as $status_id => $status_name) {
								if(in_array($status_id,$action_to_hide)) continue;
								echo '<th colspan="2">',$status_name,'</th>';
							}
						?>
									</tr>
								</thead>
								<tbody>
						<?php
							foreach($actions as $action_id => $action_name) {
								if($action_id == 0) continue;
								$res = get_actions_data($action_id);
								echo '<tr><td>',$action_name,'</td>';
								
								$results = array_fill(0,sizeof($action_status),0);
								$total = 0;
								
								if($res != NULL) {						
									foreach($res as $k => $result) {
										$results[$result->Result]=$result->total;
										$total += $result->total;
									}
								}
								echo '<!-- '; print_r($res); print_r($results); print_r($action_status); echo ' -->';
								echo '<td>',$total,'</td>';
								foreach($results as $i => $num) {
									if(in_array($i,$action_to_hide)) continue;
									echo '<td style="text-align:right">',
										($num==0)?0:number_format(round($num*100/$total,2),2),
										"% </td><td>($num)</td>";
								}
								echo '</tr>';
							}
						?>
							</tbody>
						</table>
						</li>
					</ul>
				</div>
				
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
