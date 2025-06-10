#ifdef _MSC_VER

#ifndef PLATFORM_MSVC_H
#define PLATFORM_MSVC_H

#include "common.h"

#define __compiler "MSVC"

#define __version "1.0.0"

#define _CRT_SECURE_NO_WARNINGS

typedef SOCKET socket_t;
typedef HANDLE socket_thread_handle_t;
typedef DWORD socket_thread_id_t;

#define socket_thread_callback_ret_t DWORD WINAPI
typedef PTHREAD_START_ROUTINE socket_thread_callback_t;

#define socket_sprintf(dest, format, ...)			\
	sprintf_s(dest, sizeof(dest), format, __VA_ARGS__)

#define socket_strcpy(dest, src)					\
	strcpy_s(dest, sizeof(src), src)

#define socket_strcat(dest, size, src)				\
	strcat_s(dest, size, src)

#define socket_mkdir								\
	_mkdir

#define socket_ctime								\
	__msvc_ctime
char* __msvc_ctime(const time64_t* const timep);

#define socket_sleep(time)							\
	Sleep(time * 1000)

#define socket_close								\
	closesocket

#define socket_inet_pton(...)						\
	inet_pton(__VA_ARGS__) != 1

#define socket_inet_ntop							\
	inet_ntop

#define socket_ntohs								\
	ntohs

#define socket_htons								\
	htons

#define socket_ntohl								\
	ntohl
	
#define socket_htonl								\
	htonl

#define socket_connect(sockfd, addr)				\
	__msvc_socket_connect(sockfd, (sockaddr*)&addr, sizeof(addr))
int __msvc_socket_connect(socket_t sockfd, sockaddr* addr, int addrlen);

#define socket_bind(sockfd, addr)					\
	__msvc_socket_bind(sockfd, (sockaddr*)&addr, sizeof(addr))
int __msvc_socket_bind(socket_t sockfd, sockaddr* addr, int addrlen);

#define socket_init									\
	__msvc_socket_init
int __msvc_socket_init();

#define socket_create(sockfd)						\
	__msvc_socket_create(&sockfd)
int __msvc_socket_create(socket_t * sockfd);

#define socket_listen								\
	__msvc_socket_listen
int __msvc_socket_listen(socket_t sockfd, int backlog);

#define socket_accept(out_sockfd_ptr, sockfd, addr) \
	__msvc_socket_accept(out_sockfd_ptr, sockfd, (sockaddr*)&addr, sizeof(addr))
int __msvc_socket_accept(socket_t * out_sockfd_ptr, socket_t sockfd, sockaddr* addr, int addrlen);

#define socket_getpeername(sockfd, addr)			\
	__msvc_socket_getpeername(sockfd, (sockaddr*)addr)
void __msvc_socket_getpeername(socket_t sockfd, sockaddr* addr);

#define socket_get_local_ip							\
	__msvc_socket_get_local_ip
const char* __msvc_socket_get_local_ip();

#define socket_send(sockfd, buffer, len)			\
	__msvc_socket_send(sockfd, buffer, (int)len)
int __msvc_socket_send(socket_t sockfd, const byte* buffer, int len);

#define socket_perror								\
	__msvc_socket_perror
void __msvc_socket_perror(const char* title);

#define socket_exit									\
	__msvc_socket_exit
void __msvc_socket_exit(int code);

#define socket_handle_console_stop					\
	__msvc_socket_handle_console_stop
BOOL WINAPI __msvc_socket_handle_console_stop(DWORD signal);

#define socket_bind_console_signal					\
	__msvc_socket_bind_console_signal
void __msvc_socket_bind_console_signal();

#define socket_thread_create						\
	__msvc_socket_thread_create
socket_thread_handle_t __msvc_socket_thread_create(socket_thread_callback_t func, void* args);

#define socket_start_browser                        \
	__msvc_socket_start_browser
void __msvc_socket_start_browser(const char* url);

#define __msvc_reg_class(type)						\
	typedef struct type type;						\
	void __class_##type##_$construct(type*);		\
	__make_class_reg_func(type)

#define __reg_class									\
	__msvc_reg_class

#define __msvc_make_class_reg_func(type)			\
	void __class_##type##_$construct(type* this);   \
	static inline type* __new_class_##type()		\
	{                                               \
		type* this = (type*)malloc(sizeof(type));   \
		if (this != nullptr) {                      \
			construct_for(type, this);              \
			this->$##type(this);                    \
		}                                           \
		return this;                                \
	}

#define __make_class_reg_func						\
	__msvc_make_class_reg_func

#define __msvc_new(type)							\
	__new_class_##type()

#define __new										\
	__msvc_new

#define __msvc_construct_for(type, this)			\
	__class_##type##_$construct(this)

#define __construct_for								\
	__msvc_construct_for


#endif // !PLATFORM_MSVC_H

#endif // !_MSC_VER