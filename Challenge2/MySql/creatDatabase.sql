-- MySQL Script generated by MySQL Workbench
-- 09/24/15 16:47:04
-- Model: New Model    Version: 1.0
-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

-- -----------------------------------------------------
-- Schema mydb
-- -----------------------------------------------------
DROP SCHEMA IF EXISTS `mydb` ;

-- -----------------------------------------------------
-- Schema mydb
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `mydb` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci ;
USE `mydb` ;

-- -----------------------------------------------------
-- Table `mydb`.`sensors`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `mydb`.`sensors` ;

CREATE TABLE IF NOT EXISTS `mydb`.`sensors` (
  `id` INT NOT NULL COMMENT '',
  `x` DOUBLE NOT NULL COMMENT '',
  `y` DOUBLE NOT NULL COMMENT '',
  PRIMARY KEY (`id`)  COMMENT '')
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `mydb`.`temperatures`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `mydb`.`temperatures` ;

CREATE TABLE IF NOT EXISTS `mydb`.`temperatures` (
  `row` INT NOT NULL AUTO_INCREMENT COMMENT '',
  `time` DATETIME NOT NULL COMMENT '',
  `temp` DOUBLE NOT NULL COMMENT '',
  `sensors_id` INT NOT NULL COMMENT '',
  PRIMARY KEY (`row`)  COMMENT '',
  INDEX `fk_temperatures_sensors_idx` (`sensors_id` ASC)  COMMENT '',
  CONSTRAINT `fk_temperatures_sensors`
    FOREIGN KEY (`sensors_id`)
    REFERENCES `mydb`.`sensors` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `mydb`.`average`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `mydb`.`average` ;

CREATE TABLE IF NOT EXISTS `mydb`.`average` (
  `row` INT NOT NULL AUTO_INCREMENT COMMENT '',
  `average` FLOAT NOT NULL COMMENT '',
  `time` DATETIME NOT NULL COMMENT '',
  PRIMARY KEY (`row`)  COMMENT '')
ENGINE = InnoDB;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
