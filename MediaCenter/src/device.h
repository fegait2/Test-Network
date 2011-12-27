#include <stdlib.h>
typedef struct NDevice {
	NReader *reader;
	NWriter *writer;
} NDevice;

typedef struct NReader {
	int(* read)(char *buf, size_t len);
} NReader;

typedef struct NWriter {
	int(* write)(char *buf, size_t len);
} NWriter;
