<?php

class StatsOptions {
    
    private $_optionNames;
    private $_db;
    private $_catalanitzador;
    
    public function __construct($db, $catalanitzador)
    {
        $this->_optionNames = array (
            0 => "OptionSystemRestore", 
            1 => "OptionDialect", 
            2 => "OptionShowSecDlg",
            3 => "OptionShareTwitter", 
            4 => "OptionShareFacebook", 
            5 => "OptionShareGooglePlus", 
            6 => "OptionSilentInstall"
        );
        
        $this->_db = $db;
        $this->_catalanitzador = $catalanitzador;
    }
    
    public function GetOptions() {
        
        $v = $this->_catalanitzador->get_version_selected();
        $p = $this->_catalanitzador->get_platform_selected();  

        if(!empty($v) && !empty($p)) {
            $query = "select distinct OptionId from options, sessions, operatings ".
                        "where options.SessionID = sessions.Id AND operatings.ID = sessions.OperatingsID ".
                "AND ApplicationsID in ($v) and operatings.System = '$p'";
        } else if(!empty($v)) {
                $query = "select distinct OptionId from options, sessions ".
                        "where options.SessionId = sessions.Id AND ApplicationsID in ($v)";
            } else if(!empty($p)) {

        } else {
                $query = "select distinct OptionId from options";
        }
        
        $results = $this->_db->get_results($query);
        
        $options = array();
        
        if (empty($results)) { return $options; }

        foreach($results as $result) {
            
            if(isset($this->_optionNames[$result->OptionId])) {
                $result_name = $this->_optionNames[$result->OptionId];
            }
            else {
                $result_name = $result->OptionId;
            }
            $options[$result->OptionId] = $result_name;
        }
        
        return $options;
    }
    
    public function GetOptionValues($id) {
        $optionValues = array();
        
        if(!isset($id) || (empty($id) && !is_int($id))) { return $optionValues; }
        
        $v = $this->_catalanitzador->get_version_selected();
        $p = $this->_catalanitzador->get_platform_selected();  
        
        if(!empty($v)) {
            $v = " AND ApplicationsID in ($v)";
        } else {
            $v = '';
        }

        if(!empty($p)) {
            $p = " and operatings.System = '$p'";
        } else {
            $p = '';
        }
        
        $query = "select OptionId, Value, count(Value) as ".
                "Total from options, sessions, operatings where OptionId = $id  ".
                "AND options.SessionId = sessions.Id AND operatings.ID = sessions.OperatingsID".
        "$v $p group by OptionId,Value;";
        
        $results = $this->_db->get_results($query);
        
        if(empty($results)) { return $optionValues; }
        
        foreach($results as $result) {
            $optionValues[$result->Value] = $result->Total;
        }
        
        return $optionValues;
    }
}
?>
