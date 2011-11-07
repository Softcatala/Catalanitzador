<?php
/**
 * Description
 * This class handles the xml results and create the entries in the database
 * @author Pau Iranzo <paugnu@gmail.com>
 */
class result {

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

    function __construct( )
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
        
        //No log file at this moment
        $LogFile = '';
        
        if ( !$db->get_var ( "SELECT ID FROM applications WHERE  MajorVersion = '$MajorVersion' AND MinorVersion = '$MinorVersion' AND Revision = '$Revision'"))
        {
            $db->query( "INSERT INTO applications 
                    ( MajorVersion, MinorVersion, Revision ) 
                    VALUES 
                    ( '$MajorVersion', '$MinorVersion', '$Revision' )
                    " );
             $ApplicationsID = $db->insert_id;
        }
        else
             $ApplicationsID = $db->get_var ( "SELECT ID FROM applications WHERE  MajorVersion = '$MajorVersion' AND MinorVersion = '$MinorVersion' AND Revision = '$Revision'");
        
        if ( !$db->get_var ( "SELECT ID FROM operatings WHERE OSMajorVersion = '$OSMajorVersion' AND OSMinorVersion = '$OSMinorVersion' AND SPMajorVersion = '$SPMajorVersion' AND  SPMinorVersion = '$SPMinorVersion' AND  SuiteMask = '$SuiteMask' AND Bits ='$Bits' "))
        {
            $db->query( "INSERT INTO operatings 
                    ( OSMajorVersion, OSMinorVersion, SPMajorVersion, SPMinorVersion, SuiteMask, Bits ) 
                    VALUES 
                    ( '$OSMajorVersion', '$OSMinorVersion', '$SPMajorVersion', '$SPMinorVersion', '$SuiteMask', '$Bits' )
                    " );
             $OperatingsID = $db->insert_id;
        }
        else
            $OperatingsID = $db->get_var ( "SELECT ID FROM operatings WHERE OSMajorVersion = '$OSMajorVersion' AND OSMinorVersion = '$OSMinorVersion' AND SPMajorVersion = '$SPMajorVersion' AND  SPMinorVersion = '$SPMinorVersion' AND  SuiteMask = '$SuiteMask' AND Bits ='$Bits' ");
         
         $db->query( "INSERT INTO sessions 
                ( Date, ApplicationsID, OperatingsID, LogFile ) 
                VALUES 
                ( NOW(), '$ApplicationsID', '$OperatingsID', '$LogFile' )
                " );
         $SessionID = $db->insert_id;
         
         return $SessionID;
         
    }
    
    //This function adds a new action to a particular session after the session has been created
    function add_action ( $SessionID, $ActionID, $Version, $Result)
    {        
        global $db;
        
        $db->query( "INSERT INTO actions 
                ( SessionID, ActionID, Version, Result ) 
                VALUES 
                ( '$SessionID', '$ActionID', '$Version', '$Result' )
                " );
    }
}

?>
