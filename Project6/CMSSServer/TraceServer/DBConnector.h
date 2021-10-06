#pragma once
#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H
#include <iostream>
#include "mysql_driver.h"
#include "mysql_connection.h"
#include "cppconn/driver.h"
#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"
#include"Client.h"
#define server "tcp://127.0.0.1:3306/cmss"
#define user "root"
#define pass "0matkhau"
class DBConnector
{
private:
	sql::Connection* connection;
public:
	/// <summary>
	/// Hàm khởi tạo, khởi tạo một kết nối tới database để truy vấn dữ liệu
	/// </summary>
	DBConnector();
	/// <summary>
	/// Thêm một client mới vào csdl khi có client kết nối tới.
	/// </summary>
	/// <param name="c">Client mới kết nối tới</param>
	/// <returns> trạng thái có gửi yêu cầu đồng bộ với server hay không.</returns>
	bool AddClient(Client c);
	/// <summary>
	/// Xử lý khi có client ngắt kết nối với server
	/// </summary>
	/// <param name="c">client ngắt kết nối</param>
	/// <param name="updateState"> trạng thái update của server</param>
	void Disconnect(Client c, int updateState);
	/// <summary>
	/// Thay đổi trạng thái kế nối của các client đã ngắt kết nối
	/// </summary>
	void ChangeUpdatedState();
};
#endif // !DBCONNECTOR_H

