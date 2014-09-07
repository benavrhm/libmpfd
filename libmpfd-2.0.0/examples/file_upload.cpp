/**
	Sample file uploader for Mongoose, derived from examples/big_upload.c.
	g++ -o file_upload file_upload.cpp mongoose.o -lmpfd -lpthread
	./file_upload
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libmpfd/Parser.h>
#include "mongoose.h"

const int ReadBufferSize = 4 * 1024;
char input[ReadBufferSize];
using namespace MPFD;

Parser *POSTParser;
int last = 0;

static int handle_request(struct mg_connection *conn)
{
	if (strcmp(conn->uri, "/upload") == 0)
	{
		if (last)
		{
			// Get the last chunk if any
			try {
				POSTParser->AcceptSomeData(conn->content, conn->content_len);
			} catch (MPFD::Exception e) {
				printf(__FILE__ " %d: %s\n", __LINE__, e.GetError().c_str());
			}

			// Tell the user
// ToDo: Use headers, close the connection cleanly

			mg_printf_data(conn, "%s", "Bingo!\n\n");

			// Rename the temporary file
    			std::map<std::string,MPFD::Field *> fields = POSTParser->GetFieldsMap();
			std::string filename = fields["file"]->GetFileName();

			char oldname[128], newname[128];
			strcpy(oldname, fields["file"]->GetTempFileName().c_str());

			strcpy(newname, POSTParser->GetTempDirForFileUpload().c_str());
			strcat(newname, "/");
			strcat(newname, fields["file"]->GetFileName().c_str());

			rename(oldname, newname);

			// Clean up, rest and return
			delete POSTParser;
			last = 0;
			return MG_TRUE; // Tell Mongoose we're done with this request
		}
		else
		{
			mg_printf_data(conn, "%s", "Had no data to write...");
			return MG_TRUE; // Tell Mongoose we're done with this request
		}
	}
	else
	{
		mg_printf_data(conn, "%s",
		               "<html><body>Upload example."
		               "<form method=\"POST\" action=\"/upload\" "
		               "  enctype=\"multipart/form-data\">"
		               "<input type=\"file\" name=\"file\" /> <br/>"
		               "<input type=\"submit\" value=\"Upload\" />"
		               "</form></body></html>");
		return MG_TRUE;   // Tell mongoose to close this connection
	}
}


// Mongoose sends MG_RECV for every received POST chunk.
// When last POST chunk is received, Mongoose sends MG_REQUEST, then MG_CLOSE.
static int handle_recv(struct mg_connection *conn)
{
	if (0 == last) {
		last = 1;
		try {
			POSTParser = new MPFD::Parser(
				mg_get_header(conn, "Content-Type"));
		} catch (MPFD::Exception e) {
			printf(__FILE__ " %d: %s\n", __LINE__, e.GetError().c_str());
			return false;
		}
	}

	try {
		POSTParser->AcceptSomeData(conn->content, conn->content_len);
	} catch (MPFD::Exception e) {
		printf(__FILE__ " %d: %s\n", __LINE__, e.GetError().c_str());
	}
	return conn->content_len;
}


static int handle_close(struct mg_connection *conn)
{
	printf("handle_close\n");
	return MG_TRUE;
}


static int ev_handler(struct mg_connection *conn, enum mg_event ev)
{
	switch (ev)
	{
		case MG_AUTH:     return MG_TRUE;
		case MG_REQUEST:  return handle_request(conn);
		case MG_RECV:     return handle_recv(conn);
		case MG_CLOSE:    return handle_close(conn);
		default:          return MG_FALSE;
	}
}

int main(void)
{
	struct mg_server *server = mg_create_server(NULL, ev_handler);
	mg_set_option(server, "listening_port", "8080");
	printf("Starting on port %s\n", mg_get_option(server, "listening_port"));

	for (;;)
	{
		mg_poll_server(server, 1000);
	}

	mg_destroy_server(&server);
	return 0;
}
