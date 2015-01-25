<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ca" lang="ca" dir="ltr">
<?php require( 'lib/db.php' ); ?>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
        <title><?php echo $title; ?></title>
        <meta name="description" content="<?php echo $description; ?>" />
        <meta name="keywords" content="softcatala, catalanitzador, windows, mac os x, ios, android, linux" />
        <meta name="author" content="Softcatalà" />
        <link rel="shortcut icon" href="<?php echo $globals['base_domain'] ?>/favicon.ico" />
	    <link rel="image_src" href="<?php echo $globals['base_domain'] ?>/images/logo_catalanitzador.png" />

        <link rel="stylesheet" type="text/css" href="<?php echo $globals['base_domain'] ?>/css/styles.css" media="screen" />
        <link type="text/css" rel="stylesheet" media="all" href="<?php echo $globals['base_domain'] ?>/css/sc.css" />
        <link href="http://www.softcatala.org/css/cookies/cookiecuttr.css" rel="stylesheet" type="text/css" />
        <script src="<?php echo $globals['base_domain'] ?>/js/jquery.js" type="text/javascript"></script>
        <script src="<?php echo $globals['base_domain'] ?>/js/jquery.bxSlider.min.js" type="text/javascript"></script>
        <script src="<?php echo $globals['base_domain'] ?>/js/slider.js" type="text/javascript"></script>
        <script type="text/javascript" src="http://www.softcatala.org/js/cookies/jquery.cookie.js"></script>
        <script type="text/javascript" src="http://www.softcatala.org/js/cookies/jquery.cookiecuttr.js"></script>
        <script type="text/javascript" src="http://www.softcatala.org/js/softcatala.js"></script>

    <!--[if lt IE 7]>
     <script src="http://ie7-js.googlecode.com/svn/version/2.0(beta3)/IE7.js" 
     type="text/javascript">
     </script>
    <![endif]-->

        
        <!-- Sharing tags -->
        <meta property="og:title" content="Catalanitzador de Softcatalà" />
        <meta property="og:type" content="product" />

        <meta property="og:url" content="http://www.softcatala.org/catalanitzador" />
        <meta property="og:image" content="http://www.softcatala.org/catalanitzador/images/logo_share.png" />
        <meta property="og:site_name" content="Catalanitzador de Softcatalà" />
        <meta property="og:description" content="Poseu de forma automàtica el vostre Windows o Mac OS X en català! Amb aquesta aplicació podreu catalanitzar el vostre ordinador de forma molt senzilla." />
</head>
<body>
<?php include('header_ads.php'); ?>
<?php include('header_sc.php'); ?>
