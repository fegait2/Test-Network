/*
typedef enum {
	GET,
	POST
} HTTP_METHOD;
*/

typedef enum {
	HTTP,
	RTSP
} PROTOCOL;
/*
typedef struct base_header {
	int size;
	HTTP_METHOD method;
	char *file;
	char *query;
	PROTOCOL protocol;
	float version;
	char *host;
	char *user_agent;
	char *accept;
	char *accept_language;
	char *accept_encoding;
	char *accept_charset;
	int  keep_alive;
	char *connection;
} base_header;
*/
//======================================================
typedef enum {
	GET,
	POST,
	OPTIONS,
	CREATE,
	DESCRIBE,
	SETUP,
	PLAY
} METHOD;
typedef struct req_line {
	METHOD method;
	char *path;
	char *query;
	PROTOCOL protocol;
	float version;
} req_line;

typedef struct gen_header gen_header;

typedef struct rtsp_req_extend_header {
} rtsp_req_extend_header;
typedef struct req_extend_header req_extend_header;

typedef struct req_rtsp_extend_header {
	int c_seq;
	char *transport;
} req_rtsp_extend_header;

typedef struct req_header {
	char *accept;
	char *accept_charset;
	char *accept_encoding;
	char *accept_language;
	char *host;
	char *user_agent;
	req_extend_header *extend_header;
} req_header;

typedef struct entity_header entity_header;

typedef struct req_info {
	req_line *_req_line;
	gen_header *_gen_header;
	req_header *_req_header;
	entity_header *_entity_header;
} req_info;
/*
req_info *parse_http(char *buf);
char *build_response(req_info *req);
*/
void release_header(req_info *header);
