#include "datasource.h"


DataSource *create_data_source(char *path) {
	
}

void release_data_source(DataSource *dataSource) {
	if( !dataSource )
		return;
	free(dataSource);
}

void release_frame(Frame *frame) {
	if( !frame )
		return;
	free(frame->data);
	free(frame);
}
