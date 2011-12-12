#include <stdio.h>
#include <fcntl.h>

int main() {
	printf("Content-Type: text/html\r\n\r\n");
	char *query = getenv("QUERY_STRING");
	printf("<html><body>");
	printf("%s", query);
	printf("</body></html>");
	//int fd = open("1.log", O_WRONLY);
	//write(fd, "123", 3);
	//close(fd);
	//while(1){ sleep(10); }
	return 0;
}
