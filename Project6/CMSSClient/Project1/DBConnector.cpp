#include "DBConnector.h"

DBConnector::DBConnector(char* TableName)
{
	strcat(tableName, TableName);
	sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
	connection = driver->connect(server, user, pass);
	char sqlQuerry[1024]{ 0 };
	sprintf(sqlQuerry, "show tables like '%s'", tableName);
	sql::Statement* stat = connection->createStatement();
	sql::ResultSet* rs;
	rs = stat->executeQuery(sqlQuerry);
	if(rs->rowsCount() == 0){
		memset(sqlQuerry, 0, sizeof(sqlQuerry));
		sprintf(sqlQuerry, "CREATE TABLE %s (`fileName` VARCHAR(45) NOT NULL,`ModifiedDate` DATETIME NOT NULL,`State` INT NULL,PRIMARY KEY(`fileName`)); ", tableName);
		bool query = stat->execute(sqlQuerry);
	}
}

void DBConnector::AddFile(FileInfomation fileAdd)
{
	char sqlQuerry[1024]{ 0 };
	sprintf(sqlQuerry, "select * from %s where fileName = '%s'", tableName, fileAdd.getFileName());
	sql::Statement* stat = connection->createStatement();
	sql::ResultSet* rs;
	rs = stat->executeQuery(sqlQuerry);
	if (rs->rowsCount() > 0) {
		memset(sqlQuerry, 0, sizeof(sqlQuerry));
		sprintf(sqlQuerry, "delete from %s where fileName = '%s'",
			tableName, fileAdd.getFileName());
		bool query = stat->execute(sqlQuerry);
	}
	memset(sqlQuerry, 0, sizeof(sqlQuerry));
	sprintf(sqlQuerry, "insert into %s(fileName, ModifiedDate, State) values ('%s','%s',1)", 
		tableName, fileAdd.getFileName(),fileAdd.getModifiedDate().sqlDateTimeStr());
	bool query = stat->execute(sqlQuerry);
}

void DBConnector::Delete(char* fileDel)
{
	char sqlQuerry[1024]{ 0 };
	sprintf(sqlQuerry, "delete from %s where fileName = '%s'",
		tableName, fileDel);
	sql::Statement* stat = connection->createStatement();
	bool query = stat->execute(sqlQuerry);
}

void DBConnector::Rename(char* oldFile, char* newFile)
{
	char sqlQuerry[1024]{ 0 };
	sprintf(sqlQuerry, "update %s set fileName = '%s' where fileName = '%s'",
		tableName, newFile, oldFile);
	sql::Statement* stat = connection->createStatement();
	bool query = stat->execute(sqlQuerry);
}

void DBConnector::Edit(FileInfomation fileEdit)
{
	char sqlQuerry[1024]{ 0 };
	sprintf(sqlQuerry, "update %s set modifiedDate = '%s' where fileName = '%s'",
		tableName, fileEdit.getModifiedDate().sqlDateTimeStr(), fileEdit.getFileName());
	sql::Statement* stat = connection->createStatement();
	bool query = stat->execute(sqlQuerry);
}

void DBConnector::ResetDb()
{
	sql::Statement* stat = connection->createStatement();
	char sqlQuerry[1024]{ 0 };
	sprintf(sqlQuerry, "delete from %s ", tableName);
	bool query = stat->execute(sqlQuerry);
}

//int main() {
//	FileInfomation f("list_file.txt");
//	DBConnector connect((char*)"table0");
//	connect.AddFile(f);
//	FileInfomation f1("log.txt");
//}