#include "Client.h"
#include <string.h>

Client::Client(SOCKET C, int Id)
{
    socket = C;
    id = Id;
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
