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
                <script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.8.3/jquery.min.js"></script>
                <link rel="stylesheet" href="http://code.jquery.com/ui/1.9.2/themes/base/jquery-ui.css" />
                <script src="//ajax.googleapis.com/ajax/libs/jqueryui/1.9.2/jquery-ui.min.js"></script>
                <script type="text/javascript" src="/js/highcharts.js"></script>
                <script type="text/javascript" src="/js/modules/exporting.js"></script>
                <script type="text/javascript" src="/js/skinny.js"></script>
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
