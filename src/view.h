#ifndef VIEW_H
#define VIEW_H

#include "utils.h"

class view
{
public
	void (*$view)(struct view* this);
	void (*$delete)(struct view* this);

	void (*init)(struct view* this);
	void (*exec)(struct view* this);
	void (*set_parent)(struct view* this, client* parent);

	const char* (*static_server_url)();

private
	// no this in static functions
	MHD_Result(*select_data)(struct view* this, MHD_Connection* connection);
	MHD_Result(*send_file_response)(
		struct view* this,
		MHD_Connection* connection, 
		const char* content_type, 
		const char* file_path);
	MHD_AccessHandlerCallback answer_to_connection;

private
	client* m_parent;

    MHD_Daemon* m_daemon;
};

reg_class(view);

#endif // !VIEW_H