#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>  /* for socket functions */
#include <arpa/inet.h>  /* for address types */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/util.h"

#define MAX_RECV_DGRAM 64

void Usage(char *argv[])
{
  printf("Usage: %s <local port>\n", argv[0]);
}

int main(int argc, char *argv[])
{
  int sock;
  struct sockaddr_in scannerAddr;  /* udp server (this) */
  struct sockaddr_in xplaneAddr;  /* udp client */
  unsigned int hostAddrLen;
  unsigned short localPort;
  int recvMsgLen;
  uint8_t buffer[MAX_RECV_DGRAM];
  float myFloat[8];
  char dataStr[6];
  uint32_t floatInt;

  /*----- Validate Args -----*/
  if (argc != 2) {
    Usage(argv);
    exit(1);
  }

  /*----- Setup Server ----*/
  /* Initialization */
  localPort = atoi(argv[1]);
  hostAddrLen = sizeof(scannerAddr);
  memset(&scannerAddr, 0, hostAddrLen);
  scannerAddr.sin_family = AF_INET;
  scannerAddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* any incoming interface */
  scannerAddr.sin_port = htons(localPort);

  /* Get a socket */
  if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    DieWithError("Error creating UDP socket");
  }

  /* Bind address to our socket */
  if (bind(sock, (struct sockaddr *)&scannerAddr, (socklen_t)hostAddrLen) < 0) {
    DieWithError("Error binding UDP socket to local interface");
  }

  /* Prepare application data */
  memset(&dataStr, 0, 6);
  memset(&myFloat, 0, sizeof(myFloat));

  /*----- Main Loop -----*/
  for(;;) {

    if((recvMsgLen = recvfrom(sock, &buffer, MAX_RECV_DGRAM, 0,
       (struct sockaddr *)&xplaneAddr, (socklen_t *)&hostAddrLen)) < 0) {
      DieWithError("Error receiving on socket");
    }

    printf("rcvd dgram from %s, len:%i\n", inet_ntoa(xplaneAddr.sin_addr), recvMsgLen);
/*
    printf("0:%.2f, 1:%.2f, 2:%.2f, 3:%.2f, 4:%.2f, 5:%.2f, 6:%.2f, 7:%.2f\n\n",
           buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
           buffer[6], buffer[7]);
    printf("recvd: %s\n", buffer);
*/
    strncpy(dataStr, buffer, 5);
    myFloat[0] = (float)buffer[9];
    myFloat[1] = (float)buffer[13];
    myFloat[2] = (float)buffer[17];
/*
    myFloat[3] = (float)buffer[21];
*/
    myFloat[4] = (float)buffer[25];
    myFloat[5] = (float)buffer[29];
    myFloat[6] = (float)buffer[33];
    myFloat[7] = (float)buffer[37];

    floatInt = 0;
/*
    floatInt = ((buffer[21]<<24) & (buffer[22]<<16) & (buffer[23]<<8) & (buffer[24]));
*/
    floatInt = (uint32_t)buffer[24] << 24 |
                (uint32_t)buffer[23] << 16 |
                (uint32_t)buffer[22] << 8 |
                (uint32_t)buffer[21];

    myFloat[3] = *((float *)&floatInt);

    printf("HEADER[%s, %x]\n", dataStr, (short)buffer[5]);
    printf("HEADING: %hhx,%hhx,%hhx,%hhx\n", buffer[21], buffer[22], buffer[23], buffer[24]);
    printf("HEADING: %x\n", floatInt);
    printf("HEADING: %f\n", myFloat[3]);
/*
    printf("  f0: %.2f\n  f1:%.2f\n  f2:%.2f\n  f3:%.2f\n  f4:%.2f\n  f5:%.2f\n  f6:%.2f\n  f7:%.2f\n\n",
           myFloat[0], myFloat[1], myFloat[2], myFloat[3], myFloat[4],
           myFloat[5], myFloat[6], myFloat[7]);
*/

  }

  return SUCCESS;
}
