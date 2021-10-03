#include "Client.h"
#include <string.h>

Client::Client(SOCKET c, int Id, SOCKADDR_IN saddr)
{
    socket = c;
    id = Id;
    caddr = saddr;
    changeStatus((char*)"", FREE);
}

bool Client::checkAccount(char* userName, char* Password, int Id)
{
    /// Xác thực tài khoản ở đây
    if (strcmp(userName, "admin") == 0 && strcmp(Password, "123456") == 0) {
        return true;
    }
    return false;
}

int Client::getId()
{
    return id;
}

void Client::changeStatus(std::string fileName, int sendingPosition)
{
    // tên file đang gửi của client
    status.first = fileName;
    // vị trí đã gửi 
    status.second = sendingPosition;
}

std::string Client::getStatus()
{
    char logs[1024]{ 0 };
    sprintf(logs, "%s %s %d", inet_ntoa(caddr.sin_addr), status.first.c_str(), status.second);
    std::string s = logs;
    return s;
}
