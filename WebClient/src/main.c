#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "errno.h"
#include "winsock2.h"
#include "ws2tcpip.h"

typedef uint8_t  u8;
typedef uint32_t u32;


#define SERVER_PORT 80
#define REQ_MAX_SIZE 4096
#define RES_MAX_SIZE 4096

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr    sockaddr;

SOCKET Socket;

void PrintErrorExit(u8 *Format, ...)
{
    u8  *SysMsg;
    u32  LastError = WSAGetLastError();
    
    va_list List;
    va_start(List, Format);
    
    vfprintf(stdout, Format, List);
    
    va_end(List);
    
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                   FORMAT_MESSAGE_FROM_SYSTEM,
                   0, 
                   LastError,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (u8 *)&SysMsg, 
                   0, 
                   0);
    
    fprintf(stdout, "%s\n", SysMsg);
    
    LocalFree(SysMsg);
    
	shutdown(Socket, SD_BOTH); // Disallow sends and receives
	closesocket (Socket);
    WSACleanup();
    
    exit(1);
    
    return;
}

int main(u32 ArgCount, u8 **Args)
{
    /// Get IP address to connect to via command line arg
    sockaddr_in ServerAddr;
    u32         ReadStatus;
    u8 Request [REQ_MAX_SIZE];
    u8 Response[RES_MAX_SIZE];
    WSADATA     StartupData;
    u32         SocketVersion;
    
    SocketVersion = MAKEWORD(2, 2);
    
    if(WSAStartup(SocketVersion, &StartupData) != 0)
    {
        PrintErrorExit("fail to startup winsock");
    }
    if(ArgCount != 2)
    {
        PrintErrorExit("usage: %s <server address>", Args[0]);
    }
    
    if((Socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        PrintErrorExit("failed to create socket");
    }
    
    /// Open a socket
    memset(&ServerAddr, 0, sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port   = htons(SERVER_PORT); /// host to network (byte order checking)
    
    // ip Address Text to Bin conversion
    if(InetPton(AF_INET, Args[1], &ServerAddr.sin_addr) <= 0)
    {
        PrintErrorExit("inet_pton error for %s", Args[1]);
    }
    
    if(connect(Socket, (sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0)
    {
        PrintErrorExit("failed to connect to server");
    }
    
    /// Create Message (HTTP request)
    // NOTE(MIGUEL): The '/' signifies that we the client want to retrieve
    //               the homepage. "HTTP/1.1" indicates the version of http
    //               that we communictating with. "r\n\r\n" indeicates the 
    //               end of the this http request.
    memset(Request, 0, sizeof(Request));
    sprintf(Request, "GET index.html HTTP/1.0\r\n\r\n");
    u32 RequestSize = strlen(Request);
    
    if(send(Socket, Request, RequestSize, 0) != RequestSize)
    {
        PrintErrorExit("write error");
    }
    
    memset(Response, 0, RES_MAX_SIZE);
    
    
    while((ReadStatus = recv(Socket, Response, RES_MAX_SIZE - 1, 0)) > 0)
    {
        printf("%s", Response);
    }
    
    
    if(ReadStatus < 0)
    {
        PrintErrorExit("read error");
    }
    
    
	shutdown(Socket, SD_BOTH); // Disallow sends and receives
	closesocket (Socket);
    WSACleanup();
    
    return;
}