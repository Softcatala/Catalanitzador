<?php
require( '/catalanitzador/lib/db.php' );

try {
	$total_sessions = $db->get_var('select count(sessions.ID) from sessions');
	echo $total_sessions;
} catch (Exception $e) {
	echo "S'està calculant...";
}
?>
