<?php

class Catalanitzador {

    private $_db;
    private $_versions;
    private $_vselected;
    private $_sql_vselected;

    public function __construct($db) {
        $this->_db = $db;
        $this->_versions = null;
        $this->_vselected = '';
        $this->_sql_vselected = '';
    }

    public function is_version_selected() {
        return !empty($this->_vselected);
    }

    public function get_versions() {

        if ($this->_versions == null) {
            $results = $this->_db->get_results("select ApplicationsID, count(*) as total from sessions group by ApplicationsID;");
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

    public function get_version_selected() {
        if (!$this->is_version_selected()) {
            return '';
        }

        if (empty($this->_sql_vselected)) {
            $v = $_GET['v'];
            if (empty($v) || strlen($v) != 3)
                return;

            if (is_numeric($v)) {
                $version = $this->_db->get_results("select ID from applications "
                        . " where MajorVersion = $v[0] and MinorVersion = $v[1] and "
                        . " Revision = $v[2] ");
                $this->_sql_vselected = $version[0]->ID;
            } else if ($v[2] == 'x' && is_numeric(substr($v, 0, 2))) {
                $version = $this->_db->get_results("select ID from applications "
                        . " where MajorVersion = $v[0] and MinorVersion = $v[1]");

                $firstVersion = true;
                foreach ($version as $oneVersion) {
                    if ($firstVersion) {
                        $firstVersion = false;
                    } else {
                        $this->_sql_vselected .= ',';
                    }

                    $this->_sql_vselected .= $oneVersion->ID;
                }
            }
        }

        return $this->_sql_vselected;
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

}
?>