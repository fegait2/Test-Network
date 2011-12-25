#include <stdlib.h>

typedef enum {
	UNKNOWN,
	Audio,
	Video
} NMediaType;

typedef struct Frame {
	NMediaType type;
	int size;
	void *data;
} Frame;

typedef struct DataSource {
	void(*read)(Frame *frame);
} DataSource;

DataSource *create_data_source(char *path);
void release_data_source(DataSource *dataSource);
void release_frame(Frame *frame);
