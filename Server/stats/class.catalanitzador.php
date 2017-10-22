<?php

class Catalanitzador {

    private $_db;
    private $_versions;
    private $_platforms;
    private $_vselected;
    private $_pselected;
    private $_sql_vselected;
    private $_sql_pselected;
    private $_date_from;
    private $_date_to;
    private $_version;

    public function __construct($db) {
        $this->_db = $db;
        $this->_versions = null;
        $this->_platforms = null;
        $this->_vselected = '';
        $this->_pselected = '';
        $this->_sql_vselected = '';
        $this->_date_from = '';
        $this->_date_to = '';
        $this->_version = new Version($db, $_GET['v']);
    }

    public function is_version_selected() {
        return !empty($this->_vselected);
    }

    public function is_platform_selected() {
        return $this->_pselected != '';
    }

    private function is_date_from_selected() {
        return $this->get_date_from() != '';   
    }
    
    private function is_date_to_selected() {
        return $this->get_date_to() != '';
    }
    
    public function is_date_selected() {
        return $this->is_date_from_selected() || $this->is_date_to_selected();
    }
    
    private function get_date_to($toweb = false) {
        $to = $_GET['to'];
        
        if(is_numeric($to) && strlen($to) == 8) {
            
            $date = DateTime::createFromFormat('dmY', $to);
            if($date) {
                $date->add(DateInterval::createFromDateString('1 day'));

                if($toweb) {
                    return $date->format('d/m/Y');
                }
                else {
                    return $date->format('Y-m-d 00:00:00');
                }
            }                  
        } 
        return '';
    }

    private function get_date_from($toweb = false) {
        $from = $_GET['from'];
        
        if(is_numeric($from) && strlen($from) == 8) {
            
            $date = DateTime::createFromFormat('dmY', $from);
            
            if($date) {
                if($toweb) {
                    return $date->format('d/m/Y');
                }
                else {
                    return $date->format('Y-m-d 00:00:00');
                }
            }
        }
        return '';
    }
    
    public function get_dates() {
        
        $d = '';
        if($this->is_date_selected()) {           
            if($this->is_date_from_selected()) {
                $d .= " AND sessions.Date >= '" . $this->get_date_from() . "' ";
            }
            if($this->is_date_to_selected()) {
                $d .= " AND sessions.Date <= '" . $this->get_date_to() . "' ";
            }
            return $d;
        }
        return '';
    }
    
    public function get_versions() {

        if ($this->_versions == null) {
        
            $p = '';
            if($this->is_platform_selected()) {
                $p = $this->get_platform_selected();
                $p = " AND operatings.System = '$p'";
            }

            $d = $this->get_dates();

            $result_query = " select ApplicationsID, count(sessions.ID) as total from sessions, operatings "
                          . " where sessions.OperatingsID = operatings.ID $p $d group by ApplicationsID;";
            
            
            echo "<!-- $result_query -->";
            $results = $this->_db->get_results($result_query);
            $r_versions = $this->_db->get_results("select * from applications order by ID asc");
            $versions = array();
            $totals = array();
            foreach ($r_versions as $k => $version) {
                $versions["ID-" . $version->ID] = $version->MajorVersion . '.'
                        . $version->MinorVersion . '.' . $version->Revision;
                $totals["ID-" . $version->ID] = 0;
            }

            foreach ($results as $k => $result) {
                $totals["ID-" . $result->ApplicationsID] = $result->total;
            }

            asort($versions);

            $this->_versions = array($versions, $totals);
        }

        return $this->_versions;
    }

    public function get_platforms() {

        if ($this->_platforms == null) {
            $results = $this->_db->get_results("select distinct System from operatings where System != '';");
            $platforms = array();
            foreach ($results as $platform) {
                $platforms[] = 'OS-'.$platform->System; 
            }

            $this->_platforms = $platforms;
        }

        return $this->_platforms;
    }


    public function get_version_selected() {
        return $this->_version->get_sql();
    }

    public function get_platform_selected() {
        if (!$this->is_platform_selected()) {
            return '';
        }

        if(empty($this->_sql_pselected)) {
            $p = $_GET['os'];
            if($p == '') {
                return '';
            }

            $this->_sql_pselected = $p;
        }

        return $this->_sql_pselected;
    }

    function print_platforms_table() {
    global $system_platform;
    ?>
        <table id="application_platform">
            <thead>
                <tr>
        <?php
        $platform_data = $this->get_platforms();

         foreach ($platform_data as $platform) {
            $p = str_replace('OS-','',$platform);
            if ($_GET['os'] == $p) {
                $style = ' class="active" ';
                $this->_pselected = $p;
            } else {
                $style = '';
            }
            echo '<th ', $style, '><a href="';
            echo Utils::get_query_string('os', $p);
            echo '">', $system_platform[$p], '</a></th>';

        }

        if ($this->is_platform_selected()) {
            echo '<th>';
        } else {
            echo '<th style="background-color:#CCFFCC">';
        }

        echo '<a href="', Utils::get_query_string('os', ''), '">TOTES</a></th></tr>';    
    ?>

    </thead></table><br />
    <?php
    }

    function print_versions_table() {
        ?>
        <table id="application_version">
            <thead>
                <tr>
        <?php
        $version_data = $this->get_versions();
        $this->_vselected = '';

        $numOfCols = 0;
        $lastQueryVersion = '';
        $lastFullVersion = '';

        foreach ($version_data[0] as $ID => $version) {
            $fullVersion = substr($version, 0, -1) . 'x';
            $queryVersion = str_replace('.', '', $fullVersion);

            if (!empty($lastQueryVersion) && ($lastQueryVersion != $queryVersion)) {
                if ($_GET['v'] == $lastQueryVersion) {
                    $style = ' class="active" ';
                    $this->_vselected = $lastQueryVersion;
                } else {
                    $style = '';
                }

                echo '<th colspan="', $numOfCols, '" ', $style, '><a href="';
                echo Utils::get_query_string('v', $lastQueryVersion);
                echo '">', $lastFullVersion, '</a></th>';
                $numOfCols = 0;
            }

            $numOfCols++;

            $lastQueryVersion = $queryVersion;
            $lastFullVersion = $fullVersion;
        }

        if ($_GET['v'] == $lastQueryVersion) {
            $style = ' class="active" ';
            $this->_vselected = $lastQueryVersion;
        } else {
            $style = '';
        }

        echo '<th colspan="', $numOfCols, '" ', $style, '><a href="';
        echo Utils::get_query_string('v', $lastQueryVersion);
        echo '">', $lastFullVersion, '</a></th>';
        echo '<th>&nbsp;</th>';

        echo '</tr><tr>';

        foreach ($version_data[0] as $ID => $version) {
            $v = str_replace('.', '', $version);
            if ($_GET['v'] == $v) {
                $style = ' class="active" ';
                $this->_vselected = $lastQueryVersion;
            } else {
                $style = '';
            }
            echo '<th ', $style, '><a href="';
            echo Utils::get_query_string('v', $v);
            echo '">', $version, '</a></th>';
        }

    if($this->_vselected == '' && strlen($_GET['v']) == 3) {
        if($_GET['v'][1]=='x' && $_GET['v'][2]=='x') {
            $this->_vselected = $_GET['v'];
        }
    }

        if ($this->is_version_selected()) {
            echo '<th>';
        } else {
            echo '<th style="background-color:#CCFFCC">';
        }

        echo '<a href="', Utils::get_query_string('v', ''), '">TOTES</a></th></tr>';

        echo '</thead><tbody><tr>';
        $total_v = 0;
        foreach ($version_data[0] as $ID => $version) {
            echo '<td>';
            if (isset($version_data[1][$ID])) {
                echo $version_data[1][$ID];
                $total_v += $version_data[1][$ID];
            } else {
                echo 0;
            }
            echo '</td>';
        }
        echo '<td>', $total_v, '</td>';
        ?>
                </tr>
                </tbody>
        </table>
        <?php
    }

    public function print_date_selector() {
        ?>
            <script>
                (function($) {
                    $(document).ready(function(){
                       $( "#from" ).datepicker({
                        defaultDate: "-1d",
                        changeMonth: true,
                        dateFormat: 'dd/mm/yy',
                        onClose: function( selectedDate ) {
                            $( "#to" ).datepicker( "option", "minDate", selectedDate );
                        }

                    });
                    $( "#to" ).datepicker({
                        defaultDate: "today",
                        changeMonth: true,
                        dateFormat: 'dd/mm/yy',
                        onClose: function( selectedDate ) {
                            $( "#from" ).datepicker( "option", "maxDate", selectedDate );
                        }
                    });

                    $('#addDates').click(function(){
                        var dateFrom = $('#from').val();
                        var dateTo = $('#to').val();
                        
                        var url = new $.Url(location.href);
                        url.set('from', dateFrom.replace(/\//g,''));
                        url.set('to', dateTo.replace(/\//g,''));
                        
                        location.href=url.toString();
                    });
                    
                    $('#cleanDates').click(function(){
                        var url = new $.Url(location.href);
                        url.remove('from');
                        url.remove('to');
                        
                        location.href=url.toString();
                    }); 
                    });
                })(jQuery);
                </script>
                <form action="#">
                    <strong>Filtra per dates</strong><br />
                    <label for="from">Data inicial:</label>
                    <input type="text" id="from" name="from" value="<?=$this->get_date_from(TRUE)?>"/> 
                    <label for="to">Data final:</label>
                    <input type="text" id="to" name="to" value="<?=$this->get_date_to(TRUE)?>"/> <br />
                    <input type="button" id="addDates" value="Actualitza" />
                    <input type="button" id="cleanDates" value="Neteja les dates" /><br /><br />
                </form>
        <?php
        
    }

}
?>
