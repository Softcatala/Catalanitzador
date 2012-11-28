<?php

class StatsOptions {
    
    private $_optionNames;
    private $_db;
    private $_catalanitzador;
    
    public function __construct($db,$catalanitzador)
    {
        $this->_optionNames = array 
            ( 0 => "OptionSystemRestore", 1 => "OptionDialect", 2 => "OptionShowSecDlg");
        
        $this->_db = $db;
        $this->_catalanitzador = $catalanitzador;
    }
    
    
    public function GetOptions() {
        
        $v = $this->_catalanitzador->get_version_selected();
        
        if(!empty($v)) {
            $query = "select distinct OptionId from options, sessions ".
                    "where options.SessionId = sessions.Id AND ApplicationsID in ($v)";
        }
        else {
            $query = "select distinct OptionId from options";
        }
        
        $results = $this->_db->get_results($query);
        
        $options = array();
        
        if(empty($results)) return $options;
        
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
        
        if(!isset($id)) return $optionValues;
        
        $v = $this->_catalanitzador->get_version_selected();
        
        if(!empty($v)) {
            $v = " AND ApplicationsID in ($v)";
        }
        else {
            $v = '';
        }
        
        $results = $this->_db->get_results("select OptionId, Value, count(Value) as ".
                "Total from options, sessions where OptionId = $id  ".
                "AND options.SessionId = sessions.Id $v group by OptionId,Value;");
        
        if(empty($results)) return $optionValues;
        
        foreach($results as $result) {
            $optionValues[$result->Value] = $result->Total;
        }
        
        return $optionValues;
    }
}
?>
