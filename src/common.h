#ifndef COMMON_H
#define COMMON_H

//#undef _MSC_VER
//#define __GNUC__ 1

#define compiler \
	__compiler

#define version \
    __version


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>

#include <sqlite3.h>
#include <microhttpd.h>

#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <direct.h>
#include <io.h>

#pragma comment(lib, "Ws2_32.lib")

#elif __GNUC__
#include <signal.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#endif

typedef struct tm tm;
typedef struct in_addr in_addr;
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;

typedef enum MHD_Result MHD_Result;
typedef struct MHD_Daemon MHD_Daemon;
typedef struct MHD_Connection MHD_Connection;
typedef struct MHD_Response MHD_Response;

typedef unsigned char byte;
typedef unsigned int uint;
typedef long unsigned int ulong;
typedef long long int time64_t;

typedef struct config config;
typedef struct client client;
typedef struct var_string var_string;
typedef struct package package_t;
typedef struct __class_client_shared_data_t __class_client_shared_data_t;
typedef struct __class_client_on_recieved_args_t __class_client_on_recieved_args_t;


// cpp style marcos
#define this self		// 避免IDE识别this为关键字导致报错
#define class struct
#define public
#define protected
#define private
#define nullptr NULL
#define true 1
#define false 0

#define buffer_size (1024)
#define max_msg_size (32768)
#define max_sql_cmd_size (256)
#define max_clients (64)
#define max_listen_queue (64)
#define max_url_size (256)

#define socket_sqlite_dir_path ("./view")
#define socket_sqlite_path	   ("./view/data.db")

#define view_server_localhost ("127.0.0.1")
#define view_server_port (30900)
#define view_server_root ("./view")
#define view_server_home ("/view.html")
#define view_server_default_content_type ("octet-stream")

#ifndef FAIL
#define FAIL (~0)
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

#endif // !COMMON_H