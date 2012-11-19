<?php

class StatsOptions {
    
    private $_optionNames;
    private $db;
    
    public function __construct($db)
    {
        $this->_optionNames = array 
            ( 0 => "OptionSystemRestore", 1 => "OptionDialect", 2 => "OptionShowSecDlg");
        
        $this->db = $db;
    }
    
    
    public function GetOptions() {
        
        $v = get_version_filter();
        
        if(!empty($v)) {
            $query = "select distinct OptionId from options, sessions ".
                    "where options.SessionId = sessions.Id AND ApplicationsID in ($v)";
        }
        else {
            $query = "select distinct OptionId from options";
        }
        
        $results = $this->db->get_results($query);
        
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
        
        $v = get_version_filter();
        
        if(!empty($v)) {
            $v = " AND ApplicationsID in ($v)";
        }
        else {
            $v = '';
        }
        
        $results = $this->db->get_results("select OptionId, Value, count(Value) as ".
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
