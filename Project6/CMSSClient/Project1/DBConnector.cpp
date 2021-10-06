#include "DBConnector.h"

DBConnector::DBConnector(char* TableName)
{
	strcat(tableName, TableName);
	sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
	connection = driver->connect(server, user, pass);
	char sqlQuerry[1024]{ 0 };
	sprintf(sqlQuerry, "CREATE TABLE `cmss`.`%s` (`fileName` VARCHAR(45) NOT NULL,`ModifiedDate` DATETIME NOT NULL,`Status` INT NULL,PRIMARY KEY(`fileName`)); ", tableName);
	sql::Statement* stat = connection->createStatement();
	bool query = stat->execute(sqlQuerry);
}

void DBConnector::AddFile(FileInfomation fileAdd)
{
	char sqlQuerry[1024]{ 0 };
	sprintf(sqlQuerry, "insert into %s(fileName, ModifiedDate, Status) values ('%s',,1)", tableName);
	sql::Statement* stat = connection->createStatement();
	bool query = stat->execute(sqlQuerry);
}

void DBConnector::Delete(FileInfomation fileDel)
{
}

void DBConnector::Rename(FileInfomation oldFile, FileInfomation newFile)
{
}

void DBConnector::Edit(FileInfomation fildEdit)
{
}
