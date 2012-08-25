<?php
/** WEB **/
function get_query_string($key='',$val='') {
		$queryString = array();
		$queryString = $_GET;
		$queryString[$key] = $val;

		foreach($queryString as $key=>$val) {
			if(empty($queryString[$key]))
				unset($queryString[$key]);
		}

		$queryString = htmlspecialchars(http_build_query($queryString),ENT_QUOTES);

		if($queryString == '') {
			return;
		}

		return '?'.$queryString;
}

/**** SESSIONS ****/
function get_total_sessions() {
	global $db;

	$v = get_version_filter();

	if(!empty($v)) {
		$where = ' where ApplicationsID = '.$v;
	}
	
	return $db->get_var("select count(sessions.ID) from sessions $where");
}

function get_unique_sessions() {
	global $db;

	$v = get_version_filter();

	if(!empty($v)) {
		$where = ' where ApplicationsID = '.$v;
	} else {
		$where = ' where 1 = 1 ';
	}

	$noguid = $db->get_var("select count(distinct guid) from sessions $where and guid != ''");
	$guid = $db->get_var("select count(sessions.ID) from sessions $where and guid = ''");

	return $guid + $noguid;
}

function get_stacked_sessions() {
	
	global $db;
	
	$v = get_version_filter();

	if(!empty($v)) {
		$v = ' where ApplicationsID = '.$v;
	}
	
	$results = $db->get_results("select DATE_FORMAT(Date, '%Y-%m-%d') as day, count(id) as total from sessions $v group by day order by day asc;");
	
	return $results;
}

/**** OS ****/
function print_os_data($os_count) {
	$i = 0;
	$noFirst = false;
	foreach($os_count as $k => $subtotal) {
		if($noFirst) echo ",";
		$noFirst = true;
		echo '{';
		echo 'y: ',$subtotal['percent'],',';
		echo 'color: colors[',$i,'],';
		echo 'drilldown: {';
		echo 'name:',"'$k',";
		echo 'categories:[';
		$noSecond = false;
		foreach($subtotal as $j => $subsub) {
			if($j != 'count' && $j != 'percent') {
				if($noSecond) echo ",";
				echo "'$k-$j'";
				$noSecond = true;
			}
		}
		if($k == 'Altres') echo "'Altres'";
		echo '],';
		echo 'data:[';
		$noSecond = false;
		foreach($subtotal as $j => $subsub) {
			if($j != 'count' && $j != 'percent') {
				if($noSecond) echo ",";
				echo $subsub['percent'];
				$noSecond = true;
			}
		}
		if($k == 'Altres') echo $subtotal['percent'];
		echo '],color:colors[',$i,']';
		echo '} }';
		$i++;
	}
}

function get_os_stats() {
	global $db;
	global $vselected;
	
	$where = get_version_filter();
	
	if(!empty($where)) {
		$where = ' AND ApplicationsID = '.$where;
	}
	
	$results = $db->get_results("select count(sessions.ID) as total, operatings.Name, operatings.Bits, operatings.SPMajorVersion from sessions, operatings where operatings.ID = sessions.OperatingsID ". $where ." group by operatings.Name, operatings.Bits, operatings.SPMajorVersion;");

	$total_os = 0;
	$os_count = array();

	foreach($results as $k => $result) {
		$bits = $result->Bits.'bits';
		$sp = 'SP'.$result->SPMajorVersion;
		$total_v = $result->total;	
		$total_os += $total_v;
		if(!isset($os_count[$result->Name])) {
			$os_count[$result->Name] = array();
			$os_count[$result->Name]['count']=0;
			$os_count[$result->Name]['percent']=0;
		}
		if(!isset($os_count[$result->Name][$bits.'-'.$sp])) {
			$os_count[$result->Name][$bits.'-'.$sp]=array();
			$os_count[$result->Name][$bits.'-'.$sp]['count']=0;
			$os_count[$result->Name][$bits.'-'.$sp]['percent']=0;
		}
		
		$os_count[$result->Name]['count'] += $total_v;
		$os_count[$result->Name][$bits.'-'.$sp]['count'] += $total_v;
	}

	foreach($os_count as $k => $subtotal) {

		if($k == '') {
			$os_count['Altres']['percent'] = ($subtotal['count']/$total_os)*100;
			$os_count['Altres']['count'] = $subtotal['count'];
			unset($os_count[$k]);
		} elseif($k!='Altres') {
			$os_count[$k]['percent'] = ($subtotal['count']/$total_os)*100;
			foreach($subtotal as $j=>$subsub) {
				if($j != 'percent' && $j != 'count') {
					$os_count[$k][$j]['percent']=($os_count[$k][$j]['count']/$total_os)*100;
				}
			}	
		}
	}	
	
	return $os_count;
}

/**** OFFICE ****/
function get_office_stats() {
	return get_action_stats(2);
}

function print_office_data() {
	global $office_count;
	if($office_count) {
		print_action_data($office_count);
	} else {
		print_action_data(get_office_stats());
	}
}

/**** ACTIONS DATA ****/
function get_actions_data($action_id) {
	global $db;
	
	$v = get_version_filter();

	if(!empty($v)) {
		$v = ' AND ApplicationsID = '.$v;
		$results = $db->get_results("select ActionID, Result, count(*) as total from actions a inner join sessions s on s.ID = a.SessionID where ActionID = $action_id $v group by a.ActionID , Result order by Result asc");
	} else {
		$results = $db->get_results("select ActionID, Result, count(*) as total from actions a where ActionID = $action_id group by a.ActionID , Result order by Result asc");
	}
	
	return $results;
}

function print_char($action_id,$small,$nice) {
	
	$idAllStats = $small.'_versions';
	$idInstalledOnly = $idAllStats.'_installedonly';
	
	
	echo "jQuery('#$idAllStats').after('<div id=\"$idInstalledOnly\"",
	 ' style="height: 300px; margin: 0"></div>',"');";
	
	print_javascript_chart($idAllStats,$nice,$action_id,true);
	print_javascript_chart($idInstalledOnly,$nice,$action_id,false);
}

function print_javascript_chart($id,$nice,$action_id,$allStats) {
		?>
		<?=$id?>Chart = new Highcharts.Chart({
				chart: {
						renderTo: '<?=$id?>',
						plotBackgroundColor: null,
						plotBorderWidth: null,
						plotShadow: false
				},
				title: {
						text: "Versions de<?=$nice?>"
				},
				tooltip: {
						formatter: function() {
								return '<b>'+ this.point.name +'</b><br />'
								+ 'Total: ' + this.y + ' (' +
								(Math.round(this.percentage*100)/100.0) +'%)';
						}
				},
				plotOptions: {
						pie: {
								allowPointSelect: true,
								cursor: 'pointer',
								dataLabels: {
										enabled: true,
										color: '#000000',
										connectorColor: '#000000'
								},
								events : { 
									click: function(event) { 
										change_plots('<?=str_replace('_installedonly','',$id)?>');
										} 
								}
						}
				},
				series: [{ type: 'pie', name: "Versions de<?=$nice?>", data: [<?php 
					print_action_data(get_action_stats($action_id),$allStats);
				?>]}]
		});	
	<?php
}


/**** GENERIC FUNCTIONS ****/
function get_version_filter() {
	global $vselected;
	global $db;
	$where = '';
	if($vselected) {
		$v = $_GET['v'];
		if(!empty($v) && is_numeric($v)) {
			$version = $db->get_results("select ID from applications "
				. " where MajorVersion = $v[0] and MinorVersion = $v[1] and "
				. " Revision = $v[2] ");
			$where = $version[0]->ID;
		}
	}
	
	return $where;
}

function get_versions() {
	global $db;
	$results = $db->get_results("select ApplicationsID, count(*) as total from sessions group by ApplicationsID;");
	$r_versions = $db->get_results("select * from applications order by ID asc");
	$versions = array();
	$totals = array();
	foreach($r_versions as $k => $version) {
		$versions["ID-".$version->ID] = $version->MajorVersion.'.'.$version->MinorVersion.'.'.$version->Revision;
		$totals["ID-".$version->ID] = 0;
	}
	
	foreach($results as $k => $result) {
		$totals["ID-".$result->ApplicationsID] = $result->total;
	}
	
	asort($versions);
	
	return array($versions,$totals);
}


function get_action_stats($action_id) {
	global $db;
	global $subversions;
	
	$total_action = 0;
	$action_count = array();

	$v = get_version_filter();

	if(in_array($action_id,$subversions)) {
		//$tversion = "SUBSTRING(Version,1,LOCATE('.',Version,LOCATE('.',Version)+1)-1)";
		$tversion = "IF(LOCATE('.',Version,LOCATE('.',Version)+1)=0,Version,SUBSTRING(Version,1,LOCATE('.',Version,LOCATE('.',Version)+1)-1))";
	} else {
		$tversion = "Version";
	}

	// aquest IF no és necessari però és més eficient quan no hi ha filtre
	if(!empty($v)) {
		$v = ' AND ApplicationsID = '.$v;
		$results = $db->get_results("select count(*) total, $tversion AS NVersion from actions a inner join sessions s on s.ID = a.SessionID  where ActionID = $action_id $v group by NVersion;");
	} else {
		$results = $db->get_results("select count(*) total, $tversion AS NVersion from actions where ActionID = $action_id group by NVersion;");
	}
	
	foreach($results as $k => $result) {
		$action_count[$result->NVersion] = $result->total;
		$total_action += $result->total;
	}
	return $action_count;
}



function print_action_data($action_count,$notinstalled=true) {
	$i = 0;
	foreach ( $action_count as $n=>$action ) {
		if($i!=0) echo ','; 
		if($n=='') {
			if(!$notinstalled) continue;
			$n = 'Sense programa';
		}
		$i++;
		echo '["',$n,'", ',$action,']';
	}
}

/*********** inspectors *************/
function get_inspectors_data($id) {
	
	if(!is_int($id)) return array();
	global $db;
	$_data = array();
	$sql = "select count(SessionID) as Total, InspectorID, KeyVersion, Value from inspectors where InspectorId = $id group by InspectorID, KeyVersion, Value;";
	$results = $db->get_results($sql);
	
	$lastKey = '';
		
	foreach($results as $result) {
		
			$key = $result->KeyVersion;
		
			if($key != $lastKey) {
				$_data[$key] = array();
				$_data[$key]['total'] = 0;
				$_data[$key]['data'] = array();
			}
			
			$_data[$key]['total'] += $result->Total;
			$_data[$key]['data'][$result->Value] = $result->Total;
			
			$lastKey = $key;
	}
	return $_data;
}
?>
