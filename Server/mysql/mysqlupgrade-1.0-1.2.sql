CREATE TABLE IF NOT EXISTS `inspectors` (
  `SessionID` int(11) NOT NULL,
  `InspectorID` int(11) NOT NULL,
  `Key` char(128) NOT NULL,
  `Value` char(128) NOT NULL,
  KEY `InspectorID` (`InspectorID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

ALTER TABLE `sessions` ADD `guid` VARCHAR( 255 ) NOT NULL 