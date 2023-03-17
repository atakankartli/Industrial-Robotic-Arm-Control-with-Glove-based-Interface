// sample.cpp
// Change the definition in the "strdef.h" file by the S/W version of the controller.
// Refer to the "strdef.h" file for details.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>

#include <string.h>
#include <math.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PIPEFILE "node2c.fifo"
char getData(void);


//************************************************************************************
// Real-time control sample program
// Communication packet data structure definition header file
//************************************************************************************
// strdef.h
#define VER_H7
/*************************************************************************/
/* Joint coordinate system (Set unused axis to 0) */
/* Refer to the instruction manual enclosed */
/* with each robot for details on each element. */
/************************************************************************/
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
/*************************************************************************/
/* XYZ coordinate system (Set unused axis to 0) */
/* Refer to the instruction manual enclosed */
/* with each robot for details on each element. */
/************************************************************************/
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

/*************************************************************************/
/* Pulse coordinate system (Set unused axis to 0) */
/* These coordinates express each joint */
/* with a motor pulse value. */
/*************************************************************************/
typedef struct
{
	long p1; // Motor 1 axis
	long p2; // Motor 2 axis
	long p3; // Motor 3 axis
	long p4; // Motor 4 axis
	long p5; // Motor 5 axis
	long p6; // Motor 6 axis
	long p7; // Additional axis 1 (Motor 7 axis)
	long p8; // Additional axis 2 (Motor 8 axis)
} PULSE;

/************************************************************/
/* Real-time function communication data packet */
/************************************************************/
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
		long lng1[8]; // Integer type [% / non-unit]
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
	unsigned long CCount;	   // Transmission data counter value
	unsigned short RecvType1;  // Reply data-type specification 1
	union rtdata1
	{				  // Monitor data 1
		POSE pos1;	  // XYZ type [mm/rad]
		JOINT jnt1;	  // JOINT type [mm/rad]
		PULSE pls1;	  // PULSE type [mm/rad]
		long lng1[8]; // Integer type [% / non-unit]
	} dat1;
	unsigned short RecvType2; // Reply data-type specification 2
	union rtdata2
	{				  // Monitor data 2
		POSE pos2;	  // XYZ type [mm/rad]
		JOINT jnt2;	  // JOINT type [mm/rad]
		PULSE pls2;	  // PULSE type [mm/rad] or Integer type [% / non-unit]
		long lng2[8]; // Integer type [% / non-unit]
	} dat2;
	unsigned short RecvType3; // Reply data-type specification 3

	union rtdata3
	{				  // Monitor data 3
		POSE pos3;	  // XYZ type [mm/rad]
		JOINT jnt3;	  // JOINT type [mm/rad]
		PULSE pls3;	  // PULSE type [mm/rad] or Integer type [% / non-unit]
		long lng3[8]; // Integer type [% / non-unit]
	} dat3;
} MXTCMD;

#define NO_FLAGS_SET 0
#define MAXBUFLEN 512
using namespace std;

int main(void)
{
	WSADATA Data;
	SOCKADDR_IN destSockAddr;
	SOCKET destSocket;
	unsigned long destAddr;
	int status;
	int numsnt;
	int numrcv;
	char sendText[MAXBUFLEN];
	char recvText[MAXBUFLEN];
	char dst_ip_address[MAXBUFLEN];
	unsigned short port;
	char msg[MAXBUFLEN];
	char buf[MAXBUFLEN];
	char type, type_mon[4];
	unsigned short IOSendType = 0; // Send input/output signal data designation
	unsigned short IORecvType = 0; // Reply input/output signal data designation
	unsigned short IOBitTop = 0;
	unsigned short IOBitMask = 0xffff;
	unsigned short IOBitData = 0;

	cout << " Input connection destination IP address (192.168.0.20) ->";
	cin.getline(dst_ip_address, MAXBUFLEN);

	if (dst_ip_address[0] == 0)
		strcpy(dst_ip_address, "192.168.0.20");

	cout << " Input connection destination port No. (10000) -> ";
	cin.getline(msg, MAXBUFLEN);

	if (msg[0] != 0)
		port = atoi(msg);
	else
		port = 10000;

	// cout << " Use input/output signal?([Y] / [N])-> ";
	// cin.getline(msg, MAXBUFLEN);
	//
	// if (msg[0] != 0 && (msg[0] == 'Y' || msg[0] == 'y')) {
	//	cout << " What is target? Input signal/output signal([I]nput / [O]utput)-> ";
	//	cin.getline(msg, MAXBUFLEN);
	//	switch (msg[0]) {
	//	case 'O': // Set target to output signal
	//	case 'o':
	//		IOSendType = MXT_IO_OUT;
	//		IORecvType = MXT_IO_OUT;
	//		break;
	//
	//	case 'I': // Set target to input signal
	//	case 'i':
	//	default:
	//		IOSendType = MXT_IO_NULL;
	//		IORecvType = MXT_IO_IN;
	//		break;
	//	}

	//	cout << " Input head bit No. (0 to 32767)-> ";
	//	cin.getline(msg, MAXBUFLEN);

	//	if (msg[0] != 0)
	//		IOBitTop = atoi(msg);
	//	else
	//		IOBitTop = 0;
	//
	//	if (IOSendType == MXT_IO_OUT) { // Only for output signal
	//		cout << " Input bit mask pattern for output as hexadecimal (0000 to FFFF)-> ";
	//		cin.getline(msg, MAXBUFLEN);
	//		if (msg[0] != 0)
	//			sscanf(msg, "%4x", &IOBitMask);
	//		else
	//			IOBitMask = 0;
	//
	//		cout << " Input bit data for output as hexadecimal (0000 to FFFF)-> ";
	//		cin.getline(msg, MAXBUFLEN);
	//
	//		if (msg[0] != 0)
	//			sscanf(msg, "%4x", &IOBitData);
	//		else
	//			IOBitData = 0;
	//	}
	//}

	type = 2;
	type_mon[0] = 1;
	type_mon[1] = 1;
	type_mon[2] = 1;
	type_mon[3] = 1;


	

	sprintf(msg, "IP=%s / PORT=%d / Send Type=%d / Monitor Type0/1/2/3=%d/%d/%d/%d", dst_ip_address, port, type, type_mon[0], type_mon[1], type_mon[2], type_mon[3]);
	cout << msg << endl;
	cout << "[Enter]= End / [d]= Monitor data display";
	cout << "[z/x]= Increment/decrement first command data transmitted by the delta amount. ";
	cout << " Is it all right? [Enter] / [Ctrl+C] ";
	cin.getline(msg, MAXBUFLEN);

	// Windows Socket DLL initialization
	status = WSAStartup(MAKEWORD(1, 1), &Data);
	if (status != 0)
		cerr << "ERROR: WSAStartup unsuccessful" << endl;

	// IP address, port, etc., setting
	memset(&destSockAddr, 0, sizeof(destSockAddr));
	destAddr = inet_addr(dst_ip_address);
	memcpy(&destSockAddr.sin_addr, &destAddr, sizeof(destAddr));
	destSockAddr.sin_port = htons(port);
	destSockAddr.sin_family = AF_INET;

	// Socket creation
	destSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (destSocket == INVALID_SOCKET)
	{
		cerr << "ERROR: socket unsuccessful" << endl;
		status = WSACleanup();
		if (status == SOCKET_ERROR)
			cerr << "ERROR: WSACleanup unsuccessful" << endl;
		return (1);
	}

	MXTCMD MXTsend;
	MXTCMD MXTrecv;
	JOINT jnt_now;
	POSE pos_now;
	PULSE pls_now;
	unsigned long counter = 0;
	int loop = 1;
	int disp = 0;
	int disp_data = 0;
	int ch;
	float delta = (float)0.0;
	long ratio = 1;
	int retry;
	fd_set SockSet;	  // Socket group used with select
	timeval sTimeOut; // For timeout setting
	memset(&MXTsend, 0, sizeof(MXTsend));
	memset(&jnt_now, 0, sizeof(JOINT));
	memset(&pos_now, 0, sizeof(POSE));
	memset(&pls_now, 0, sizeof(PULSE));

	while (loop)
	{
		memset(&MXTsend, 0, sizeof(MXTsend));
		memset(&MXTrecv, 0, sizeof(MXTrecv));

		// Transmission data creation
		if (loop == 1)
		{ // Only first time
			MXTsend.Command = MXT_CMD_NULL;
			MXTsend.SendType = MXT_TYP_NULL;
			MXTsend.RecvType = type;
			MXTsend.SendIOType = MXT_IO_NULL;
			MXTsend.RecvIOType = IOSendType;
			MXTsend.CCount = counter = 0;
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
				MXTsend.dat.pls.p1 += (long)((delta * ratio) * 10);
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

		// Keyboard input
		// [Enter]=End / [d]= Display the monitor data, or none / [0/1/2/3]= Change of monitor data display
		// [z/x]=Increment/decrement first command data transmitted by the delta amount
		while (_kbhit() != 0)
		{
			ch = getData();

			switch (ch)
			{
			case 0x0d:
				MXTsend.Command = MXT_CMD_END;
				loop = 0;
				break;
			case 'P':
				delta += (float)0.3;
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
		}

		memset(sendText, 0, MAXBUFLEN);
		memcpy(sendText, &MXTsend, sizeof(MXTsend));

		if (disp)
		{
			sprintf(buf, "Send (%ld):", counter);
			cout << buf << endl;
		}
		numsnt = sendto(destSocket, sendText, sizeof(MXTCMD), NO_FLAGS_SET, (LPSOCKADDR)&destSockAddr, sizeof(destSockAddr));

		if (numsnt != sizeof(MXTCMD))
		{
			cerr << "ERROR: sendto unsuccessful" << endl;
			status = closesocket(destSocket);
			if (status == SOCKET_ERROR)
				cerr << "ERROR: closesocket unsuccessful" << endl;

			status = WSACleanup();

			if (status == SOCKET_ERROR)
				cerr << "ERROR: WSACleanup unsuccessful" << endl;

			return (1);
		}

		memset(recvText, 0, MAXBUFLEN);
		retry = 1; // No. of reception retries

		while (retry)
		{
			FD_ZERO(&SockSet);			  // SockSet initialization
			FD_SET(destSocket, &SockSet); // Socket registration
			sTimeOut.tv_sec = 1;		  // Transmission timeout setting (sec)
			sTimeOut.tv_usec = 0;		  // (micro sec)
			status = select(0, &SockSet, (fd_set *)NULL, (fd_set *)NULL, &sTimeOut);
			if (status == SOCKET_ERROR)
			{
				return (1);
			}

			if ((status > 0) && (FD_ISSET(destSocket, &SockSet) != 0))
			{ // If it receives by the time-out
				numrcv = recvfrom(destSocket, recvText, MAXBUFLEN, NO_FLAGS_SET, NULL, NULL);
				if (numrcv == SOCKET_ERROR)
				{
					cerr << "ERROR: recvfrom unsuccessful" << endl;
					status = closesocket(destSocket);
					if (status == SOCKET_ERROR)
						cerr << "ERROR: closesocket unsuccessful" << endl;
					status = WSACleanup();
					if (status == SOCKET_ERROR)
						cerr << "ERROR: WSACleanup unsuccessful" << endl;
					return (1);
				}

				memcpy(&MXTrecv, recvText, sizeof(MXTrecv));
				char str[10];

				if (MXTrecv.SendIOType == MXT_IO_IN)
					sprintf(str, "IN%04x", MXTrecv.IoData);

				else if (MXTrecv.SendIOType == MXT_IO_OUT)
					sprintf(str, "OT%04x", MXTrecv.IoData);
				else
					sprintf(str, "------");

				int DispType;
				void *DispData = nullptr;

				switch (disp_data)
				{
				case 0:
					DispType = MXTrecv.RecvType;
					DispData = &MXTrecv.dat;
					break;
				case 1:
					DispType = MXTrecv.RecvType1;
					DispData = &MXTrecv.dat1;
					break;
				case 2:
					DispType = MXTrecv.RecvType2;
					DispData = &MXTrecv.dat2;
					break;
				case 3:
					DispType = MXTrecv.RecvType3;
					DispData = &MXTrecv.dat3;
					break;
				default:
					break;
				}

				switch (DispType)
				{
				case MXT_TYP_JOINT:
				case MXT_TYP_FJOINT:
				case MXT_TYP_FB_JOINT:
					if (loop == 1)
					{
						memcpy(&jnt_now, DispData, sizeof(JOINT));
						loop = 2;
					}
					if (disp)
					{
						JOINT *j = (JOINT *)DispData;
						sprintf(buf, "Receive (%ld): TCount=%d Type(JOINT) = % d\n % 7.2f, % 7.2f, % 7.2f, % 7.2f, % 7.2f, % 7.2f, % 7.2f, % 7.2f (% s)", MXTrecv.CCount, MXTrecv.TCount, DispType, j->j1, j->j2, j->j3, j->j4, j->j5, j->j6, j->j7, j->j8, str);
						cout << buf << endl;
					}
					break;

				case MXT_TYP_POSE:
				case MXT_TYP_FPOSE:
				case MXT_TYP_FB_POSE:
					if (loop == 1)
					{
						memcpy(&pos_now, &MXTrecv.dat.pos, sizeof(POSE));
						loop = 2;
					}
					if (disp)
					{
						POSE *p = (POSE *)DispData;
						sprintf(buf, "Receive (%ld): TCount=%d Type(POSE) = % d\n % 7.2f, % 7.2f, % 7.2f, % 7.2f, % 7.2f, % 7.2f, % 04x, % 04x(% s)", MXTrecv.CCount, MXTrecv.TCount, DispType, p->w.x, p->w.y, p->w.z, p->w.a, p->w.b, p->w.c, p->intsflg1, p->intsflg2, str);
						cout << buf << endl;
					}
					break;

				case MXT_TYP_PULSE:
				case MXT_TYP_FPULSE:
				case MXT_TYP_FB_PULSE:
				case MXT_TYP_CMDCUR:
				case MXT_TYP_FBKCUR:
					if (loop == 1)
					{
						memcpy(&pls_now, &MXTrecv.dat.pls, sizeof(PULSE));
						loop = 2;
					}
					if (disp)
					{
						PULSE *l = (PULSE *)DispData;
						sprintf(buf, "Receive (%ld): TCount=%d Type(PULSE / OTHER) = % d\n % ld, % ld, % ld, % ld, % ld, % ld, % ld, % ld(% s)", MXTrecv.CCount, MXTrecv.TCount, DispType, l->p1, l->p2, l->p3, l->p4, l->p5, l->p6, l->p7, l->p8, str);
						cout << buf << endl;
					}
					break;

				case MXT_TYP_NULL:
					if (loop == 1)
					{
						loop = 2;
					}
					if (disp)
					{
						sprintf(buf, "Receive (%ld): TCount=%d Type(NULL)=%d\n (%s)", MXTrecv.CCount, MXTrecv.TCount, DispType, str);
						cout << buf << endl;
					}
					break;

				default:
					cout << "Bad data type.\n"
						 << endl;
					break;
				}

				counter++; // Count up only when communication is successful
				retry = 0; // Leave reception loop
			}

			else
			{ // Reception timeout
				cout << "... Receive Timeout! <Push [Enter] to stop the program>" << endl;
				retry--; // No. of retries subtraction
				if (retry == 0)
					loop = 0; // End program if No. of retries is 0
			}
		} /* while(retry) */
	}	  /* while(loop) */
	// End

	cout << "/// End /// ";
	sprintf(buf, "counter = %ld", counter);
	cout << buf << endl;
	// Close socket
	status = closesocket(destSocket);

	if (status == SOCKET_ERROR)
		cerr << "ERROR: closesocket unsuccessful" << endl;

	status = WSACleanup();

	if (status == SOCKET_ERROR)
		cerr << "ERROR: WSACleanup unsuccessful" << endl;

	return 0;
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
