<?php
    require_once '../lib/db_stats.php';
    require_once 'class.utils.php';
    require_once 'class.menu.php';
    require_once 'class.catalanitzador.php';

    global $db;
    $Catalanitzador = new Catalanitzador($db);
    
    require_once 'functions.php';
    require_once 'constants.php';
    require_once 'header.php';

?>
    <body>
        <h1>Estadístiques del <a href="http://www.softcatala.org/catalanitzador" 
            title="Catalanitzador per al Windows">Catalanitzador</a> de 
        <a href="http://www.softcatala.org">Softcatalà</a></h1>
        <div class="last_update">
            Última actualització de les dades: <em><?=date('H:j:s d/m/Y')?></em>
        </div>
        <div id="menu">
            <a href="<?=Utils::get_query_string('show','')?>" class="<?= Menu::css_menu('')?>">Resum</a><a 
               href="<?=Utils::get_query_string('show','versions')?>" class="<?=Menu::css_menu('versions')?>">Versions</a><a 
               href="<?=Utils::get_query_string('show','inspectors')?>" class="<?=Menu::css_menu('inspectors')?>">Inspectors</a><a
               href="<?=Utils::get_query_string('show','options')?>" class="<?=Menu::css_menu('options')?>">Opcions</a><a
               href="<?=Utils::get_query_string('show','logs')?>" class="<?=Menu::css_menu('logs')?>">Logs</a>
            <br />
        </div>
<?php
    if(!Menu::is_active('logs')) {
?>    
        <div id="totals" style="witdh:800px;margin:0 auto"> <div>
            <h2>Versions del Catalanitzador</h2>
            <?php $Catalanitzador->print_platforms_table(); ?>
            <?php $Catalanitzador->print_versions_table(); ?>
            <?php $Catalanitzador->print_date_selector(); ?>
        </div>
        <br />
<?php
    }
    
    Menu::include_active();
?>
<?php
    include('footer.php');
?>
</html>
