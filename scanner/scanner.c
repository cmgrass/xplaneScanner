#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>  /* for socket functions */
#include <arpa/inet.h>  /* for address types */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/util.h"

/* DEBUG MACROS */
/*
#define CMG_DBG 1
*/
#ifdef CMG_DBG 
#define CMG_PRINT(x) printf x
#else
#define CMG_PRINT(X) do {} while(0)
#endif

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
  uint32_t floatInt2;

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
    strncpy(dataStr, buffer, 5);
    floatInt = 0;

    floatInt = (uint32_t)buffer[24] << 24 |
                (uint32_t)buffer[23] << 16 |
                (uint32_t)buffer[22] << 8 |
                (uint32_t)buffer[21];
    floatInt2 = (uint32_t)buffer[20] << 24 |
                (uint32_t)buffer[19] << 16 |
                (uint32_t)buffer[18] << 8 |
                (uint32_t)buffer[17];


    myFloat[3] = *((float *)&floatInt);
    myFloat[2] = *((float *)&floatInt2);

    CMG_PRINT(("HEADER[%s, %x]\n", dataStr, (short)buffer[5]));
    CMG_PRINT(("HEADING mag: %hhx,%hhx,%hhx,%hhx\n", buffer[21], buffer[22], buffer[23], buffer[24]));
    CMG_PRINT(("HEADING mag: %x\n", floatInt));
    printf("HEADING mag: %f\n", myFloat[3]);
    printf("HEADING true: %f\n", myFloat[2]);
  }

  return SUCCESS;
}
