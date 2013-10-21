<?php

$inspectors = array ( 1 => "LibreOffice", 2 => "Skype", 3 => "PDFCreator", 4=> "WinRAR", 5=>"iTunes", 6 => "Thunderbird", 7 => "CCleaner");

?>
	<h2>Inspectors</h2>
	<em>
		Els inspectors són mòduls del Catalanitzador que comproven si hi ha un 
		programa instal·lat i la versió del mateix, per tal de servir com a guia
		prioritzada per al desenvolupament del Catalanitzador
	</em>
	<?php
		
		foreach($inspectors as $id => $inspector) {
			$inspector_data = get_inspectors_data($id);
			
			$keys = sizeof($inspector_data);
			if($keys == 0) continue;
			echo "<h3>$inspector</h3>";
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
