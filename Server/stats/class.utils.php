<?php


class Utils {

    public static function get_query_string($key = '', $val = '') {
        $queryString = array();
        $queryString = $_GET;
        $queryString[$key] = $val;

        $toRemove = array();

        foreach ($queryString as $k => $v) {
            if ($queryString[$k] == '') {
                $toRemove[] = $k;
            }
        }

        foreach ($toRemove as $i => $k) {
            unset($queryString[$k]);
        }

        $qString = htmlspecialchars(http_build_query($queryString), ENT_QUOTES);

        return '?' . $qString;
    }
    
}
?>
