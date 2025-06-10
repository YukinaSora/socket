#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "utils.h"
#include "view.h"

typedef enum
{
	CLIENT = 0x01,
	SERVER = 0x02,
	ALL	   = CLIENT | SERVER
} mode;

typedef struct config
{
	int mode;
	const char* server_ip;
	uint server_port;
	uint id;
} config;

typedef struct package
{
	size_t size;
	uint id;
	uint temperature;
	uint humidity;
	time64_t time;
} package_t;

struct var_string
{
	size_t size;
	char data[];
};

struct __class_client_shared_data_t {
	volatile client* client;
};

struct __class_client_on_recieved_args_t {
	client* this;
	socket_t sockfd;
};

class client
{
public
	void (*$client)(struct client* this);
	void (*$delete)(struct client* this);

	void (*init)(struct client* this);
	void (*init_client)(struct client* this);
	void (*init_server)(struct client* this);
	void (*init_sqlite)(struct client* this);
	void (*check_sqlite_exist)(struct client* this);
	void (*set_config)(struct client* this, config c);
	void (*exec)(struct client* this);

	void (*send)(struct client* this, const package_t package);
	void (*make_reports)(struct client* this);

	void (*listen)(struct client* this);
	socket_thread_callback_t on_recieved;
	void (*make_view)(struct client* this);

	void (*failed)(struct client* this, const char* msg);
	void (*clear_sockfd)(struct client* this);

private
	config m_config;
	uint m_id;
	socket_t m_sockfd_client;
	socket_t m_sockfd_server;
	sqlite3* m_sqlite;
	int m_temperature;
	int m_humidity;

	view* m_view;
};

reg_class(client);

#endif // !CONNECTOR_H