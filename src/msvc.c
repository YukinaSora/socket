#ifdef _MSC_VER

#include "msvc.h"

#include "client.h"

extern __class_client_shared_data_t __class_client_shared_data;

char* __msvc_ctime(const time64_t* const timep)
{
	static char* buffer[26];
	ctime_s((char*)buffer, 26, timep);
	return (char*)buffer;
}

int __msvc_socket_connect(socket_t sockfd, sockaddr* addr, int addrlen)
{
	return connect(sockfd, addr, addrlen) == SOCKET_ERROR;
}

int __msvc_socket_bind(socket_t sockfd, sockaddr* addr, int addrlen)
{
	return bind(sockfd, addr, addrlen) == SOCKET_ERROR;
}

int __msvc_socket_init()
{
	WSADATA wsaData = { 0 };
	return WSAStartup(MAKEWORD(2, 2), &wsaData) != 0;
}

int __msvc_socket_create(socket_t * sockfd)
{
	// 初始化WinSock
	if (socket_init()) {
		socket_perror("Socket create failed");
		socket_exit(EXIT_FAILURE);
	}
	return (*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET;
}

int __msvc_socket_listen(socket_t sockfd, int backlog)
{
	return listen(sockfd, backlog) == SOCKET_ERROR;
}

int __msvc_socket_accept(socket_t * out_sockfd_ptr, socket_t sockfd, sockaddr* addr, int addrlen)
{
	*out_sockfd_ptr = accept(sockfd, addr, &addrlen);
	return *out_sockfd_ptr == INVALID_SOCKET;
}

void __msvc_socket_getpeername(socket_t sockfd, sockaddr* addr)
{
	socklen_t addrlen = (socklen_t)sizeof((sockaddr_in*)addr);
	getpeername(sockfd, addr, &addrlen);
}

const char* __msvc_socket_get_local_ip()
{
	static char hostname[256] = { 0 };

	gethostname(hostname, sizeof(hostname));

	struct hostent* host = gethostbyname(hostname);
	if (host == nullptr) {
		socket_perror("gethostbyname failed");
		return nullptr;
	}

	const char* ip = socket_inet_ntop(AF_INET, host->h_addr_list[0], hostname, INET_ADDRSTRLEN);

	return ip;
}

int __msvc_socket_send(socket_t sockfd, const byte* buffer, int len)
{
	return send(sockfd, buffer, len, 0) == SOCKET_ERROR;
}

void __msvc_socket_perror(const char* title)
{
	printf("%s: %d\n", title, WSAGetLastError());
}

void __msvc_socket_exit(int code)
{
	WSACleanup();
	exit(code);
}

BOOL WINAPI __msvc_socket_handle_console_stop(DWORD signal)
{
	if (signal != CTRL_C_EVENT) return true;

	printf("Socket client stop.\n");
	socket_close(__class_client_shared_data.client->m_sockfd_client);
	socket_exit(EXIT_SUCCESS);
	return false;
}

void __msvc_socket_bind_console_signal()
{
	if (!SetConsoleCtrlHandler(__msvc_socket_handle_console_stop, TRUE)) {
		socket_perror("SetConsoleCtrlHandler failed");
		socket_exit(EXIT_FAILURE);
	}
}

socket_thread_handle_t __msvc_socket_thread_create(socket_thread_callback_t func, void* param)
{
	// 用于接收线程ID, 但是我们不需要
	DWORD _;
	return CreateThread(NULL, 0, func, param, 0, &_);
}

void __msvc_socket_start_browser(const char* url)
{
	char cmd[max_url_size] = { 0 };
	socket_sprintf(cmd, "start %s", url);
	system(cmd);
}

#endif