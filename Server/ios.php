<?php include('header.html'); ?>

<?php
$lang = substr($_SERVER['HTTP_ACCEPT_LANGUAGE'], 0, 2);
switch ($lang) {
    case "es":
        $extension = 'es';
        break;
    case "fr":
        $extension = 'fr';
        break;    
    case "en":
        $extension = 'en';
        break;    
    default:
        $extension = 'fr';
        break;
    }
?>

<div class="center">
    <div class="container">
    <span class="breadcrumbs"><a href="/" title="Inici">Inici</a> » Com configurar l'iOS en català</span>
    <h1 class="page">Catalanitzador <span class="so">de Softcatalà</span></h1>
    <h2>Com configurar l'iOS en català</h2>
    	<div class="download_section general">

<p>Per tal de catalanitzar un dispositiu que utilitzi el sistema operatiu iOS (utilitzat pels iPad i iPhone), seguiu les passes següents:</p>

<ul class="general">
    <li>Pitgeu la icona Ajustes (Settings si el teniu configurat en anglès o Réglages si el teniu en francès) per anar a les preferències del dispositiu mòbil.</li>
    <li>Seleccioneu l'opció General i després Internacional.</li>
    <li>Seleccioneu l'opció Idioma (Language si el teniu configurat en anglès o Langue si el teniu en francès) i escolliu el Català.</li>
    <li>Premeu OK per desar el canvi de configuració.</li>
    <li>Després d'uns segons us apareixerà la interfície en català.</li>
</ul>

<br class="p"/>

<p>Si teniu cap dubte o us cal ajuda per configurar el vostre mòbil o tauleta podeu adreçar-vos al <a href="http://www.softcatala.org/forum/viewforum.php?f=2&sid=04e08382c279bf4d89b5add56a481378">fòrum de Mac OS</a>.</p>

<br class="p"/>

		</div>    
        <div class="so_screenshot_section">
            <ul id="slider_android">
                <li><img src="/images/so/ios1_<?php echo $extension ?>.png" alt="Catalanitzar l'ios captura 1"/></li>
                <li><img src="/images/so/ios2_<?php echo $extension ?>.png" alt="Catalanitzar l'ios captura 2"/></li>
                <li><img src="/images/so/ios3_<?php echo $extension ?>.png" alt="Catalanitzar l'ios captura 3"/></li>
                <li><img src="/images/so/ios4_<?php echo $extension ?>.png" alt="Catalanitzar l'ios captura 4"/></li>
                <li><img src="/images/so/ios5_<?php echo $extension ?>.png" alt="Catalanitzar l'ios captura 5"/></li>
                <li><img src="/images/so/ios_ca.png" alt="Catalanitzar l'ios captura 5"/></li>
            </ul>
            
        </div>
	</div>
</div>

<?php include('footer.html'); ?>
