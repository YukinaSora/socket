#ifndef UTILS_H
#define UTILS_H

#include "platform.h"

#define make_var_string(size) (var_string*)try_malloc(sizeof(var_string) + size + 1)

#define contains(options, value) (((int)options & (int)value) == (int)value)

#define reg_class \
	__reg_class

#define new \
	__new

#define construct_for \
	__construct_for

// 创建类实例
#define instance(type) ({                           \
    type instance;                                  \
	construct_for(type, &instance);                 \
    instance;									    \
})


// 删除类指针指向的对象
// 将调用类的析构函数
#define delete(this) do{this->$delete(this); free(this); this = nullptr;}while(0)


// 删除类实例
// 将调用类的析构函数
#define analysis(instance) do{instance.$delete(&instance);} while(0)


// 获取类的成员函数名
#define func_in_class(type, name) __class_##type##_##name


// 为类的成员函数绑定成员函数指针
#define bind_func_for(type, ptr, name) do{ptr->name = func_in_class(type, name);} while(0)
#define bind_func_for_this(type, name) bind_func_for(type, this, name)


// 初始化类的成员指针变量
#define init_base_ptr_for(type, ptr, name) do{ptr->name = (type*)malloc(sizeof(type));} while(0)
#define init_base_ptr_for_this(type, name) init_base_ptr_for(type, this, name)


#define socket_str_is(str, substr)	\
	(strcmp(str, substr) == 0)


#define exit_with_error(message) do{socket_perror(message); socket_exit(EXIT_FAILURE);}while(0)


#define try(expression) if( (expression) )
#define when_failed
#define when_succeed else


#define socket_is_avalid(sockfd) (sockfd != INVALID_SOCKET)
#define socket_is_invalid(sockfd) (sockfd == INVALID_SOCKET)


#define socket_package_to_bytes(package, bytes)			\
	memcpy(bytes, &package, package.size);


#define socket_bytes_to_package(bytes, package)			\
	memcpy(&package, bytes, package.size);


#define socket_try_create_socket(...)					\
	try(socket_create(__VA_ARGS__))						\
	when_failed {										\
		exit_with_error("Socket create failed");		\
	}


#define socket_try_inet_pton(...)						\
	try(socket_inet_pton(__VA_ARGS__))					\
	when_failed {										\
		this->failed(this, "Inet_pton failed");			\
	}


#define socket_try_connect(...)							\
	try(socket_connect(__VA_ARGS__))					\
	when_failed {										\
        this->failed(this, "Connect failed");			\
	}


#define socket_try_bind(...)							\
	try(socket_bind(__VA_ARGS__))						\
	when_failed {										\
        this->failed(this, "Bind failed");				\
	}


#define socket_try_listen(...)							\
	try(socket_listen(__VA_ARGS__))						\
	when_failed {										\
		this->failed(this, "Listen failed");			\
	}


#define socket_try_send(...)							\
	try(socket_send(__VA_ARGS__))						\
	when_failed {										\
		this->failed(this, "Send failed");				\
	}


#define socket_try_open_sqlite(...)						\
	try(sqlite3_open(__VA_ARGS__))						\
	when_failed {										\
		socket_perror("Error opening sqlite database");	\
		exit(EXIT_FAILURE);								\
	}


#define socket_try_sqlite_prepare_v3(...)				\
	try(sqlite3_prepare_v3(__VA_ARGS__))				\
	when_failed {										\
        printf("sqlite3_prepare_v3 failed: %s\n",		\
			sqlite3_errmsg(this->m_parent->m_sqlite));	\
		return MHD_NO;									\
	}


#define socket_try_sqlite_bind_text(stmt, ...)			\
	try(sqlite3_bind_text(stmt, __VA_ARGS__))			\
	when_failed {										\
		printf("sqlite3_bind_text failed: %s\n",		\
			sqlite3_errmsg(this->m_parent->m_sqlite));	\
		sqlite3_finalize(stmt);							\
		return MHD_NO;									\
	}



#define socket_random(min, max) \
	(min + rand() % (max - min + 1))


#define socket_rangify(min, max, value) \
 	(value < min ? min : (value > max ? max : value))


#define socket_print_line() do{printf("--------------------------------------------------\n");}while(0)


// 程序信息
#define info() do{printf("Socket client version %s, compiled by %s.\n", __version, compiler);}while(0)


void* try_malloc(unsigned long long size);


int str_to_int(const char* str);

#endif // !UTILS_H