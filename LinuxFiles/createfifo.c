#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILE1 "/home/timeserverD/cr751/node2c.fifo"

void createfifo(char *FIFO_FILE);

int main()
{
    createfifo(FILE1);
    return 0;
}

void createfifo(char *FIFO_FILE)
{
    /* Create the FIFO if it does not exist */
    mkfifo(FIFO_FILE, 0666);
}