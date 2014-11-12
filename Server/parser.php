<?php
/**
 * Catalanitzador parser that handles an XML file that contain log stats when a user finishes the «catalanització» of his computer
 *
 */
require( 'lib/db.php' );
require( 'lib/class/result.php' );

if ( isset ($_GET['debug']))
    error_reporting(E_ALL);

$result = new Result();

if (isset($_POST['xml'])){
    $xmlstring = utf8_decode($_POST['xml']);

    //Transform the xml string into a simplexml object
    $xml = simplexml_load_string($xmlstring, 'SimpleXMLElement', LIBXML_NOCDATA | LIBXML_NOBLANKS);

    if (!empty($xml)){
        //Application
        $result->MajorVersion = $xml->application['MajorVersion'][0];
        $result->MinorVersion = $xml->application['MinorVersion'][0];
        $result->Revision = $xml->application['Revision'][0];

        //Operating
        $result->OSMajorVersion = $xml->operating['OSMajorVersion'][0];
        $result->OSMinorVersion = $xml->operating['OSMinorVersion'][0];
        $result->SPMajorVersion = $xml->operating['SPMajorVersion'][0];
        $result->SPMinorVersion = $xml->operating['SPMinorVersion'][0];
        $result->SuiteMask = $xml->operating['SuiteMask'][0];
        $result->Bits = $xml->operating['Bits'][0];
        $result->System = $xml->operating['System'][0];
        $result->ProductType = $xml->operating['ProductType'][0];
        $result->Name = $xml->operating['Name'][0];
        $result->guid = $xml->session['guid'][0];

        //Log
        if (isset($_POST['log'])){
            $log = utf8_encode($_POST['log']);  
            file_put_contents("saved.txt", $log); 
            $result->LogFile = $log;
        }

        //Get the new SessionID in order to save actions
        $SessionID =  $result->save_session();

        if ($xml->actions->action){
            foreach ( $xml->actions->action as $action ){
                $result->add_action($SessionID, $action['id'], $action['version'], $action['result']);
            }
        }

        //Actions
        if ($xml->inspectors->inspector){
            foreach ( $xml->inspectors->inspector as $inspector ){
                $result->add_inspector($SessionID, $inspector['id'], $inspector['key'], $inspector['value']);
            }
        }

        //Options
        if ($xml->options->option){
            foreach ( $xml->options->option as $option ){
                $result->add_option($SessionID, $option['id'], $option['value']);
            }
        }

        return true;
    }
    else
        return false;
}
else{
    if (isset ($_GET['debug'])){
        echo 'No s\'ha rebut cap variable. A continuació es fa un dump de la variable $_POST:';
        echo '<pre>';var_dump($_POST);echo '</pre>';
    }
    else
        return false;
}

?>
