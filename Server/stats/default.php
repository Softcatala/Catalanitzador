<?php
    global $Catalanitzador;
    global $actions;
    global $action_status;
    global $action_to_hide;
?>
                    <h2>Dades totals d'ús</h2>
					<ul>
						<li><strong>Sessions:</strong> 
							<?php echo get_total_sessions(); ?> 
							<span style="color:#666666">
							<?php echo "(",get_unique_sessions()," / ",get_unique_sessions_percent();
							      echo "% usuaris únics)"; ?>
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
				<?php if($Catalanitzador->is_version_selected()) { ?>
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
								echo '<td>',$total,'</td>';
								foreach($results as $i => $num) {
									if(in_array($i,$action_to_hide)) continue;
									echo '<td style="text-align:right">',
										($num==0)?0:number_format(round($num*100/$total,2),2),
										"% </td><td style=\"text-align:right\">($num)</td>";
								}
								echo '</tr>';
							}
						?>
							</tbody>
						</table>
						</li>
					</ul>
				</div>
				
