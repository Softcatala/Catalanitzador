<?php
/* We use a library that let us create queries in a really easy way */
include_once( 'ezdb1.php');

/* Connection data 
$globals = array();
$globals['db_user']			= 'cat';
$globals['db_password']			= '123incata908';
$globals['db_name']			= 'catalanitzador';
$globals['db_server']			= 'localhost';
$globals['mysql_persistent']		= true;*/

$globals = array();
$globals['db_user']			= 'softvalencia';
$globals['db_password']			= 'carxofeta&123';
$globals['db_name']			= 'catalanitzador_dev';
$globals['db_server']			= 'localhost';
$globals['mysql_persistent']		= true;

/* Now we connect to the database */
$db = new db($globals['db_user'], $globals['db_password'], $globals['db_name'], $globals['db_server']);

$db->persistent = $globals['mysql_persistent'];
$db->query("SET NAMES 'utf8'");

?>
