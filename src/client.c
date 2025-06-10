#include "client.h"

__class_client_shared_data_t __class_client_shared_data;

static void __class_client_$client(client* this)
{
	memset(&this->m_config, 0, sizeof(this->m_config));

	this->m_id = 0;
	this->m_temperature = 0;
	this->m_sockfd_client = 0;
	this->m_sockfd_server = 0;
	this->m_sqlite = nullptr;
	this->m_view = nullptr;
}

static void __class_client_$delete(client* this)
{
	this->clear_sockfd(this);

	sqlite3_free(this->m_sqlite);

	delete(this->m_view);
}

static void __class_client_init(client* this)
{
	__class_client_shared_data.client = this;

	if (contains(this->m_config.mode, (int)CLIENT))
	{
		this->init_client(this);
	}
	if (contains(this->m_config.mode, (int)SERVER))
	{
		this->init_server(this);
	}
	this->init_sqlite(this);
}

static void __class_client_init_client(client* this)
{
	printf("Initializing client.\n");

	socket_t sockfd = 0;
	socket_try_create_socket(sockfd);

	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(this->m_config.server_port);
	socket_try_inet_pton(addr.sin_family, this->m_config.server_ip, &addr.sin_addr);

	printf("Connecting to %s:%d.\n", this->m_config.server_ip, this->m_config.server_port);
	socket_try_connect(sockfd, addr);

	this->m_sockfd_client = sockfd;

	printf("Connection testablished successfully.\n");
}

static void __class_client_init_server(client* this)
{
	printf("Initializing server.\n");

	// 创建Socket
	socket_t sockfd = 0;
	socket_try_create_socket(sockfd);

	// 绑定Socket
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = socket_htons(this->m_config.server_port);
	addr.sin_addr.s_addr = socket_htonl(INADDR_ANY);
	socket_try_bind(sockfd, addr);

	this->m_sockfd_server = sockfd;

	this->m_view = new(view);
	this->m_view->set_parent(this->m_view, this);

	//printf("Server listening on %s:%d.\n", this->m_config.server_ip, this->m_config.server_port);
}

static void __class_client_init_sqlite(client* this)
{
	printf("Initializing SQLite.\n");

	this->check_sqlite_exist(this);

	socket_try_open_sqlite(socket_sqlite_path, &this->m_sqlite);
	printf("SQLite opened successfully.\n");

	int ret = sqlite3_exec(this->m_sqlite, 
		"CREATE TABLE IF NOT EXISTS data ("
			"no INTEGER PRIMARY KEY AUTOINCREMENT,"
			"id INT, "
			"temperature INT, "
			"humidity INT, "
			"time INT"
		");", 
		NULL, NULL, NULL);
	if (ret != SQLITE_OK)
	{
		printf("Create table failed.\n");
	}
}

static void __class_client_check_sqlite_exist(client* this)
{
	struct stat info;

	// 检查目录是否存在
	if (stat(socket_sqlite_dir_path, &info) != 0) {
		// 目录不存在，创建目录
		if (socket_mkdir(socket_sqlite_dir_path) != 0) {
			printf("Error creating directory.\n");
			return;
		}
		printf("Directory created: %s\n", socket_sqlite_dir_path);
	}
	else if (!(info.st_mode & S_IFDIR)) {
		// 路径存在但不是目录
		fprintf(stderr, "Path exists but is not a directory: %s\n", socket_sqlite_dir_path);
		return;
	}

	// 检查文件是否存在
	if (stat(socket_sqlite_path, &info) != 0) {
		// 文件不存在，创建文件
		FILE* file = fopen(socket_sqlite_path, "w");
		if (file == NULL) {
			printf("Error creating sqlite database.\n");
			return;
		}
		fclose(file);
		printf("Sqlite database created: %s\n", socket_sqlite_path);
	}
}

static void __class_client_set_config(client* this, config c)
{
	this->m_config = c;
	this->m_id     = c.id;
	this->m_sockfd_client = 0;
	this->m_sockfd_server = 0;
	this->m_temperature = 0;

	this->init(this);
}

static void __class_client_exec(client* this)
{
	socket_bind_console_signal();
	if (contains(this->m_config.mode, (int)CLIENT))
	{
		this->make_reports(this);
	}
	if (contains(this->m_config.mode, (int)SERVER))
	{
		this->listen(this);
	}
}

static void __class_client_send(client* this, const package_t package)
{
	int failed = socket_send(this->m_sockfd_client, (const byte*)&package, package.size);

	if (failed)
	{
		socket_perror("Send failed");
		exit(EXIT_FAILURE);
	}
	else {
		socket_print_line();
		printf("Sent package:\n\tid: %d\n\ttemperature: %d\n\thumidity: %d\n\ttime: %s", package.id, package.temperature, package.humidity, socket_ctime((time_t*)&package.time));
		socket_print_line();
	}
}

static void __class_client_make_reports(client* this)
{
	printf("Recording...\n");

	srand((uint)time(NULL));
	int temperature = socket_random(-10, 40);
	int humidity    = socket_random(30, 100);
	time64_t now = time(NULL);

	while (true)
	{
		this->m_temperature = temperature;

		package_t package = {
			.size		 = sizeof(package_t),
			.id			 = this->m_id,
			.temperature = this->m_temperature,
			.humidity	 = humidity,
			.time		 = now
		};

		this->send(this, package);

		socket_sleep(1);

		temperature += socket_random(-2, 2);
		temperature =  socket_rangify(-10, 40, temperature);

		humidity += socket_random(-2, 2);
		humidity =  socket_rangify(-30, 100, humidity);

		now = time(NULL);
	}
}

static void __class_client_listen(client* this)
{
	typedef __class_client_on_recieved_args_t recieved_args_t;

	socket_try_listen(this->m_sockfd_server, max_listen_queue);
	printf("Server is now listening on %s:%d.\n", view_server_localhost, this->m_config.server_port);

	this->make_view(this);
	printf("View output on %s:%d.\n", view_server_localhost, view_server_port);

	socket_start_browser(this->m_view->static_server_url());
	
	// 监听客户端连接
	while (true)
	{
		socket_t client_sockfd;
		sockaddr_in addr = { 0 };
		
		if (socket_accept(&client_sockfd, this->m_sockfd_server, addr))
		{
			continue;
		}

		recieved_args_t* args = (recieved_args_t*)try_malloc(sizeof(recieved_args_t));
		args->this = this;
		args->sockfd = client_sockfd;

		socket_thread_create(this->on_recieved, args);
	}
}

static socket_thread_callback_ret_t __class_client_on_recieved(void* _args)
{
	typedef __class_client_on_recieved_args_t args_t;

	args_t   args   = *(args_t*)_args;
	client*	 this   = args.this;
	socket_t sockfd = args.sockfd;
	byte*	 buffer = (byte*)try_malloc(buffer_size);

	// 监听客户端消息
	while (true)
	{
		if (recv(sockfd, buffer, buffer_size, 0) == 0)
		{
			printf("Client disconnected.\n");
			socket_close(sockfd);
			goto ret;
		}

		package_t package = *(package_t*)buffer;
		//socket_print_line();
		//printf("Received package:\n\tid: %d\n\ttemperature: %d\n\thumidity: %d\n\ttime: %s\n", package.id, package.temperature, package.humidity, socket_ctime((time_t*)& package.time));
		//socket_print_line();

		// 插入数据到数据库
		char sql[max_sql_cmd_size] = { 0 };
		socket_sprintf(sql, "INSERT INTO data (id, temperature, humidity, time) VALUES (%d, %d, %d, %lld);", 
			package.id, package.temperature, package.humidity, package.time);
		int ret = sqlite3_exec(this->m_sqlite, sql, NULL, NULL, NULL);
		if (ret != SQLITE_OK)
		{
			socket_perror("Insert data failed");
		}
	}

ret:
	free(buffer);
	free(_args);

	return 0;
}

static void __class_client_make_view(client* this)
{
	this->m_view->exec(this->m_view);
}

static void __class_client_failed(client* this, const char* msg)
{
	socket_perror(msg);
	this->clear_sockfd(this);
}

static void __class_client_clear_sockfd(client* this)
{
	if (contains(this->m_config.mode, CLIENT))
		socket_close(this->m_sockfd_client);

	if (contains(this->m_config.mode, SERVER))
		socket_close(this->m_sockfd_server);

	socket_exit(EXIT_FAILURE);
}

void __class_client_$construct(client* this)
{
	this->$client			 = __class_client_$client;
	this->$delete			 = __class_client_$delete;
	this->init 				 = __class_client_init;
	this->init_client		 = __class_client_init_client;
	this->init_server		 = __class_client_init_server;
	this->init_sqlite		 = __class_client_init_sqlite;
	this->check_sqlite_exist = __class_client_check_sqlite_exist;
	this->set_config		 = __class_client_set_config;
	this->exec				 = __class_client_exec;
	this->send				 = __class_client_send;
	this->make_reports		 = __class_client_make_reports;
	this->listen			 = __class_client_listen;
	this->on_recieved		 = __class_client_on_recieved;
	this->make_view			 = __class_client_make_view;
	this->failed			 = __class_client_failed;
	this->clear_sockfd		 = __class_client_clear_sockfd;
}
