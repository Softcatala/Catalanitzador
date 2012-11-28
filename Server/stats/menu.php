<?php
    $show = 'default';

    if (isset($_GET['show'])) {
        switch ($_GET['show']) {
            case 'inspectors':
                $show = 'inspectors';
                break;
            case 'versions':
                $show = 'versions';
                break;
            case 'options':
                $show = 'options';
                break;
        }
    }

    include($show . '.php');
?>