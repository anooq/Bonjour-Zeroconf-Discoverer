#include <stdio.h>			// For stdout, stderr
#include <string.h>			// For strlen(), strcpy()
#include <errno.h>			// For errno, EINTR
#include <time.h>
#include <sys/time.h>			// For struct timeval


#define SA_LEN(addr) (((addr)->sa_family == AF_INET6)? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in))

#include "dns_sd.h"
#include "ClientCommon.h"


//*************************************************************************************************************
// Globals

typedef union { unsigned char b[2]; unsigned short NotAnInteger; } Opaque16;

static uint32_t opinterface = kDNSServiceInterfaceIndexAny;
static DNSServiceRef client    = NULL;
static int num_printed;

#define LONG_TIME 100000000

static volatile int stopNow = 0;
static volatile int timeOut = LONG_TIME;


