<?php
/**
 * Description
 * This class handles the xml results and create the entries in the database
 * @author Pau Iranzo <paugnu@gmail.com>
 */
class result
{

    var $MajorVersion;
    var $MinorVersion;
    var $Revision;
    var $OSMajorVersion;
    var $OSMinorVersion;
    var $SPMajorVersion;
    var $SPMinorVersion;
    var $SuiteMask;
    var $Bits;
    var $LogFile;

    function __construct()
    {
    }

    function save_session()
    {
        global $db;

        $MajorVersion = $db->escape( $this->MajorVersion );
        $MinorVersion = $db->escape( $this->MinorVersion );
        $Revision = $db->escape( $this->Revision );
        $OSMajorVersion = $db->escape( $this->OSMajorVersion );
        $OSMinorVersion = $db->escape( $this->OSMinorVersion );
        $SPMajorVersion = $db->escape( $this->SPMajorVersion );
        $SPMinorVersion = $db->escape( $this->SPMinorVersion );
        $SuiteMask = $db->escape( $this->SuiteMask );
        $Bits = $db->escape( $this->Bits );
        $System = $db->escape( $this->System );
        $ProductType = $db->escape( $this->ProductType );
        $Name = $db->escape( $this->Name );
        $guid = $db->escape( $this->guid);
        $LogFile = $db->escape( $this->LogFile);

        $applications_query = $db->get_var ( "SELECT ID FROM applications WHERE  MajorVersion = '$MajorVersion' AND MinorVersion = '$MinorVersion' AND Revision = '$Revision'");
        $operatings_query = $db->get_var ( "SELECT ID FROM operatings WHERE OSMajorVersion = '$OSMajorVersion' AND OSMinorVersion = '$OSMinorVersion' AND SPMajorVersion = '$SPMajorVersion' AND  SPMinorVersion = '$SPMinorVersion' AND  SuiteMask = '$SuiteMask' AND System = '$System' AND ProductType ='$ProductType' AND Name ='$Name' AND Bits ='$Bits' ");
        $operatings_query_null = $db->get_var ( "SELECT ID FROM operatings WHERE OSMajorVersion = '$OSMajorVersion' AND OSMinorVersion = '$OSMinorVersion' AND SPMajorVersion = '$SPMajorVersion' AND  SPMinorVersion = '$SPMinorVersion' AND  SuiteMask = '$SuiteMask' AND System = '$System' AND ProductType ='$ProductType' AND Name ='' AND Bits ='$Bits' ");        

        if ( !$applications_query ){
            $db->query( "INSERT INTO applications
                    ( MajorVersion, MinorVersion, Revision )
                    VALUES
                    ( '$MajorVersion', '$MinorVersion', '$Revision' )
                    " );
             $ApplicationsID = $db->insert_id;
        }
        else
             $ApplicationsID = $applications_query;

        if ( !$operatings_query ){
            if( $id = $operatings_query_null ){
                $db->query( "UPDATE operatings SET Name = '$Name' WHERE id = '$id'" );
                $OperatingsID = $id;
            }
            else{
                $db->query( "INSERT INTO operatings
                        ( OSMajorVersion, OSMinorVersion, SPMajorVersion, SPMinorVersion, SuiteMask, System, ProductType, Name, Bits )
                        VALUES
                        ( '$OSMajorVersion', '$OSMinorVersion', '$SPMajorVersion', '$SPMinorVersion', '$SuiteMask', '$System', '$ProductType', '$Name', '$Bits' )
                        " );
                 $OperatingsID = $db->insert_id;
             }
        }
        else
            $OperatingsID = $operatings_query;

         $db->query( "INSERT INTO sessions
                ( Date, ApplicationsID, OperatingsID, LogFile, guid )
                VALUES
                ( NOW(), '$ApplicationsID', '$OperatingsID', '$LogFile', '$guid' )
                " );
         $SessionID = $db->insert_id;

         return $SessionID;
    }

    //This function adds a new action to a particular session after the session has been created
    function add_action($SessionID, $ActionID, $Version, $Result)
    {
        global $db;

        $db->query( "INSERT INTO actions
                ( SessionID, ActionID, Version, Result )
                VALUES
                ( '$SessionID', '$ActionID', '$Version', '$Result' )
                " );
    }

    //This function adds a new inspector to a particular session after the session has been created
    function add_inspector($SessionID, $InspectorID, $KeyVersion, $Value)
    {
        global $db;

        $db->query( "INSERT INTO inspectors
                ( SessionID, InspectorID, KeyVersion, Value )
                VALUES
                ( '$SessionID', '$InspectorID', '$KeyVersion', '$Value' )
                " );
    }

    //This function adds a new option to a particular session after the session has been created
    function add_option($SessionID, $OptionID, $Value)
    {
        global $db;

        $db->query( "INSERT INTO options
                ( SessionID, OptionID, Value )
                VALUES
                ( '$SessionID', '$OptionID', '$Value' )
                " );
    }
}

?>
