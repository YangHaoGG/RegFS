#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main()
{
    int i=0;
    int fd = open ("/tmp/reg/system",O_RDONLY);
    if (fd == -1)
        return -1;

    char buf[1024];
    memset(buf,0,1024);
    read(fd,buf,1024);
    for(;i<1024;i++)
        printf("%2x",buf[i]);

    close (fd);
}
