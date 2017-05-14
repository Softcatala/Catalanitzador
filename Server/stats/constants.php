<?php
    //Actions
    $actions = array( 
        0 => 'NoAction', 
        1 => 'WindowsLPI', 
        2 => 'MSOfficeLPI', 
        3 => 'ConfigureLocale', 
        4 =>'IEAcceptLanguage', 
        5 => 'IELPI', 
        6 => 'ConfigureDefaultLanguage', 
        7 => 'Chrome', 
        8=>'Firefox', 
        9 => 'OpenOffice', 
        10=>'AcrobatReader',
        11 => 'Windows Live', 
        12 => 'CatalanitzadorUpdate', 
        13 => 'Skype',
        14 => 'iTunes',
        15 => 'MacSystemAction',
        16 => 'MacSpellChecker',
        17 => 'FirefoxLangPack',
        18 => 'LangToolLibreOffice',
        19 => 'LangToolFirefox',
        20 => 'DictSynonymsActionID'
    );

    $subversions = array(4, 7, 8);

    // ActionStatus
    $action_status = array( 
        0 => 'NotSelected',       // The user did not select the action
        1 => 'Selected',          // Selected but no started
        2 => 'CannotBeApplied',   // The action cannot be applied (e.g. Windows ES needed US found or application not found)
        3 => 'AlreadyApplied',    // Action not needed
        4 => 'InProgress',        // Selected and in progress
        5 => 'Successful',        // End up OK
        6 => 'FinishedWithError', // End up with error
        7 => 'NotInstalled'       // Software is not installed
    );

    // actions we don't want to show in the main table
    $action_to_hide = array(1, 4);                            
    
    $system_platform = array(
        0 => 'Windows',
    1 => 'MacOS X'
    );

    // OS
    $os_names = array( 
        "6.1" => "Windows 7", 
        "6.0" => "Windows Vista", 
        "5.2" => "Windows XP x64",
        "5.1" => "Windows XP", 
        "5.0" => "Windows 2000"
    );
?>
