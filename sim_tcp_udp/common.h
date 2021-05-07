#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <netdb.h>
#include <dirent.h>
#include <sys/time.h>
#include <termios.h>

#define PP          printf
#define TRUE        1
#define FALSE       0
#define FAIL        (-1)
#define SUCCESS     (0)

#define HEADER      0x24

#define BUFSIZE     4096
#define LINESIZE    1024
#define LINESZ      100
#define TCP_MAXLEN  1024
#define MAX_SN      100
#define DISCONNECT  0x00
#define CONNECT     0x01
#define FD_GW_USESIZE 	10


typedef unsigned char  u8_t;     ///< unsigned 8-bit value (0 to 255)
typedef unsigned short u16_t;    ///< unsigned 16-bit value (0 to 65535)
typedef unsigned int u32_t;    ///< unsigned 32-bit value (0 to 4294967296)

typedef unsigned char  uint8_t;     ///< unsigned 8-bit value (0 to 255)
typedef unsigned short uint16_t;    ///< unsigned 16-bit value (0 to 65535)
typedef unsigned int uint32_t;    ///< unsigned 32-bit value (0 to 4294967296)

typedef unsigned char  int8u;     ///< unsigned 8-bit value (0 to 255)
typedef unsigned short int16u;    ///< unsigned 16-bit value (0 to 65535)
typedef unsigned int int32u;    ///< unsigned 32-bit value (0 to 4294967296)

/*
typedef char  int8_t;
typedef short int16_t;
typedef int int32_t;
*/

typedef enum {
	SIM_SERVER = 1,
	SIM_CLIENT = 2,
	SIM_TCP = 11,
	SIM_UDP= 12
} EN_SIM_CFG;


typedef struct
{
	int sim_type; // server, client
	int sim_proto; // tcp, udp
	int sim_port;

	char src_mac[20];
	char dst_mac[20];
	char src_ip[20];
	char dst_ip[20];
	char if_name[8];
} DS_BASE_INFO;

