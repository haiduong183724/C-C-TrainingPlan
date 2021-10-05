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
#define server "tcp://127.0.0.1:3306/bttuan"
#define user "root"
#define pass "0matkhau"
class DBConnector
{
private:
	sql::Connection* connection;
public:
	DBConnector();


};
#endif // !DBCONNECTOR_H



