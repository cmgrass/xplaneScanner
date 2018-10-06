#include <stdio.h>
#include <sys/socket.h>  /* for socket functions */
#include <arpa/inet.h>  /* for address types */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/util.h"

#define MAX_RECV_DGRAM 32

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
  float buffer[MAX_RECV_DGRAM];

  puts("Hi Chris!");

  /*----- Validate Args -----*/
  if (argc != 2) {
    Usage(argv);
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

  /*----- Main Loop -----*/
  for(;;) {

    if((recvMsgLen = recvfrom(sock, &buffer, MAX_RECV_DGRAM, 0,
       (struct sockaddr *)&xplaneAddr, (socklen_t *)&hostAddrLen)) < 0) {
      DieWithError("Error receiving on socket");
    }

    printf("got datagram from %s, len:%i\n", inet_ntoa(xplaneAddr.sin_addr), recvMsgLen);
    printf("0:%.2f, 1:%.2f, 2:%.2f, 3:%.2f, 4:%.2f, 5:%.2f, 6:%.2f, 7:%.2f\n\n",
           buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
           buffer[6], buffer[7]);

  }

  return SUCCESS;
}
