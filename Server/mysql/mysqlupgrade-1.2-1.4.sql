--
-- Estructura de la taula `options`
--

CREATE TABLE IF NOT EXISTS `options` (
  `SessionID` int(11) NOT NULL,
  `OptionID` int(11) NOT NULL,
  `Value` char(128) NOT NULL,
  KEY `OptionID` (`OptionID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;