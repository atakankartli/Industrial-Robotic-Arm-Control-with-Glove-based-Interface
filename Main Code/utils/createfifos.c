#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILE1 "/var/log/ptp4l.log"
#define FILE2 "/var/log/ts2phc.log"
#define FILE3 "/home/timeserver/c2web.fifo"
#define FILE4 "/home/timeserver/web2c.fifo"
#define FILE5 "/home/timeserver/eth.fifo"

void createfifo(char *FIFO_FILE);

int main()
{
    createfifo(FILE1);
    createfifo(FILE2);
    createfifo(FILE3);
    createfifo(FILE4);
    createfifo(FILE5);
    return 0;
}

void createfifo(char *FIFO_FILE)
{
    /* Create the FIFO if it does not exist */
    mkfifo(FIFO_FILE, 0666);
}