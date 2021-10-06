#pragma once
#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H
#include <iostream>
#include "mysql_driver.h"
#include "mysql_connection.h"
#include "cppconn/driver.h"
#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/metadata.h"
#include "cppconn/exception.h"
#include"FileInfomation.h"
#define server "tcp://127.0.0.1:3306/cmss"
#define user "root"
#define pass "0matkhau"
class DBConnector
{
private:
	char tableName[1024]{ 0 };
	sql::Connection* connection;
public:
	DBConnector(char* tableName);
	void AddFile(FileInfomation fileAdd);
	void Delete(char* fileDel);
	void Rename(char* oldFile, char* newFile);
	void Edit(FileInfomation fildEdit);
	void ResetDb();
};
#endif // !DBCONNECTOR_H



