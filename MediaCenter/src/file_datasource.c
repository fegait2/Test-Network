#include "file_datasource.h"

static void read(Frame *frame);
DataSource *create_file_data_source(char *path) {
	DataSource *ds = (DataSource *) calloc(1, sizeof(DataSource));

	ds->read = read;
}

void read(Frame *frame) {
}
