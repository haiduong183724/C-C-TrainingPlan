#include "DBConnector.h"

DBConnector::DBConnector()
{
	sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
	connection = driver->connect(server, user, pass);
}

bool DBConnector::AddClient(Client c)
{
	char* ip_addr = inet_ntoa(c.caddr.sin_addr);
	char sqlQuerry[1024]{ 0 };
	sprintf(sqlQuerry, "select * from clients where ip_addr = '%s'", ip_addr);
	sql::Statement* stat = connection->createStatement();
	sql::ResultSet* res;
	res = stat->executeQuery(sqlQuerry);
	// client đã kết nối tới
	if(res->rowsCount() > 0){
		memset(sqlQuerry, 0, sizeof(sqlQuerry));
		sprintf(sqlQuerry, "update clients set connectstate = 1, updatestate = 1 where ip_addr = '%s'", ip_addr);
		bool query = stat->execute(sqlQuerry);
		while (res->next()) {
			if (res->getInt("updatestate") == 0) {
				// phải đồng bộ
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		// thêm một client mới
		memset(sqlQuerry, 0, sizeof(sqlQuerry));
		sprintf(sqlQuerry, "insert into clients(ip_addr,connectstate, updatestate) values ('%s',1,1)", ip_addr);
		bool query = stat->execute(sqlQuerry);
		return true;
	}
}

void DBConnector::Disconnect(Client c, int updateState)
{
	char* ip_addr = inet_ntoa(c.caddr.sin_addr);
	char sqlQuerry[1024]{ 0 };
	sprintf(sqlQuerry, " = '%s'", ip_addr);
	sql::Statement* stat = connection->createStatement();
	sprintf(sqlQuerry, "update clients set connectstate = 0, updatestate = %d where ip_addr = '%s'",updateState, ip_addr);
	bool query = stat->execute(sqlQuerry);
}

void DBConnector::ChangeUpdatedState()
{
	sql::Statement* stat = connection->createStatement();
	char* sqlQuerry = (char*) "update clients set updatestate = 0 where connectstate = 0";
	bool query = stat->execute(sqlQuerry);
}
