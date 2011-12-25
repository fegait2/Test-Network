#include <string.h>
#include <stdlib.h>
#include "util.h"

typedef struct char_int_pairs {
	char *key;
	int value;
	int key_size;
} char_int_pairs;



#define METHOD_COUNT 7 
#define PROTOCOL_COUNT 2
char_int_pairs _method[METHOD_COUNT] = { {"GET", GET, 3},
	 						 {"POST", POST, 4},
							 {"OPTIONS", OPTIONS, 7},
							 {"CREATE", CREATE, 6},
							 {"DESCRIBE", DESCRIBE, 8},
							 {"SETUP", SETUP, 5},
							 {"PLAY", PLAY, 4} };
char_int_pairs _protocol[PROTOCOL_COUNT] = { {"HTTP", HTTP, 4},
											 {"RTSP", RTSP, 4} };

req_info *parse_http(char *buf_header) {
	if( !buf_header )
		return;
	char *line_header  = buf_header;
	char *line_tail = NULL;
	int line_size = 0;
	int buf_len = strlen(buf_header);
	int i  = 0;
	req_info *header = (req_info *)calloc(sizeof(char), sizeof(req_info));
	

	header->_req_line = (req_line *)calloc(sizeof(char), sizeof(req_line));
	//Get the size, if it is exist
	if( (line_header = strstr(line_header, "size:")) ) {
		line_tail = strchr(line_header, '\n');
		*line_tail = '\0';
		//header->size = atoi(line_header + 5);
		line_header = line_tail + 1;
	}

	if( !line_header )
		line_header = buf_header; 

	//Get the method, file, protocol, version
	if( (line_tail = strchr(line_header, '\n')) && line_tail < buf_header + buf_len ) {
		/*
		if( strncmp(line_header, "GET", 3) == 0 )
			header->_req_line->method = GET;
		else if(strncmp(line_header, "POST", 4) == 0)
			header->_req_line->method = POST;
		*/
		int j = 0;
		for(; j < METHOD_COUNT;j ++ ) {
			if( strncmp(line_header, _method[j].key, _method[j].key_size) == 0 ) {
				header->_req_line->method = _method[j].value;
				break;
			} 
		}

		char *tmp = NULL;
		if( (line_header = strchr(line_header, ' ')) ) {
			line_header ++;
			if( (tmp = strchr(line_header, ' ')) ) {
				//char *path = (char *) calloc(sizeof(char), tmp - line_header );
				char *query = strchr(line_header, '?');
				if( !query )
					query = tmp;
				header->_req_line->path = (char *) calloc(sizeof(char), query - line_header );
				memcpy(header->_req_line->path, line_header, query - line_header);
				if( query != tmp ) {
					header->_req_line->query = (char *) calloc(sizeof(char), tmp - query );
					memcpy(header->_req_line->query, query + 1, tmp - query);
				}
				line_header = tmp + 1;
			}
		}

		if( line_header < line_tail ) {
			*line_tail = '\0';
			for(j = 0; j < PROTOCOL_COUNT; j ++) {
				if( strncmp(line_header, _protocol[j].key, _protocol[j].key_size) == 0 ) {
					header->_req_line->protocol = _protocol[j].value;
					header->_req_line->version = atof(line_header + _protocol[j].key_size+1); //"HTTP/"
					break;
				}
			}
			/*
			if( strncmp(line_header, "HTTP", 4) == 0 ) {
				header->_req_line->protocol = HTTP;
				header->_req_line->version = atof(line_header + 5); //"HTTP/"
			}
			*/
		}

		line_header = line_tail + 1;

		header->_req_header = (req_header *) calloc(sizeof(char), sizeof(req_header));

		if( header->_req_line->protocol == RTSP ) {
			header->_req_header->extend_header = (req_extend_header *) calloc(sizeof(char), sizeof(req_rtsp_extend_header));
		}
		//printf("Header: %s\n", line_header);
		//Parse the request header
		while( (line_tail = strstr(line_header, "\r\n")) ) {
			*line_tail = '\0';
			if( strstr(line_header, "CSeq") ) {
				((req_rtsp_extend_header *)header->_req_header->extend_header)->c_seq = atoi(line_header + 5);
				//break;
			} else if( strstr(line_header, "Transport") ) {
				line_header += 10; //Has a ':'
				while(*line_header == ' ' ) {
					line_header ++;
				}
				((req_rtsp_extend_header *)header->_req_header->extend_header)->transport = (char *)calloc(sizeof(char), strlen(line_header));
				strcpy(((req_rtsp_extend_header *)header->_req_header->extend_header)->transport, line_header);	
			}
			if( line_tail + 2 < buf_header + buf_len )
				line_header = line_tail + 2;
			else 
				break;
		}
	}

	return header;
	
/*
	if( (line_tail = strchr(line, '\n')) < header + buf_size ) {
		
	}
*/
	//while( (line_tail = strchr(line, '\n')) < header + buf_size ) {
	//	
	//}
}
/*
char *

char *build_response(req_info *req) {
	char *response = 
	char *line = build_response_line(req->_req_line);
}
*/
void release_header(req_info *header) {
	if( !header )
		return;
	if( header->_req_line->path )
		free(header->_req_line->path);
	if( header->_req_line->query)
		free(header->_req_line->query);

	free(header);
}
