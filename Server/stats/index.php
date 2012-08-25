<?php

require( '../lib/db_stats.php' );
require( 'functions.php' );

global $db;

//Actions
$actions = array( 
	0 => 'NoAction', 1 => 'WindowsLPI', 2 => 'MSOfficeLPI', 3 => 'ConfigureLocale', 
	4 =>'IEAcceptLanguage', 5 => 'IELPI', 6 => 'ConfigureDefaultLanguage', 
	7 => 'Chrome', 8=>'Firefox', 9 => 'OpenOffice', 10=>'AcrobatReader',
	11 => 'Windows Live', 12 => 'CatalanitzadorUpdate', 13 => 'Skype'
	);

$subversions = array();
$subversions[] = 4; $subversions[] = 7; $subversions[] = 8;
                     
// ActionStatus
$action_status = array( 
	0 => 'NotSelected',		// The user did not select the action
	1 => 'Selected',			// Selected but no started
	2 => 'CannotBeApplied',	// The action cannot be applied (e.g. Windows ES needed US found or application not found)
	3 => 'AlreadyApplied',		// Action not needed
	4 => 'InProgress',			// Selected and in progress
	5 => 'Successful',			// End up OK
	6 => 'FinishedWithError',	// End up with error
	7 => 'NotInstalled'		// Software is not installed
);
$action_to_hide = array ( 1, 4);                            
// OS
$os_names = array( "6.1" => "Windows 7", "6.0" => "Windows Vista", "5.2" => "Windows XP x64",
	"5.1" => "Windows XP", "5.0" => "Windows 2000");

$inspectors = array ( 1 => "LibreOffice", 2 => "Skype", 3 => "PDFCreator", 4=> "WinRAR");

?><!DOCTYPE HTML>
<html>
        <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
                <title>Estadístiques Catalanitzador per al Windows</title>


                <!-- 1. Add these JavaScript inclusions in the head of your page -->
                <script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7.1/jquery.min.js"></script>
                <script type="text/javascript" src="/js/highcharts.js"></script>


                <!-- 1a) Optional: add a theme file -->
                <!--
                        <script type="text/javascript" src="../js/themes/gray.js"></script>
                -->

                <!-- 1b) Optional: the exporting module -->
                <script type="text/javascript" src="/js/modules/exporting.js"></script>

                <!-- 2. Add the JavaScript to initialize the chart on document ready -->
                
				<style type="text/css">
					table {
						font-family: verdana,arial,sans-serif;
						font-size:11px;
						color:#333333;
						border-width: 1px;
						border-color: #666666;
						border-collapse: collapse;
						margin-left: auto;
						margin-right: auto;
					}
					table th {
						border-width: 1px;
						padding: 8px;
						border-style: solid;
						border-color: #666666;
						background-color: #dedede;
					}
					table td {
						border-width: 1px;
						padding: 8px;
						border-style: solid;
						border-color: #666666;
						background-color: #ffffff;
					}
					#action_results_button {
						font-size: 1.5em;
						font-weigth:bold;
						cursor: pointer;
					}
				</style>
				<script>
				jQuery(document).ready(function(){
					jQuery('div').filter(function() {
						return this.id.match(/installedonly/);
					}).hide();
				});
				
				function change_plots(str) {
					jQuery('#'+str).toggle();
					jQuery('#'+str+'_installedonly').toggle();
				}
				</script>
        </head>
        <body>
				<h1>Estadístiques del <a href="http://catalanitzador.softcatala.org" 
					title="Catalanitzador per al Windows">Catalanitzador</a> de 
					<a href="http://www.softcatala.org">Softcatalà</a></h1>
				<div id="totals" style="witdh:800px;margin:0 auto">
					<div>
					<h2>Versions d'ús del catalanitzador</h2>
					<table id="application_version">
						<thead>
							<tr>
					<?php
						$version_data = get_versions();
						$vselected = false;
						foreach($version_data[0] as $ID => $version) {
							$v = str_replace('.','',$version);
							if($_GET['v'] == $v) {
								$style = ' style="background-color:#CCFFCC" ';
								$vselected = true;
							} else {
								$style = '';
							}
							echo '<th ',$style,'><a href="?v=',$v,'">',$version,'</a></th>';
						}
						
						if($vselected) {
							echo '<th>';
						} else {
							echo '<th style="background-color:#CCFFCC">';
						}
						
						echo '<a href="?">TOTES</a></th></tr></thead><tbody><tr>';
						$total_v = 0;
						foreach($version_data[0] as $ID => $version) {
							echo '<td>';
							if(isset($version_data[1][$ID])) {
								echo $version_data[1][$ID];
								$total_v += $version_data[1][$ID];
							} else {
								echo 0;
							}
							echo '</td>';
						}
						echo '<td>',$total_v,'</td>';
						
					?>
							</tr>
						</tbody>
					</table>
					</div>
					<h2>Dades totals d'ús</h2>
					Última actualització de les dades: <em><?=date('H:j:s d/m/Y')?></em>
					<?php
						$total_sessions = $db->get_var("select count(sessions.ID) from sessions;");
					?>
					<ul>
						<li><strong>Sessions:</strong> <?=$total_sessions?>
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
	<h2>Inspectors</h2>
	<em>
		Els inspectors són mòduls del Catalanitzador que comproven si hi ha un 
		programa instal·lat i la versió del mateix, per tal de servir com a guia
		prioritzada per al desenvolupament del Catalanitzador
	</em>
	<?php
		
		foreach($inspectors as $id => $inspector) {
			$inspector_data = get_inspectors_data($id);
			
			echo "<h3>$inspector</h3>";
			$keys = sizeof($inspector_data);
			echo "<table><thead><tr>";
			
			foreach($inspector_data as $key=>$keyValues) {
				echo "<th>$key</th>";
			}
			echo "</tr></thead><tbody><tr>";
			foreach($inspector_data as $key=>$keyValues) {
				echo "<td style='vertical-align:top;';>";
				echo "<table><tbody>";
				$total_occurrences = $keyValues['total'];
				foreach($keyValues['data'] as $keyValue=>$keyCount) {
					if(empty($keyValue)) $keyValue = '-';
					
					echo "<tr><td style='border:0;text-align:left;font-weight:strong;width:40%'>",
						$keyValue,"</td>","<td style='border:0;text-align:right;width:30%'>",
						$keyCount,"</td><td style='border:0;text-align:right;width:30%'>",
						number_format(round($keyCount*100/$total_occurrences,2),2),"%</td></tr>";
				}
				echo "</tbody></table>";
				
				echo "</td>";
			}
			echo "</tr></tbody></table>";
		}
	?>
</html>
