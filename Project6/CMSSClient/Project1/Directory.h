#pragma once
#ifndef DIRECTORY_H
#define DIRECTORY_H
#include<fstream>
#include<vector>
#include "FileInfomation.h"
using namespace std;
enum FileStatus
{
FILE_DELETE = 1,// file xóa
FILE_EDIT = 2,// file chỉnh sửa
FILE_ADD = 3,// file thêm mới
FILE_RENAME = 4// đổi tên file
};
namespace SocketSendData {
	bool isConnect = false;
	int sendData(SOCKET s, const char* buff, int len);
}
class Directory
{
private: 
	char directoryPath[1024]{0};// đường dẫn của thư mục
	/// <summary>
	/// Kiểm tra xem có file nào bị xóa, thêm không
	/// </summary>
	/// <param name="delList">danh sách các file chưa cập nhật</param>
	/// <param name="addList">danh sách các file đã cập nhật</param>
	void checkFile(vector<FileInfomation> delList, vector<FileInfomation> addList);
public:
	vector<FileInfomation> listFile;// danh sách các file có trong thư mục
	vector<pair<FileInfomation, FileStatus>> listFileChange;// danh sách các kết quả giám sát thu được
	Directory(const char* path);
	Directory();
	char* getPath();
	/// <summary>
	/// cập nhật danh sách các file
	/// </summary>
	vector<FileInfomation> settingFile();
	/// <summary>
	/// Kiểm tra các file hiện tại có bị sửa hay không
	/// </summary>
	void traceFile();
	/// <summary>
	/// Kiểm tra các file trong thư mục có bị xóa, thêm, đổi tên không
	/// </summary>
	void traceDirectory();
	/// <summary>
	/// ghi nhận thay đổi
	/// </summary>
	/// <param name="f">file có sự thay đổi</param>
	/// <param name="">trạng thái thay đổi</param>
	void logFile(FileInfomation f, FileStatus);

	void delFile(FileInfomation fileDeleted);

	void addFile(FileInfomation fileAdded);

	void clear();
};

#endif // !DIRECTORY_H



