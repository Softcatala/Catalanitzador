<?php

class Menu {

    private static $_sections = array('inspectors', 'versions', 'options', 'logs');
    private static $DEFAULT = 'default';
    private static $_active = '';

    public static function css_menu($str = '') {
        if (self::is_active($str)) {
            return ' active ';
        } else {
            return '';
        }
    }

    public static function is_active($str = '') {
        if (empty($str)) {
            $str = self::$DEFAULT;
        }

        return ($str == self::get_active());
    }

    public static function get_active() {
        if (empty(self::$_active)) {
            self::$_active = self::$DEFAULT;

            if (isset($_GET['show']) && in_array($_GET['show'], self::$_sections)) {
                self::$_active = $_GET['show'];
            }
        }
        return self::$_active;
    }

    public static function include_active() {
        include(self::get_active() . '.php');
    }

}

?>
