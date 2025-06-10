#ifdef __GNUC__

#include "gcc.h"

#include "client.h"

extern __class_client_shared_data_t __class_client_shared_data;

int __gcc_socket_connect(socket_t sockfd, sockaddr* addr, int addrlen)
{
	return connect(sockfd, addr, addrlen) == SOCKET_ERROR;
}

int __gcc_socket_bind(socket_t sockfd, sockaddr* addr, int addrlen)
{
	return bind(sockfd, addr, addrlen) == SOCKET_ERROR;
}

int __gcc_socket_create(socket_t * sockfd)
{
	return (*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR;
}

int __gcc_socket_listen(socket_t sockfd, int backlog)
{
	return listen(sockfd, backlog) == SOCKET_ERROR;
}

int __gcc_socket_accept(socket_t * out_sockfd, socket_t sockfd, sockaddr* addr, int addrlen)
{
	uint _addrlen = (uint)addrlen;
	*out_sockfd = accept(sockfd, addr, &_addrlen);
	return *out_sockfd == SOCKET_ERROR;
}

void __gcc_socket_getpeername(socket_t sockfd, sockaddr* addr)
{
	socklen_t addrlen = (socklen_t)sizeof((sockaddr_in*)addr);
	getpeername(sockfd, addr, &addrlen);
}

const char* __gcc_socket_get_local_ip()
{
	static char hostname[256] = { 0 };
	if (gethostname(hostname, sizeof(hostname)) == FAIL)
	{
		socket_perror("Get host name failed");
		return nullptr;
	}

	hostent* hostentry = gethostbyname(hostname);
	if (hostentry == nullptr)
	{
		socket_perror("Get host by name failed");
		return nullptr;
	}

	const char* ip = socket_inet_ntop(AF_INET, hostentry->h_addr_list[0], hostname, sizeof(hostname));

	return ip;
}

int __gcc_socket_send(socket_t sockfd, const byte* buffer, int len)
{
	return send(sockfd, buffer, len, 0) == SOCKET_ERROR;
}

void __gcc_socket_handle_console_stop()
{
	printf("Socket client stop.\n");
	socket_t sockfd = __class_client_shared_data.client->m_sockfd_client;
	socket_close(sockfd);
	socket_exit(EXIT_SUCCESS);
}

void __gcc_socket_bind_console_signal()
{
	signal(SIGINT, socket_handle_console_stop);
}

socket_thread_handle_t __gcc_socket_thread_create(socket_thread_callback_t func, void* args)
{
	pthread_t thread;
	pthread_create(&thread, nullptr, func, args);
	pthread_detach(thread);
	return thread;
}

void __gcc_socket_start_browser(const char* url)
{
	static char cmd[max_url_size] = { 0 };
	socket_sprintf(cmd, "xdg-open %s", url);
	system(cmd);
}

#endif