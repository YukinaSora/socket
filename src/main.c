#include "client.h"

const char* path;

void Usage()
{
	printf("Usage: %s <mode> <server_ip> <id>\n"
		   "	mode: --server | -S | --client | -C\n"
		   "	server_ip: <ip>\n"
		   "	id: <int>\n", path);
}

static int callback(void* NotUsed, int argc, char** argv, char** azColName)
{
	int i;
	for (i = 0; i < argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int main(int argc, char* argv[], char** env)
{
	if (argc < 2)
	{
		Usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	path = argv[0];

	info();

	config _config = { 
		.server_port = 30080,
		.server_ip = view_server_localhost,
		.id = 0
	};

	for (int i = 0; i < argc; i++)
	{
		const char* arg = argv[i];

		if (socket_str_is(arg, "--server") || socket_str_is(arg, "-S"))
		{
			_config.mode = SERVER;
		}
		else if (socket_str_is(arg, "--client") || socket_str_is(arg, "-C"))
		{
			_config.mode = CLIENT;
		}
		else if (socket_str_is(arg, "--ip") || socket_str_is(arg, "-I"))
		{
			if (++i == argc) { Usage(); return 1; }
			_config.server_ip = argv[i];
		}
		else if (socket_str_is(arg, "--port") || socket_str_is(arg, "-P"))
		{
			if (++i == argc) { Usage(); return 1; }
			_config.server_port = str_to_int(argv[i]);
		}
		else if (socket_str_is(arg, "--id") || socket_str_is(arg, "-ID"))
		{
			if (++i == argc) { Usage(); return 1; }
			_config.id = str_to_int(argv[i]);
		}
	}

	client* c = new(client);
	c->set_config(c, _config);
	c->exec(c);
	
	delete(c);

	return 0;
}