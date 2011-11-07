<?php
/**
 * Catalanitzador parser that handles an XML file that contain log stats when a user finishes the «catalanització» of his computer
 *
 */
require( 'lib/db.php' );
require( 'lib/class/result.php' );

$result = new Result();

$xmlstring = utf8_decode($_POST['xml']);
$xmlstring = str_replace ( '\\', '', $xmlstring);

/* -- XML string example
$xmlstring =
<?xml version='1.0'?>
<execution>
<application MajorVersion='0' MinorVersion='1' Revision='2' />
<operating OSMajorVersion='6' OSMinorVersion='0' SPMajorVersion='2' SPMinorVersion='0' SuiteMask='768' Bits='32'/>

<actions>
<action id='1' version='' result='0'/>
<action id='3' version='' result='0'/>
<action id='4' version='' result='4'/>
<action id='2' version='' result='0'/>
</actions>
</execution>
;
-- */

//Transform the xml string into a simplexml object
$xml = simplexml_load_string($xmlstring, 'SimpleXMLElement', LIBXML_NOCDATA | LIBXML_NOBLANKS);

//If the xml is a file
//$xml = simplexml_load_file("xml/model.xml");

if (!empty($xml))
{              
        //Application
        $result->MajorVersion = $xml->application['MajorVersion'][0];
        $result->MinorVersion = $xml->application['MinorVersion'][0];
        $result->Revision = $xml->application['Revision'][0];

        //Operating
        $result->OSMajorVersion = $xml->operating['OSMajorVersion'][0];
        $result->OSMinorVersion = $xml->operating['OSMinorVersion'][0];
        $result->SPMajorVersion = $xml->operating['SPMajorVersion'][0];
        $result->SPMinorVersion = $xml->operating['OSMajorVersion'][0];
        $result->SuiteMask = $xml->operating['SuiteMask'][0];
        $result->Bits = $xml->operating['Bits'][0];
        
        //Get the new SessionID in order to save actions
        $SessionID =  $result->save_session();

        if ( $xml->actions->action )
        {
                echo '<div style="margin: 0 auto; width: 400px; padding: 1px 20px 20px; background: #CCC; font-family: Arial; font-size: 13px;"><h2>Info</h2>';
                foreach ( $xml->actions->action as $action )
                {
                        $result->add_action($SessionID, $action['id'], $action['version'], $action['result']);
                        echo 'S\'ha afegit l\'acció: '.$action['id'].' de la sessió '.$SessionID.'<br/>';
                }
                echo '</div>';
        }

        return true;
}
else
{
        echo '<div style="margin: 0 auto; width: 400px; padding: 1px 20px 20px; background: #CCC; font-family: Arial; font-size: 13px;">';
        echo "<h2>Error</h2>No s'ha trobat cap xml o bé l'xml no està ben formatat. <br/><br/>Recordeu enviar-lo per la variable _POST['xml']";
        echo "</div>";
        return false;
}


?>
