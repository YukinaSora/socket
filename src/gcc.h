#ifdef __GNUC__

#ifndef PLATFORM_GCC_H
#define PLATFORM_GCC_H

#include "common.h"

#define __compiler "GCC"

#define __version "1.0.0"

typedef long long int socket_t;
typedef pthread_t socket_thread_handle_t;
typedef pthread_t socket_thread_id_t;

#define socket_thread_callback_ret_t void*
typedef socket_thread_callback_ret_t(*socket_thread_callback_t)(void*);

#define far
#define FAR far

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

#ifndef INVALID_SOCKET
#define INVALID_socket_t (socket_t )(~0)
#endif

#define socket_sprintf(...) \
	sprintf(__VA_ARGS__)

#define socket_strcpy(dest, src) \
	strcpy(dest, src)

#define socket_strcat(dest, size, src) \
	strcat(dest, src)

#define socket_mkdir(path) \
	mkdir(path, 0755)

#define socket_sleep \
	sleep

#define socket_ctime \
	ctime

#define socket_close \
	close

#define socket_inet_pton(...) \
	inet_pton(__VA_ARGS__) != 1

#define socket_inet_ntop \
	inet_ntop

#define socket_ntohs \
	ntohs

#define socket_htons \
	htons

#define socket_ntohl \
	ntohl

#define socket_htonl \
	htonl

#define socket_connect(sockfd, addr) \
	__gcc_socket_connect(sockfd, (sockaddr*)&addr, sizeof(addr))
int __gcc_socket_connect(socket_t sockfd, sockaddr* addr, int addrlen);

#define socket_bind(sockfd, addr) \
	__gcc_socket_bind(sockfd, (sockaddr*)&addr, sizeof(addr))
int __gcc_socket_bind(socket_t sockfd, sockaddr* addr, int addrlen);

#define socket_init(sockfd) \
	(0)

#define socket_create(sockfd) \
	__gcc_socket_create(&sockfd)
int __gcc_socket_create(socket_t * sockfd);

#define socket_listen \
	__gcc_socket_listen
int __gcc_socket_listen(socket_t sockfd, int backlog);

#define socket_accept(out_sockfd_ptr, sockfd, addr) \
	__gcc_socket_accept(out_sockfd_ptr, sockfd, (sockaddr*)&addr, sizeof(addr))
int __gcc_socket_accept(socket_t * out_sockfd, socket_t sockfd, sockaddr* addr, int addrlen);

#define socket_getpeername(sockfd, addr) \
	__gcc_socket_getpeername(sockfd, (sockaddr*)addr)
void __gcc_socket_getpeername(socket_t sockfd, sockaddr* addr);

#define socket_get_local_ip \
	__gcc_socket_get_local_ip
const char* __gcc_socket_get_local_ip();

#define socket_send(sockfd, buffer, len) \
	__gcc_socket_send(sockfd, buffer, (int)len)
int __gcc_socket_send(socket_t sockfd, const byte* buffer, int len);

#define socket_perror \
	perror

#define socket_exit \
	exit

#define socket_handle_console_stop \
	__gcc_socket_handle_console_stop
void __gcc_socket_handle_console_stop();

#define socket_bind_console_signal \
	__gcc_socket_bind_console_signal
void __gcc_socket_bind_console_signal();

#define socket_thread_create \
	__gcc_socket_thread_create
socket_thread_handle_t __gcc_socket_thread_create(socket_thread_callback_t callback, void* arg);

#define socket_start_browser \
	__gcc_socket_start_browser
void __gcc_socket_start_browser(const char* url);

#define __gcc_reg_class(type)						\
	typedef struct type type;						\
	void __class_##type##_$construct(type*)
	
#define __reg_class									\
    __gcc_reg_class

#define __gcc_new(type)								\
	({												\
		type * this = (type*)malloc(sizeof(type));  \
		if (this != nullptr) {						\
			construct_for(type, this);				\
			this->$##type(this);					\
		}                                           \
		this;                                       \
	})

#define __new										\
	__gcc_new

#define __gcc_construct_for(type, ptr)	({          \
    if (ptr != nullptr)                             \
		__class_##type##_$construct(ptr);           \
    ptr;											\
})

#define __construct_for								\
	__gcc_construct_for

#endif // !PLATFORM_GCC_H

#endif // !__GNUC__