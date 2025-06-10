#define _CRT_SECURE_NO_WARNINGS

#include "view.h"

#include "client.h"


static void __class_view_$view(struct view* this)
{
	this->m_daemon = nullptr;
}

static void __class_view_$delete(struct view* this)
{
	MHD_stop_daemon(this->m_daemon);
}

static void __class_view_init(struct view* this)
{
}

static void __class_view_exec(struct view* this)
{
	this->m_daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, view_server_port, NULL, NULL,
		this->answer_to_connection, this, MHD_OPTION_END);
}

static void __class_view_set_parent(struct view* this, client* parent)
{
	this->m_parent = parent;
}

static const char* __class_view_static_server_url()
{
    static char url[max_url_size] = { 0 };
	socket_sprintf(url, "http://localhost:%d", view_server_port);
    return url;
}

static MHD_Result __class_view_select_data(view* this, MHD_Connection* connection)
{
    const char* limit = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "limit");
    if (!limit) limit = "20";

    const char* id = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "id");
    if (!id) id = "0";

    const char* sql_cmd = "SELECT * FROM data WHERE id = ? ORDER BY no DESC LIMIT ?;";
    sqlite3_stmt* stmt = nullptr;
    socket_try_sqlite_prepare_v3(this->m_parent->m_sqlite, sql_cmd, -1, 0, &stmt, nullptr);
    socket_try_sqlite_bind_text(stmt, 1, id, -1, SQLITE_STATIC);
    socket_try_sqlite_bind_text(stmt, 2, limit, -1, SQLITE_STATIC);

    char* data[buffer_size] = { 0 };
    size_t i = 0;
    size_t data_size = 0;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
        int no = sqlite3_column_int(stmt, 0);
        int id = sqlite3_column_int(stmt, 1);
        int temperature = sqlite3_column_int(stmt, 2);
        int humidity = sqlite3_column_int(stmt, 3);
        time64_t time = sqlite3_column_int(stmt, 4);

        char* buffer = (char*)try_malloc(buffer_size * sizeof(char));
        sprintf(buffer, "{\"id\":%d,\"temperature\":%d,\"humidity\":%d,\"time\":%lld}", id, temperature, humidity, time);
        data[i++] = buffer;
        data_size += strlen(buffer) + 1;
	}

    data_size += 12; // "{\"data\":[]}\0"
    char* response_data = (char*)try_malloc(data_size * sizeof(char));
    response_data[0] = '\0';
    socket_strcat(response_data, data_size, "{\"data\":[");
	for (size_t j = 0; j < i; j++) {
        socket_strcat(response_data, data_size, data[j]);
		if (j != i - 1) {
            socket_strcat(response_data, data_size, ",");
		}
	}
    socket_strcat(response_data, data_size, "]}");

	MHD_Response* response = MHD_create_response_from_buffer(strlen(response_data), response_data, MHD_RESPMEM_MUST_FREE);
	MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	MHD_add_response_header(response, "Content-Type", "application/json");
    MHD_queue_response(connection, MHD_HTTP_OK, response);

    // release memory
    MHD_destroy_response(response);
    for (size_t j = 0; j < i; j++) {
		free(data[j]);
	}
	sqlite3_finalize(stmt);

	return MHD_YES;
}

static MHD_Result __class_view_send_file_response(view* this, MHD_Connection* connection, const char* content_type, const char* file_path)
{
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        return MHD_NO;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* file_content = (char*)malloc(file_size * sizeof(char));
    if (!file_content) {
        fclose(file);
        return MHD_NO;
    }

    fread(file_content, 1, file_size, file);
    fclose(file);

    MHD_Response* response = MHD_create_response_from_buffer(file_size, file_content, MHD_RESPMEM_MUST_FREE);

    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(response, "Content-Type", content_type);

    MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

static MHD_Result __class_view_answer_to_connection(void* cls,
    struct MHD_Connection* connection,
    const char* url,
    const char* method,
    const char* _version,
    const char* upload_data,
    size_t* upload_data_size,
    void** req_cls)
{
    view* this = (view*)cls;

	if (!socket_str_is(method, MHD_HTTP_METHOD_GET)) {
		return MHD_NO;
	}

    //printf("Url: %s\n", url);
    if (socket_str_is(url, "/") || socket_str_is(url, "")) {
		url = view_server_home;
	}

    if (socket_str_is(url, "/db_data")) {
        return this->select_data(this, connection);
    }

    char file[256] = { 0 };
    socket_sprintf(file, "%s%s", view_server_root, url);

    //printf("file_path: %s\n", file);

    // ºó×ºÃû
    const char* file_type = strrchr(url, '.');
    const char* content_type = { 0 };
    if (!file_type)
    {
        content_type = view_server_default_content_type;
    }
    else
    {
        file_type++;
        if (socket_str_is(file_type, "html"))
		{
			content_type = "text/html";
		}
		else if (socket_str_is(file_type, "css"))
		{
			content_type = "text/css";
		}
		else if (socket_str_is(file_type, "js"))
		{
			content_type = "application/javascript";
		}
        else if (socket_str_is(file_type, "wasm"))
        {
            content_type = "application/wasm";
		}
    }

	return this->send_file_response(this, connection, content_type, file);
}

void __class_view_$construct(struct view* this)
{
	this->$view                = __class_view_$view;
	this->$delete              = __class_view_$delete;
	this->init                 = __class_view_init;
	this->exec                 = __class_view_exec;
    this->set_parent           = __class_view_set_parent;
    this->select_data          = __class_view_select_data;
    this->static_server_url    = __class_view_static_server_url;
    this->send_file_response   = __class_view_send_file_response;
    this->answer_to_connection = __class_view_answer_to_connection;
}