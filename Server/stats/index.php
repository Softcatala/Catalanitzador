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


?><!DOCTYPE HTML>
<html>
        <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
                <title>Estadístiques Catalanitzador per al Windows</title>
		<link rel="stylesheet" href="style.css">

		<?php
			if(get_query_string()!='') {
				echo '<meta name="robots" content="noindex">';
			}
		?>


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
		<div class="last_update">
			Última actualització de les dades: <em><?=date('H:j:s d/m/Y')?></em>
		</div>
		<div id="menu">
			<a href="<?=get_query_string('show','')?>" class="<?=css_is_active('')?>">Resum</a><a href="<?=get_query_string('show','versions')?>" class="<?=css_is_active('versions')?>">Versions</a><a href="<?=get_query_string('show','inspectors')?>" class="<?=css_is_active('inspectors')?>">Inspectors</a><a href="<?=get_query_string('show','options')?>" class="<?=css_is_active('options')?>">Opcions</a>
		</div>
		<div id="totals" style="witdh:800px;margin:0 auto"> <div>
			<h2>Versions del Catalanitzador</h2>
			<table id="application_version">
				<thead>
					<tr>
			<?php
				$version_data = get_versions();
				$vselected = false;
				foreach($version_data[0] as $ID => $version) {
					$v = str_replace('.','',$version);
					if($_GET['v'] == $v) {
						$style = ' class="active" ';
						$vselected = true;
					} else {
						$style = '';
					}
					echo '<th ',$style,'><a href="';
					echo get_query_string('v',$v);
					echo '">',$version,'</a></th>';
				}
					
				if($vselected) {
					echo '<th>';
				} else {
					echo '<th style="background-color:#CCFFCC">';
				}
						
				echo '<a href="',get_query_string('v',''),'">TOTES</a></th></tr></thead><tbody><tr>';
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
		<br />
<?php
	$show = 'default';

	if(isset($_GET['show'])) {
		switch($_GET['show']) {
			case 'inspectors':
				$show = 'inspectors';
				break;
			case 'versions':
				$show = 'versions';
				break;
                        case 'options':
                                $show = 'options';
                                break;
		}
	}
	
	include($show.'.php');
?>
</html>
