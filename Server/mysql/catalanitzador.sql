-- phpMyAdmin SQL Dump
-- version 3.3.7deb7
-- http://www.phpmyadmin.net
--
-- Servidor: localhost
-- Temps de generació: 08-03-2012 a les 08:20:23
-- Versió del servidor: 5.1.49
-- Versió de PHP : 5.3.8-2~bpo60+1

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

--
-- Base de dades: `catalanitzador`
--

-- --------------------------------------------------------

--
-- Estructura de la taula `actions`
--

CREATE TABLE IF NOT EXISTS `actions` (
  `SessionID` int(11) NOT NULL,
  `ActionID` int(11) NOT NULL,
  `Version` char(16) NOT NULL,
  `Result` int(11) NOT NULL,
  KEY `SessionID` (`SessionID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Estructura de la taula `applications`
--

CREATE TABLE IF NOT EXISTS `applications` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `MajorVersion` tinyint(4) NOT NULL,
  `MinorVersion` tinyint(4) NOT NULL,
  `Revision` tinyint(4) NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=8 ;

-- --------------------------------------------------------

--
-- Estructura de la taula `operatings`
--

CREATE TABLE IF NOT EXISTS `operatings` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `OSMajorVersion` tinyint(4) NOT NULL,
  `OSMinorVersion` tinyint(4) NOT NULL,
  `SPMajorVersion` tinyint(4) NOT NULL,
  `SPMinorVersion` tinyint(4) NOT NULL,
  `SuiteMask` int(11) NOT NULL,
  `ProductType` int(11) NOT NULL,
  `Name` text NOT NULL,
  `Bits` tinyint(4) NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=46 ;

-- --------------------------------------------------------

--
-- Estructura de la taula `sessions`
--

CREATE TABLE IF NOT EXISTS `sessions` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `Date` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  `ApplicationsID` int(11) NOT NULL,
  `OperatingsID` int(11) NOT NULL,
  `LogFile` text NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1527 ;

