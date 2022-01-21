#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "errno.h"
#include "winsock2.h"
#include "ws2tcpip.h"

typedef uint8_t  u8;
typedef uint32_t u32;


#define SERVER_PORT 18000
#define MAX_BUFFER_SIZE 4096

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr    sockaddr;

SOCKET Listen;
SOCKET Conn;

u8 DemoWebPage[] =
"HTTP/1.0 200 \r\n\r\n"
"<head></head>"
"<body>"
"<h1>Yeah Bitch!!!!</h>"
"<img "
"src=https://www.pinkvilla.com/imageresize"
"/Aaron%20Paul%20to%20reprise%20his%20role%2"
"0of%20Jesse%20Pinkman%20in%20the%20sequel%2"
"0of%20the%20show%20Breaking%20Bad%20%281%29.jpg"
"?width=752&format=webp&t=pvorg"
"></img>"
"</body>";

void PrintErrorExit(u8 *Format, ...)
{
    u8  *SysMsg;
    u32  LastError = WSAGetLastError();
    
    va_list List;
    va_start(List, Format);
    
    vfprintf(stderr, Format, List);
    
    va_end(List);
    
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                   FORMAT_MESSAGE_FROM_SYSTEM,
                   0, 
                   LastError,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (u8 *)&SysMsg, 
                   0, 
                   0);
    
    fprintf(stderr, "%s\n", SysMsg);
    
    LocalFree(SysMsg);
    
	shutdown(Listen, SD_BOTH); // Disallow sends and receives
	closesocket (Listen);
	shutdown(Conn, SD_BOTH); // Disallow sends and receives
	closesocket (Conn);
    WSACleanup();
    
    exit(1);
    
    return;
}

u8 *Bin2Hex(u8 *Input, size_t Length)
{
    u8 *Result;
    u8 *Hexits = "0123456789ABCDEF";
    
    if(Input == NULL || Length <= 0) return NULL;
    
    int ResultLength = (Length * 3) + 1;
    
    Result = malloc(ResultLength);
    
    for(u32 I = 0; I < Length; I++)
    {
        Result[(I * 3)]     = Hexits[Input[I] >> 0x04];
        Result[(I * 3) + 1] = Hexits[Input[I]  & 0x0F];
        Result[(I * 3) + 2] = ' ';
    }
    
    return Result;
}


int main(u32 ArgCount, u8 **Args)
{
    /// Get IP address to connect to via command line arg
    sockaddr_in ServerAddr;
    u32         BytesRead;
    u8 RecvBuffer[MAX_BUFFER_SIZE];
    u8 SendBuffer[MAX_BUFFER_SIZE];
    WSADATA     StartupData;
    u32         SocketVersion;
    
    SocketVersion = MAKEWORD(2, 2);
    
    if(WSAStartup(SocketVersion, &StartupData) != 0)
    {
        PrintErrorExit("fail to startup winsock");
    }
    
    if((Listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        PrintErrorExit("failed to create socket");
    }
    
    /// Open a socket
    memset(&ServerAddr, 0, sizeof(ServerAddr));
    ServerAddr.sin_family      = AF_INET;
    ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY); /// host to network(u32) (byte order checking)
    ServerAddr.sin_port        = htons(SERVER_PORT); /// host to network(u16) (byte order checking)
    
    // NOTE(MIGUEL): Listen instad of connecting
    if(bind(Listen, (sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0)
    {
        PrintErrorExit("bind error");
    }
    
    // NOTE(MIGUEL): Listen instad of connecting
    if(listen(Listen, 10) < 0)
    {
        PrintErrorExit("listen error");
    }
    
    for(;;)
    {
        sockaddr_in Address;
        socklen_t   AddressLength;
        
        fprintf(stdout, "waiting for a connection on port %d\n", SERVER_PORT);
        fflush(stdout);
        
        // NOTE(MIGUEL): This socket, Conn is used for interaction with the
        //               client. Unlike Listen which is just a socket used 
        //               to pick up incoming connections.
        Conn = accept(Listen, (sockaddr *)NULL, NULL);
        
        memset(RecvBuffer, 0, sizeof(RecvBuffer));
        
        while((BytesRead = recv(Conn, RecvBuffer, MAX_BUFFER_SIZE - 1, 0)) > 0)
        {
            fprintf(stdout, "\n%s\n\n%s\n", Bin2Hex(RecvBuffer, BytesRead), RecvBuffer);
            
            if(RecvBuffer[BytesRead - 1] == '\n')
            {
                break;
            }
            
            memset(RecvBuffer, 0, sizeof(RecvBuffer));
        }
        
        if(BytesRead < 0) PrintErrorExit("read error.");
        
        memset(SendBuffer, 0, sizeof(SendBuffer));
        snprintf(SendBuffer, sizeof(SendBuffer), "%s",
                 DemoWebPage);
        
        send(Conn, SendBuffer, strlen(SendBuffer), 0);
        
        shutdown(Conn, SD_BOTH); // Disallow sends and receives
        closesocket (Conn);
    }
    
    
	shutdown(Listen, SD_BOTH); // Disallow sends and receives
	closesocket (Listen);
	shutdown(Conn, SD_BOTH); // Disallow sends and receives
	closesocket (Conn);
    WSACleanup();
    
    return;
}