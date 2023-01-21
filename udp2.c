/*
    Simple udp client
*/
#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEST_IP_ADDR "192.168.0.11"
#define BUFLEN 512 // Max length of buffer
#define PORT 8888  // The port on which to send data

int send2udp(char *message[BUFLEN]);
void die(char *s);

int main(void)
{
    while (1)
    {
        send2udp("bok");
    }

    return 0;
}

int send2udp(char *message[BUFLEN])
{
    struct sockaddr_in si_other;
    int udpSocket, i, socketLen = sizeof(si_other);
    char buffer[BUFLEN];

    if ((udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
        printf("\nSocket error.\n");
        return 1;
    }

    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);

    if (inet_aton(DEST_IP_ADDR, &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    // send the message
    if (sendto(udpSocket, message, strlen(message), 0, (struct sockaddr *)&si_other, socketLen) == -1)
    {
        die("sendto()");
    }

    // receive a reply and print it
    // clear the buffer by filling null, it might have previously received data
    memset(buffer, '\0', BUFLEN);
    // try to receive some data, this is a blocking call
    if (recvfrom(udpSocket, buffer, BUFLEN, 0, (struct sockaddr *)&si_other, &socketLen) == -1)
    {
        die("recvfrom()");
    }

    puts(buffer);
    close(udpSocket);
    return 0;
}

void die(char *s)
{
    perror(s);
    exit(1);
}