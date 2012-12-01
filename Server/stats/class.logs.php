<?php

class Logs {
    
    private $_db;
    private $_logs;
    
    public function __construct($db) {
        $this->_db = $db;
        $this->_logs = array();
    }
    
    private function get_logs($start_date,$end_date) {
        $id = $start_date.'-'.$end_date;
        if(!isset($this->_logs[$id])) {
            $this->_logs[$id] = array();
            
            $start = DateTime::createFromFormat('d/m/Y',$start_date);
            $end = DateTime::createFromFormat('d/m/Y', $end_date);
            
            if($start === false || $end === false) {
                return $this->_logs[$id];
            }
            
            $start = $start->format('Y-m-d');
            $end = $end->format('Y-m-d').' 23:59:59';
            $sql = "select ID, Date, LogFile from sessions where LogFile != '' and Date >= '$start' and Date <= '$end'";
            
            $results = $this->_db->get_results($sql);
            if($results) {
                foreach($results as $result) {
                    $this->_logs[$id][$result->ID] = array($result->Date,$result->LogFile);
                }
            }
        }
        
        return $this->_logs[$id];
    }
    
    public function print_logs($start,$end) {
        $logs = $this->get_logs($start, $end);
        foreach($logs as $id => $log) {
            if(empty($log[1])) {
                $empty = ' wrap-log-empty ';
            } else {
                $empty = '';
            }
?>
<div id="wrap-log-<?=$id?>" class="<?=$empty?>">
    <h3 style="cursor:pointer;text-decoration:underline" id="log-<?=$id?>" class="log-id"><?=$id?> (<?=$log[0]?>)</h3>
    <pre id="content-log-<?=$id?>" style="border:1px solid black;padding: 5px;"><?=$log[1]?></pre>
</div>
<?php
        }
            
    }
}
?>
