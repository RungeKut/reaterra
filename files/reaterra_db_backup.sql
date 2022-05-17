-- MySQL dump 10.17  Distrib 10.3.25-MariaDB, for debian-linux-gnueabihf (armv7l)
--
-- Host: localhost    Database: REATERRA
-- ------------------------------------------------------
-- Server version	10.3.25-MariaDB-0+deb10u1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Интервалы`
--

DROP TABLE IF EXISTS `Интервалы`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Интервалы` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `Интервал` varchar(45) NOT NULL,
  `Порядок` float NOT NULL,
  `Тип` varchar(45) NOT NULL,
  `Длительность` int(11) NOT NULL,
  `Скорость` decimal(10,2) NOT NULL,
  `Угол` float NOT NULL,
  `Ускорение` float DEFAULT NULL,
  `idПротокола` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `кПротоколам_idx` (`id`,`Тип`),
  KEY `кПротоколу_idx` (`idПротокола`),
  CONSTRAINT `кПротоколу` FOREIGN KEY (`idПротокола`) REFERENCES `Протоколы` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=275 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Интервалы`
--

LOCK TABLES `Интервалы` WRITE;
/*!40000 ALTER TABLE `Интервалы` DISABLE KEYS */;
INSERT INTO `Интервалы` VALUES (1,'1',1,'0',180,2.70,5.71059,NULL,1),(2,'2',2,'0',180,4.00,6.84277,NULL,1),(3,'3',3,'0',180,5.40,7.96961,NULL,1),(4,'4',4,'0',180,6.70,9.09028,NULL,1),(5,'5',5,'0',180,8.00,10.204,NULL,1),(6,'6',6,'0',180,8.80,11.3099,NULL,1),(7,'7',7,'0',180,9.60,12.4074,NULL,1),(8,'1',1,'0',180,2.70,0,NULL,2),(9,'2',2,'0',180,2.70,2.86241,NULL,2),(10,'3',3,'0',180,2.70,5.71059,NULL,2),(11,'4',4,'0',180,4.00,6.84277,NULL,2),(12,'5',5,'0',180,5.40,7.96961,NULL,2),(13,'6',6,'0',180,6.70,9.09028,NULL,2),(14,'7',7,'0',180,8.00,10.204,NULL,2),(15,'8',8,'0',180,8.80,11.3099,NULL,2),(16,'9',9,'0',180,9.60,12.4074,NULL,2),(17,'1',1,'0',180,1.90,0,NULL,3),(18,'2',2,'0',180,1.90,1.71836,NULL,3),(19,'3',3,'0',180,1.90,3.43363,NULL,3),(20,'4',4,'0',180,2.70,3.43363,NULL,3),(21,'1',1,'0',180,3.00,0,NULL,4),(22,'2',2,'0',180,3.00,2.00453,NULL,4),(23,'3',3,'0',180,3.00,4.00417,NULL,4),(24,'4',4,'0',180,3.00,5.99409,NULL,4),(25,'5',5,'0',180,3.00,7.96961,NULL,4),(26,'6',6,'0',180,3.00,9.92625,NULL,4),(27,'1',1,'0',120,1.60,0,NULL,5),(28,'2',2,'0',120,3.20,0,NULL,5),(29,'3',3,'0',120,3.20,2.00453,NULL,5),(30,'4',4,'0',120,3.20,4.00417,NULL,5),(31,'5',5,'0',120,3.20,5.99409,NULL,5),(32,'6',6,'0',120,3.20,7.96961,NULL,5),(33,'7',7,'0',120,3.20,9.92625,NULL,5),(34,'8',8,'0',120,3.20,11.8598,NULL,5),(35,'1',1,'0',60,5.30,1.14576,NULL,6),(36,'2',2,'0',60,5.30,1.71836,NULL,6),(37,'3',3,'0',60,5.30,2.29061,NULL,6),(38,'4',4,'0',60,5.30,2.86241,NULL,6),(39,'5',5,'0',60,5.30,3.43363,NULL,6),(40,'6',6,'0',60,5.30,4.00417,NULL,6),(41,'7',7,'0',60,5.30,5.14276,NULL,6),(42,'1',1,'0',120,3.20,0,NULL,7),(43,'2',2,'0',120,3.20,1.4321,NULL,7),(44,'3',3,'0',120,3.20,2.86241,NULL,7),(45,'4',4,'0',120,3.20,4.28915,NULL,7),(46,'5',5,'0',120,3.20,5.71059,NULL,7),(47,'6',6,'0',120,3.20,7.12502,NULL,7),(48,'7',7,'0',120,3.20,8.53077,NULL,7),(49,'1',1,'0',120,4.80,1.4321,NULL,8),(50,'2',2,'0',120,4.80,2.86241,NULL,8),(51,'3',3,'0',120,4.80,4.28915,NULL,8),(52,'4',4,'0',120,4.80,5.71059,NULL,8),(53,'5',5,'0',120,4.80,7.12502,NULL,8),(54,'6',6,'0',120,4.80,8.53077,NULL,8),(55,'7',7,'0',120,4.80,9.92625,NULL,8),(56,'1',1,'0',120,5.50,1.14576,NULL,9),(57,'2',2,'0',120,5.50,2.29061,NULL,9),(58,'3',3,'0',120,5.50,3.43363,NULL,9),(59,'4',4,'0',120,5.50,4.57392,NULL,9),(60,'5',5,'0',120,5.50,5.71059,NULL,9),(61,'6',6,'0',120,5.50,6.84277,NULL,9),(62,'7',7,'0',120,5.50,7.96961,NULL,9),(63,'1',1,'0',180,2.70,5.71059,NULL,10),(64,'2',2,'0',120,4.80,5.71059,NULL,10),(65,'3',3,'0',120,6.40,5.71059,NULL,10),(66,'4',4,'0',180,8.00,5.71059,NULL,10),(67,'5',5,'0',120,8.00,8.53077,NULL,10),(68,'6',6,'0',180,9.70,8.53077,NULL,10),(69,'0',1,'0',120,2.70,0,NULL,11),(70,'0.5',2,'0',120,2.70,2.86241,NULL,11),(71,'1.0',3,'0',120,2.70,5.71059,NULL,11),(72,'1.5',4,'0',120,3.40,6.2773,NULL,11),(73,'2.0',5,'0',120,4.00,6.84277,NULL,11),(74,'2.5',6,'0',120,4.80,7.40691,NULL,11),(75,'3.0',7,'0',120,5.40,7.96961,NULL,11),(76,'3.5',8,'0',120,6.20,8.53077,NULL,11),(77,'4.0',9,'0',120,6.70,9.09028,NULL,11),(78,'4.5',10,'0',120,7.40,9.64805,NULL,11),(79,'5.0',11,'0',120,8.10,10.204,NULL,11),(80,'1',1,'0',120,2.70,5.71059,NULL,12),(81,'2',2,'0',120,3.20,5.71059,NULL,12),(82,'3',3,'0',120,4.00,5.71059,NULL,12),(83,'4',4,'0',120,4.80,5.71059,NULL,12),(84,'5',5,'0',120,5.60,5.71059,NULL,12),(85,'6',6,'0',120,6.40,5.71059,NULL,12),(86,'7',7,'0',120,6.40,7.96961,NULL,12),(87,'1',1,'0',60,2.40,0,NULL,13),(88,'2',2,'0',60,3.20,0,NULL,13),(89,'3',3,'0',60,3.20,0.859372,NULL,13),(90,'4',4,'0',60,3.20,1.71836,NULL,13),(91,'5',5,'0',60,4.00,1.71836,NULL,13),(92,'6',6,'0',60,4.00,2.86241,NULL,13),(93,'7',7,'0',60,4.00,4.00417,NULL,13),(94,'8',8,'0',60,4.80,4.00417,NULL,13),(95,'9',9,'0',60,4.80,5.14276,NULL,13),(96,'10',10,'0',60,4.80,6.2773,NULL,13),(97,'11',11,'0',60,5.60,6.2773,NULL,13),(98,'12',12,'0',60,5.60,7.40691,NULL,13),(99,'13',13,'0',60,5.60,9.09028,NULL,13),(100,'14',14,'0',60,6.70,9.09028,NULL,13),(101,'15',15,'0',60,8.00,9.09028,NULL,13),(102,'0',1,'1',12,3.20,0,NULL,14),(103,'1',2,'0',120,3.20,0,NULL,14),(104,'2',3,'0',120,3.20,1.14576,NULL,14),(105,'3',4,'0',120,3.20,2.29061,NULL,14),(106,'4',5,'0',120,3.20,3.43363,NULL,14),(107,'5',6,'0',120,3.20,4.57392,NULL,14),(108,'6',7,'0',120,3.20,5.71059,NULL,14),(109,'7',8,'0',120,3.20,6.84277,NULL,14),(110,'8',9,'0',120,3.20,7.96961,NULL,14),(111,'9',10,'0',120,3.20,9.09028,NULL,14),(112,'10',11,'0',120,3.20,10.204,NULL,14),(113,'11',12,'0',1800,3.20,10.204,NULL,14),(133,'1',1,'0',60,1.00,0,1,-1);
/*!40000 ALTER TABLE `Интервалы` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `История`
--

DROP TABLE IF EXISTS `История`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `История` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `Время` datetime NOT NULL DEFAULT current_timestamp(),
  `Пациент` int(11) NOT NULL,
  `Профиль` varchar(45) DEFAULT NULL,
  `Длительность` int(11) DEFAULT NULL,
  `Расстояние` float DEFAULT NULL,
  `Скорость` float DEFAULT NULL,
  `Ускорение` float DEFAULT NULL,
  `Угол` float DEFAULT NULL,
  `ПульсМин` int(11) DEFAULT NULL,
  `ПульсМакс` int(11) DEFAULT NULL,
  `Энергия` float DEFAULT NULL,
  `Мощность` float DEFAULT NULL,
  `МощностьАэробная` float DEFAULT NULL,
  `ТипПрофиля` varchar(45) DEFAULT NULL,
  `Пульс` int(11) DEFAULT NULL,
  `ДлинаШага` int(11) DEFAULT NULL,
  `СимметрияЛевая` int(11) DEFAULT NULL,
  `СимметрияПравая` int(11) DEFAULT NULL,
  `РежимСАС` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=189 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `История`
--

LOCK TABLES `История` WRITE;
/*!40000 ALTER TABLE `История` DISABLE KEYS */;
/*!40000 ALTER TABLE `История` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Параметры`
--

DROP TABLE IF EXISTS `Параметры`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Параметры` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `Имя` varchar(45) NOT NULL,
  `Значение` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `Имя_UNIQUE` (`Имя`)
) ENGINE=InnoDB AUTO_INCREMENT=23 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Параметры`
--

LOCK TABLES `Параметры` WRITE;
/*!40000 ALTER TABLE `Параметры` DISABLE KEYS */;
INSERT INTO `Параметры` VALUES (1,'ПарольАдминистратора','357'),(2,'ПарольВрача','12345'),(3,'ЕдиницыУгол','град'),(4,'ЕдиницыРасстояние','км'),(5,'ЕдиницыЭнергия','ккал'),(6,'ЕдиницыМощность','Вт'),(7,'ЕдиницыМощностьАэробная','МЕТ'),(8,'ЕдиницыСкорость','км/ч'),(9,'СенсорЧСС',''),(10,'СенсорЧССАдрес',''),(11,'СенсорЧССНазвание',''),(12,'МаксимальноДопустимаяЧСС','200'),(13,'СкоростьШаганияБОС','4.0'),(14,'УровеньУскоренияДорожки','3'),(15,'УровеньТорможенияДорожки','3'),(17,'ОтключитьПарольВрача','0'),(18,'db_version','1'),(21,'ПараметрыБocНижний','0.1^0.5^25^0.2^0.5^25^0.3^0.5^25^0.5^1.0^25^0.5^1.0^25'),(22,'ПараметрыБocВерхний','0.5^0.5^12^0.8^1.0^12^1.0^1.0^ 10^1.2^1.2^8^2.0^1.6^7');
/*!40000 ALTER TABLE `Параметры` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Пациенты`
--

DROP TABLE IF EXISTS `Пациенты`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Пациенты` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `Фамилия` varchar(45) NOT NULL,
  `Имя` varchar(45) DEFAULT NULL,
  `Отчество` varchar(45) DEFAULT NULL,
  `Пол` varchar(1) DEFAULT NULL,
  `Возраст` int(11) DEFAULT NULL,
  `Диагноз` varchar(45) DEFAULT NULL,
  `Врач` varchar(45) DEFAULT NULL,
  `Профиль` int(11) DEFAULT NULL,
  `Вес` int(11) DEFAULT NULL,
  `Номер` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Пациенты`
--

LOCK TABLES `Пациенты` WRITE;
/*!40000 ALTER TABLE `Пациенты` DISABLE KEYS */;
/*!40000 ALTER TABLE `Пациенты` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Протоколы`
--

DROP TABLE IF EXISTS `Протоколы`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Протоколы` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `Тип` varchar(45) NOT NULL,
  `Порядок` float NOT NULL,
  `Название` varchar(45) NOT NULL,
  `Описание` varchar(255) NOT NULL DEFAULT '',
  `Модификация` varchar(45) NOT NULL DEFAULT '',
  `ДлительностьЕд` varchar(45) NOT NULL DEFAULT 'сек',
  `ДлительностьФмт` varchar(45) NOT NULL DEFAULT '4.0',
  `СкоростьЕд` varchar(45) NOT NULL DEFAULT 'км/ч',
  `СкоростьФмт` varchar(45) NOT NULL DEFAULT '4.1',
  `УголЕд` varchar(45) NOT NULL DEFAULT 'град',
  `УголФмт` varchar(45) NOT NULL DEFAULT '4.1',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=42 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Протоколы`
--

LOCK TABLES `Протоколы` WRITE;
/*!40000 ALTER TABLE `Протоколы` DISABLE KEYS */;
INSERT INTO `Протоколы` VALUES (-1,'USER',0,'','','','сек','4.0','км/ч','4.1','град','4.1'),(1,'ПРОТ10',0,'Bruce','','Классический','мин','4.0','км/ч','4.1','%','4.1'),(2,'ПРОТ10',0.1,'Bruce','','Модифицированный','мин','4.0','км/ч','4.1','%','4.1'),(3,'ПРОТ10',0.2,'Bruce','','Облегченный','мин','4.0','км/ч','4.1','%','4.1'),(4,'ПРОТ10',1,'Naughton','','Классический','мин','4.0','км/ч','4.1','%','4.1'),(5,'ПРОТ10',1.1,'Naughton','','Модифицированный','мин','4.0','км/ч','4.1','%','4.1'),(6,'ПРОТ10',2,'Balk','','Balk-Ware','мин','4.0','км/ч','4.1','%','4.1'),(7,'ПРОТ10',2.1,'Balk','','Substandard Balk','мин','4.0','км/ч','4.1','%','4.1'),(8,'ПРОТ10',2.2,'Balk','','Standard Balk','мин','4.0','км/ч','4.1','%','4.1'),(9,'ПРОТ10',2.3,'Balk','','Superstandard Balk','мин','4.0','км/ч','4.1','%','4.1'),(10,'ПРОТ10',3,'Ellesteadxx','','','мин','4.0','км/ч','4.1','%','4.1'),(11,'ПРОТ10',4,'Cornell','','','мин','4.0','км/ч','4.1','%','4.1'),(12,'ПРОТ10',5,'Kattus','','','мин','4.0','км/ч','4.1','%','4.1'),(13,'ПРОТ10',6,'STEEP','','','мин','4.0','км/ч','4.1','%','4.1'),(14,'ПРОТ10',7,'Gardner','','','мин','4.0','км/ч','4.1','%','4.1');
/*!40000 ALTER TABLE `Протоколы` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2020-11-01 21:03:59
