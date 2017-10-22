<?php

class Version
{
    private $sql;
    private $db;
    private $v;

    public function __construct($v)
    {
        global $db;

        $this->sql = false;

        $this->db = $db;
        $this->v = $v;
    }

    public function get_sql()
    {

        if ($this->sql === false) {

            if (empty($this->v) || (strlen($this->v) < 3) || (strlen($this->v) > 4)) {
                $this->sql = '';
            } else {
                if (is_numeric($this->v)) {
                    $this->get_numeric_version();
                } else if ($this->v[2] == 'x' && is_numeric(substr($this->v, 0, 2))) {
                    $this->get_minor_version();
                } else if (($this->v[2] == 'x' && $this->v[1] == 'x') && is_numeric(substr($this->v, 0, 1))) {
                    $this->get_major_version();
                }
            }
        }

        return $this->sql;
    }

    private function get_numeric_version()
    {
        $major = $this->v[0];
        $minor = $this->v[1];
        $revision = substr($this->v, 2);

        $query = "select ID from applications "
            . " where MajorVersion = $major and MinorVersion = $minor and "
            . " Revision = $revision ";

        $version = $this->db->get_results($query);
        $this->sql = $version[0]->ID;
    }

    private function get_minor_version()
    {
        $version = $this->db->get_results("select ID from applications "
            . " where MajorVersion = $this->v[0] and MinorVersion = $this->v[1]");

        $firstVersion = true;
        foreach ($version as $oneVersion) {
            if ($firstVersion) {
                $firstVersion = false;
            } else {
                $this->sql .= ',';
            }

            $this->sql .= $oneVersion->ID;
        }
    }

    private function get_major_version()
    {
        $version = $this->db->get_results("select ID from applications where MajorVersion = $this->v[0]");

        $firstVersion = true;
        foreach ($version as $oneVersion) {
            if ($firstVersion) {
                $firstVersion = false;
            } else {
                $this->sql .= ',';
            }

            $this->sql .= $oneVersion->ID;
        }
    }
}