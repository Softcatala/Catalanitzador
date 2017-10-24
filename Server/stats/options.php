<?php

include('class.options.php');
global $db;
global $Catalanitzador;
global $Version;
?>
        <h2>Opcions</h2>
        <em>
                Aquestes gràfiques representen les diferents opcions que 
                han seleccionat els usuaris
        </em>
<?php
        $statsOption = new StatsOptions($db,$Catalanitzador,$Version);
        
        $allOptions = $statsOption->GetOptions();
        foreach($allOptions as $optionId => $optionName) {
            $values = $statsOption->GetOptionValues($optionId);
            
?>          <h3><?=$optionName?></h3>
            <div id="_<?=$optionId?>_options" style="height: 300px; margin: 0 auto"></div>
            <script type="text/javascript">
                    <?php print_javascript_options_chart('_'.$optionId.'_options',$optionName,$values); ?>
            </script>
<?php
        }
?>