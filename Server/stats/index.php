<?php
    require_once '../lib/db_stats.php';
    require_once 'class.utils.php';
    require_once 'class.catalanitzador.php';

    global $db;
    $Catalanitzador = new Catalanitzador($db);
    
    require_once 'functions.php';
    require_once 'constants.php';
    require_once 'header.php';

?>
    <body>
        <h1>Estadístiques del <a href="http://catalanitzador.softcatala.org" 
            title="Catalanitzador per al Windows">Catalanitzador</a> de 
        <a href="http://www.softcatala.org">Softcatalà</a></h1>
        <div class="last_update">
            Última actualització de les dades: <em><?=date('H:j:s d/m/Y')?></em>
        </div>
        <div id="menu">
            <a href="<?=Utils::get_query_string('show','')?>" class="<?=css_is_active('')?>">Resum</a><a href="<?=Utils::get_query_string('show','versions')?>" class="<?=css_is_active('versions')?>">Versions</a><a href="<?=Utils::get_query_string('show','inspectors')?>" class="<?=css_is_active('inspectors')?>">Inspectors</a><a href="<?=Utils::get_query_string('show','options')?>" class="<?=css_is_active('options')?>">Opcions</a>
        </div>
        <div id="totals" style="witdh:800px;margin:0 auto"> <div>
            <h2>Versions del Catalanitzador</h2>
            <?php $Catalanitzador->print_versions_table(); ?>
        </div>
        <br />
<?php
    include('menu.php');
?>
</html>
