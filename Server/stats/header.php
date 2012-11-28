<!DOCTYPE HTML>
<html>
        <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
                <title>Estadístiques Catalanitzador per al Windows</title>
                <link rel="stylesheet" href="style.css" type="text/css" />
		<?php
			if(Utils::get_query_string()!='') {
				echo '<meta name="robots" content="noindex">';
			}
		?>
                <script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7.1/jquery.min.js"></script>
                <script type="text/javascript" src="/js/highcharts.js"></script>
                <script type="text/javascript" src="/js/modules/exporting.js"></script>
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
