/*
    Simple udp client
*/
#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEST_IP_ADDR "192.168.0.20"
//#define DEST_IP_ADDR "169.254.237.55"
#define MAXBUFLEN 512 // Max length of sendText
#define PORT 10000  // The port on which to send data
#define PIPEFILE "node2c.fifo"
#define VER_H7


void die(char *s);
char getData(void);

typedef struct
{
	float j1; // J1 axis angle (radian)
	float j2; // J2 axis angle (radian)
	float j3; // J3 axis angle (radian)
	float j4; // J4 axis angle (radian)
	float j5; // J5 axis angle (radian)
	float j6; // J6 axis angle (radian)
	float j7; // Additional axis 1 (J7 axis angle) (radian)
	float j8; // Additional axis 2 (J8 axis angle) (radian)
} JOINT;

typedef struct
{
	float x;  // X axis coordinate value (mm)
	float y;  // Y axis coordinate value (mm)
	float z;  // Z axis coordinate value (mm)
	float a;  // A axis coordinate value (radian)
	float b;  // B axis coordinate value (radian)
	float c;  // C axis coordinate value (radian)
	float l1; // Additional axis 1 (mm or radian)
	float l2; // Additional axis 2 (mm or radian)
} WORLD;


typedef struct
{
	WORLD w;
	unsigned intsflg1; // Structural flag 1
	unsigned intsflg2; // Structural flag 2
} POSE;


typedef struct
{
	float p1; // Motor 1 axis
	float p2; // Motor 2 axis
	float p3; // Motor 3 axis
	float p4; // Motor 4 axis
	float p5; // Motor 5 axis
	float p6; // Motor 6 axis
	float p7; // Additional axis 1 (Motor 7 axis)
	float p8; // Additional axis 2 (Motor 8 axis)
} PULSE;


typedef struct enet_rtcmd_str
{
	unsigned short Command;	 // Command
#define MXT_CMD_NULL 0		 // Real-time external command invalid
#define MXT_CMD_MOVE 1		 // Real-time external command valid
#define MXT_CMD_END 255		 // Real-time external command end
	unsigned short SendType; // Command data type designation
	unsigned short RecvType; // Monitor data type designation
//////////// Command or monitor data type ///
#define MXT_TYP_NULL 0 // No data
// For the command and monitor ////////////////////
#define MXT_TYP_POSE 1	// XYZ data
#define MXT_TYP_JOINT 2 // Joint data
#define MXT_TYP_PULSE 3 // pulse data
///////////// For position related monitor ///
#define MXT_TYP_FPOSE 4	   // XYZ data (after filter process)
#define MXT_TYP_FJOINT 5   // Joint data (after filter process)
#define MXT_TYP_FPULSE 6   // Pulse data (after filter process)
#define MXT_TYP_FB_POSE 7  // XYZ data (Encoder feedback value)
#define MXT_TYP_FB_JOINT 8 // Joint data (Encoder feedback value)
#define MXT_TYP_FB_PULSE 9 // Pulse data (Encoder feedback value)
// For current related monitors ////////////////////
#define MXT_TYP_CMDCUR 10 // Electric current command
#define MXT_TYP_FBKCUR 11 // Electric current feedback
	union rtdata
	{				  // Command data
		POSE pos;	  // XYZ type [mm/rad]
		JOINT jnt;	  // Joint type [rad]
		PULSE pls;	  // Pulse type [pls]
		float lng1[8]; // Integer type [% / non-unit]
	} dat;
	unsigned short SendIOType; // Send input/output signal data designation
	unsigned short RecvIOType; // Return input/output signal data designation
#define MXT_IO_NULL 0		   // No data
#define MXT_IO_OUT 1		   // Output signal
#define MXT_IO_IN 2			   // Input signal
	unsigned short BitTop;	   // Head bit No.
	unsigned short BitMask;	   // Transmission bit mask pattern designation (0x0001-0xffff)
	unsigned short IoData;	   // Input/output signal data (0x0000-0xffff)
	unsigned short TCount;	   // Timeout time counter value
	float CCount;	   // Transmission data counter value
	unsigned short RecvType1;  // Reply data-type specification 1
	union rtdata1
	{				  // Monitor data 1
		POSE pos1;	  // XYZ type [mm/rad]
		JOINT jnt1;	  // JOINT type [mm/rad]
		PULSE pls1;	  // PULSE type [mm/rad]
		float lng1[8]; // Integer type [% / non-unit]
	} dat1;
	unsigned short RecvType2; // Reply data-type specification 2
	union rtdata2
	{				  // Monitor data 2
		POSE pos2;	  // XYZ type [mm/rad]
		JOINT jnt2;	  // JOINT type [mm/rad]
		PULSE pls2;	  // PULSE type [mm/rad] or Integer type [% / non-unit]
		float lng2[8]; // Integer type [% / non-unit]
	} dat2;
	unsigned short RecvType3; // Reply data-type specification 3

	union rtdata3
	{				  // Monitor data 3
		POSE pos3;	  // XYZ type [mm/rad]
		JOINT jnt3;	  // JOINT type [mm/rad]
		PULSE pls3;	  // PULSE type [mm/rad] or Integer type [% / non-unit]
		float lng3[8]; // Integer type [% / non-unit]
	} dat3;
} MXTCMD;


#define NO_FLAGS_SET 0
#define MAXBUFLEN 512
using namespace std;



int main(void)
{
    struct sockaddr_in destSockAddr;
    int destSocket, i, socketLen = sizeof(destSockAddr);
    char sendText[MAXBUFLEN];
    char recvText[MAXBUFLEN];
    char type, type_mon[4];
    unsigned short IOSendType = 0; // Send input/output signal data designation
	unsigned short IORecvType = 0; // Reply input/output signal data designation
	unsigned short IOBitTop = 0;
	unsigned short IOBitMask = 0xffff;
	unsigned short IOBitData = 0;


	type = 2;
	type_mon[0] = 1;
	type_mon[1] = 1;
	type_mon[2] = 1;
	type_mon[3] = 1;

    printf("IP=%s / PORT=%d / Send Type=%d / Monitor Type0/1/2/3=%d/%d/%d/%d\n", DEST_IP_ADDR, PORT, type, type_mon[0], type_mon[1], type_mon[2], type_mon[3]);


    if ((destSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        die("socket");
        printf("\nSocket error.\n");
        return 1;
    }

    memset((char *)&destSockAddr, 0, sizeof(destSockAddr));
    destSockAddr.sin_family = AF_INET;
    destSockAddr.sin_port = htons(PORT);

    if (inet_aton(DEST_IP_ADDR, &destSockAddr.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    MXTCMD MXTsend;
	MXTCMD MXTrecv;
	JOINT jnt_now;
	POSE pos_now;
	PULSE pls_now;
	float counter = 0;
	int loop = 1;
	int disp = 0;
	int disp_data = 0;
	int ch;
	float delta = (float)0.0;
	float ratio = 1;
	int retry;

	memset(&MXTsend, 0, sizeof(MXTsend));
	memset(&jnt_now, 0, sizeof(JOINT));
	memset(&pos_now, 0, sizeof(POSE));
	memset(&pls_now, 0, sizeof(PULSE));

    while(loop)
    {
        memset(&MXTsend, 0, sizeof(MXTsend));
	memset(&MXTrecv, 0, sizeof(MXTrecv));
        if (loop == 1)
		{ // Only first time
			MXTsend.Command = MXT_CMD_NULL;
			MXTsend.SendType = MXT_TYP_NULL;
			MXTsend.RecvType = type;
			MXTsend.SendIOType = MXT_IO_NULL;
			MXTsend.RecvIOType = IOSendType;
			MXTsend.CCount = counter = 0;
			loop = 2;
		}
		else
		{ // Second and following times
			MXTsend.Command = MXT_CMD_MOVE;
			MXTsend.SendType = type;
			MXTsend.RecvType = type_mon[0];
			MXTsend.RecvType1 = type_mon[1];
			MXTsend.RecvType2 = type_mon[2];
			MXTsend.RecvType3 = type_mon[3];

			switch (type)
			{
			case MXT_TYP_JOINT:
				memcpy(&MXTsend.dat.jnt, &jnt_now, sizeof(JOINT));
				MXTsend.dat.jnt.j1 += (float)(delta * ratio * 3.141592 / 180.0);
				break;
			case MXT_TYP_POSE:
				memcpy(&MXTsend.dat.pos, &pos_now, sizeof(POSE));
				MXTsend.dat.pos.w.x += (delta * ratio);
				break;
			case MXT_TYP_PULSE:
				memcpy(&MXTsend.dat.pls, &pls_now, sizeof(PULSE));
				MXTsend.dat.pls.p1 += (float)((delta * ratio) * 10);
				break;
			default:
				break;
			}

			MXTsend.SendIOType = IOSendType;
			MXTsend.RecvIOType = IORecvType;
			MXTsend.BitTop = IOBitTop;
			MXTsend.BitMask = IOBitMask;
			MXTsend.IoData = IOBitData;
			MXTsend.CCount = counter;
		}
      
			//ch = getData();/////BLOCKING
			ch = 'P';

			switch (ch)
			{
			case 0x0d:
				MXTsend.Command = MXT_CMD_END;
				loop = 0;
				break;
			case 'P':
				delta += (float)0.05;
				break;
			case 'N':
				delta -= (float)0.3;
				break;
			case 'H':
				delta = (float)0.0;
				break;
			case 'd':
				disp = ~disp;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
				disp_data = ch - '0';
				break;
			}


		memset(sendText, 0, MAXBUFLEN);
		memcpy(sendText, &MXTsend, sizeof(MXTsend));
		
		printf("Data Length: %d\n", sizeof(MXTsend));
		printf("Data Length (.dat): %d\n", sizeof(MXTsend.dat));
		printf("Data Length (.dat.pos): %d\n", sizeof(MXTsend.dat.pos));
		printf("Data Length (.dat.jnt): %d\n", sizeof(MXTsend.dat.jnt));
		printf("Data Length (.dat.pls): %d\n", sizeof(MXTsend.dat.pls));
		printf("Data Length(float): %d\n", sizeof(float));
		printf("Data: ");
		fflush(stdout);
		
		int i;
		for(i = 0; i < sizeof(MXTsend); i++)
		{
			printf("%X ", sendText[i]);
		}
		
		printf("\n");
		
		printf("sendto Length: %d\n", strlen(sendText));
		
		
		// send the sendText
	    if (sendto(destSocket, sendText, sizeof(MXTsend), 0, (struct sockaddr *)&destSockAddr, socketLen) == -1)
	    {
		die("sendto()");
	    }
		sleep(1);

    }


    


    // receive a reply and print it
    // clear the sendText by filling null, it might have previously received data
    // try to receive some data, this is a blocking call
    

    puts(sendText);
    close(destSocket);
    return 0;
}

void die(char *s)
{
    perror(s);
    exit(1);
}

char getData(void)
{
	int fd;
	char readbuffer[1];
	uint8_t read_bytes;


	if ((fd = open(PIPEFILE, O_RDONLY)) == -1)
	{
		printf("Can't read the file:%s\n", PIPEFILE);
		return 1;
	}

	readbuffer[read_bytes] = '\0';
	if ((read_bytes = read(fd, readbuffer, sizeof(readbuffer))) != -1)
	{
		readbuffer[read_bytes] = '\0';
		printf("\nNew pipe data:%c\n", readbuffer[0]);
	}


	close(fd);
	return readbuffer[0];
}
