#include "common.h"

#define FP(fmt, args...) 

#define PNAME "psim"

DS_BASE_INFO gl_base;

char gl_serverAddr[24] = "192.168.0.106";
int gl_tcp_conn_flag = DISCONNECT;
int gl_ServSocketFd = 0;
int gl_SocketFailCnt = 0;


#if 0
char *CFG_Trim(char *str)
{
	char *buf;
	int len;

	buf = str;

	len = strlen(buf) - 1;
	while(buf[len] == ' ' || buf[len] == '\n' || buf[len] == '\r')  buf[len--] = 0;
	while(*buf == ' ')  buf++;
	strcpy(str, buf);

	return str;
}

int CFG_GetInteger(const char *fname, const char *Part, const char *Tag, int *Val, int Default)
{
	 FILE  *file;
	 char  buf[256], buf1[256];
	 int   sflag, i;

	 file = fopen(fname, "r");
	 if(file == NULL)  return -1;

	 sflag = 0;
	 while(1)
	 {
		if (fgets(buf, sizeof(buf), file) == NULL)  break;
		if (buf == NULL)  break;
		CFG_Trim(buf);

		if (buf[0] == '[') {
			if (strncmp(CFG_Trim(&buf[1]), Part, strlen(Part)) == 0)  
		   		sflag = 1;
		}
		else if (buf[0] == '#') {  
			continue;  
		}
		else {
			if (sflag == 1 && buf[0] == '[') {
				fclose(file);
				return -1;		// Not found
			}
			if(sflag == 1 && strncmp(buf, Tag, strlen(Tag)) == 0) 
			{
			  	for(i=0; i<(int)strlen(buf) && buf[i]!='='; i++);
				strcpy(buf1, &buf[i+1]);
				CFG_Trim(buf1);
				
				*Val = atoi(buf1);
				fclose(file);
		    	return 0;
			}
		}
	 }
	 *Val = Default;
	 fclose(file);

	 return -1;	// Not found
}


int CFG_GetString(const char *fname, const char *Part, const char *Tag, char *Val, const char *Default)
{
	FILE  *file;
	char  buf[256], buf1[256];
	int   sflag, i;

	file = fopen(fname, "r");
	if (file == NULL) {  
		PP("%s open fail\n", fname);
		return -1;
	}
	sflag = 0;
	while(1)
	{
		if (fgets(buf, sizeof(buf), file) == NULL)  break;
		if (buf == NULL)  break;
		CFG_Trim(buf);
		if (buf[0] == '[') {
			if (strncmp(CFG_Trim(&buf[1]), Part, strlen(Part)) == 0) {
				sflag = 1;
			}
		}
		else if(buf[0] == '#') {  
			continue;  
		} 
		else { 
			if (sflag == 1 && buf[0] == '[') { 
				fclose(file);
				return -1;		// Not found
			}
			if (strncmp(buf, Tag, strlen(Tag)) == 0 && sflag == 1) {
				for(i=0; i<(int)strlen(buf) && buf[i]!='='; i++);
			    strcpy(buf1, &buf[i+2]);
			    strcpy(Val, buf1);
				fclose(file);
			    return 0;
			}
		}
	}
	strcpy(Val, Default);
	fclose(file);
	return -1;	// Not found
}

int	CFG_Exit(const char *ReadFile, const char *Item) 
{
	PP("config file error\n");
	PP("File = %s\n", ReadFile);
	PP("Item = %s\n", Item);
	exit(1);
}

void CFG_Init(const char *fname)
{
	int ret;

	memset(&gl_base, 0, sizeof(DS_BASE_INFO));

	// string type
	ret =(CFG_GetString(fname, PNAME, "SRC_MAC", gl_base.src_mac,"")==0)?0:CFG_Exit(fname,"SRC_MAC");
	ret =(CFG_GetString(fname, PNAME, "DST_MAC", gl_base.dst_mac,"")==0)?0:CFG_Exit(fname,"DST_MAC");
	
	ret =(CFG_GetString(fname, PNAME, "SRC_IP" , gl_base.src_ip,"")==0)?0:CFG_Exit(fname,"SRC_IP");
	ret =(CFG_GetString(fname, PNAME, "DST_IP" , gl_base.dst_ip,"")==0)?0:CFG_Exit(fname,"DST_IP");
	ret =(CFG_GetString(fname, PNAME, "IF_NAME", gl_base.if_name,"")==0)?0:CFG_Exit(fname,"IF_NAME");
		
	// integer type
	ret =(CFG_GetString(fname, PNAME, "SIM_MODE", gl_base.sim_mode, 0)==0)?0:CFG_Exit(fname,"SIM_MODE");
	ret =(CFG_GetInteger(fname,PNAME, "SRC_PORT", &gl_base.src_port, 1234)==0)?0:CFG_Exit(fname, "SRC_PORT");
	ret =(CFG_GetInteger(fname,PNAME, "DST_PORT", &gl_base.dst_port, 1234)==0)?0:CFG_Exit(fname, "DST_PORT");
	ret =(CFG_GetInteger(fname,PNAME, "VLAN_FLAG", &gl_base.vlan_flag, 0)==0)?0:CFG_Exit(fname, "VLAN_FLAG");
	ret =(CFG_GetInteger(fname,PNAME, "VLAN_ID", &gl_base.vlan_id, 100)==0)?0:CFG_Exit(fname, "VLAN_ID");

	PP("SIM_MODE= %d(0:rx, 1:tx, 2:all) \n", gl_base.sim_mode);
	PP("IF_NAME = %s \n", gl_base.if_name);

	PP("SRC_MAC = %s \n", gl_base.src_mac);
	PP("DST_MAC = %s \n", gl_base.dst_mac);
	PP("SRC_IP  = %s \n", gl_base.src_ip);
	PP("DST_IP  = %s \n", gl_base.dst_ip);

	PP("SRC_PORT= %d \n", gl_base.src_port);
	PP("DST_PORT= %d \n", gl_base.dst_port);
	PP("VLAN_ID = %d \n", gl_base.vlan_id);
	PP("VLAN_FLAG = %d \n", gl_base.vlan_flag);
}
#endif


int pd_Send_TcpData(int socket, u8_t *data, int length)
{
	int i;
	int sent;
	int tmpres;

	sent = 0;

	if (length > 0) {
		while(sent < length) {
			tmpres = send(socket, data, length, 0);
			if (tmpres==-1) {
				PP("[%s] TCP_Send_Fault : Can't send buffer\n", __func__);
				return -1;
			}
			else {
				sent+=tmpres;
			}
		}
#if 1
		PP("[%s] length[%d]= \n", __func__, length);
		for(i=0; i<length; i++) {
			PP("%x ", data[i]);
		}
		PP("\n");
#endif
		return 1;
	}
	return 0;
}


int ConnectWait(int sockfd, struct sockaddr *saddr, int addrsize, int sec)
{
	int res;
	//struct sockaddr_in addr;
	long arg;
	fd_set myset;
	struct timeval tv;
	int valopt;
	socklen_t lon;

	if((arg = fcntl(sockfd, F_GETFL, NULL)) < 0) {
		fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
		return FAIL;
	}

	arg |= O_NONBLOCK;
	if(fcntl(sockfd, F_SETFL, arg) < 0) {
		fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
		return FAIL;
	}

	res = connect(sockfd, saddr, addrsize);
	if(res < 0) {
		if(errno == EINPROGRESS) {
			fprintf(stderr, "EINPROGRESS in connect() - selecting\n");
			do {
				tv.tv_sec = sec;
				tv.tv_usec = 0;
				FD_ZERO(&myset);
				FD_SET(sockfd, &myset);
				res = select(sockfd + 1, NULL, &myset, NULL, &tv);
				if(res < 0 && errno != EINTR)
				{
					fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
					return FAIL;
				}
				else if(res > 0) 	{
					lon = sizeof(int);
					if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*) (&valopt), &lon) < 0) {
						fprintf(stderr, "Error in getsockopt() %d - %s\n", errno, strerror(errno));
						return FAIL;
					}
					if(valopt) {
						fprintf(stderr, "Error in delayed connection() %d - %s\n", valopt,
						strerror(valopt));
						return FAIL;
					}
					break;
				}
				else {
					fprintf(stderr, "Timeout in select() - Cancelling!\n");
					return FAIL;
				}
			} while(1);
		}
		else {
			fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
			return FAIL;
		}
	}
	if((arg = fcntl(sockfd, F_GETFL, NULL)) < 0) {
		fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
		return FAIL;
	}
	arg &= (~O_NONBLOCK);
	if(fcntl(sockfd, F_SETFL, arg) < 0) {
		fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
		return FAIL;
	}
	return SUCCESS;
}
                    

int pd_Parse_RcvData(u8_t *data, int nread)
{
	int i;
	PP("[%s] \n", __func__);
	PP("RCV_DATA[%d]= ", nread);
	for(i=0; i<nread; i++) {
		PP("%02x ", data[i]);
	}
	PP("\n");
}


void *TH_UDP_CLIENT(void *data)
{
	void* dummyPointer; // dummy return pointer
	struct sockaddr_in serv_addr;
	struct addrinfo hints;
	int sockfd;
	char ip_addr[20];
	int ret;
	int i, n;
	int len;
	struct addrinfo* res;
	int err;
	struct sockaddr_in* resaddr;
	int dnsfialCnt = 0;
	int socketfialCnt = 0;

	int nread;
	int read_size;
	char server_ipStr[20];

	char tbuff[20] = "ABCD1234";

	u8_t rcvBuf[TCP_MAXLEN];
	u8_t msg[TCP_MAXLEN];
	char half_flag = 0;
	char cmpl_flag = 0;

	u32_t offset = 0;
	u32_t rem_size = 0;
	u32_t msg_size = 0;
	u32_t half_size = 0;
	u32_t half_rem_size = 0;
	u16_t msg_id;
	int addrlen = sizeof(serv_addr); //서버 주소의 size를 저장

	int seq = 0;

UDP_Client_Loop:
	dnsfialCnt = 0;
	socketfialCnt = 0;

	while(gl_tcp_conn_flag == DISCONNECT)
	{
		if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
		{
			perror("socket");
			sleep(3);
			if(++socketfialCnt > 5)
			{
				//system("reboot");
			}
			continue;
		}
		memset((char *) &serv_addr, 0, sizeof(serv_addr));
		memset((char *) &hints, 0, sizeof(hints));
		memset(&ip_addr, 0, sizeof(ip_addr));

		PP("[%s] HOST=%s, PORT=%d\n", __func__, gl_base.dst_ip, gl_base.sim_port);
		res = 0;
		hints.ai_family = AF_INET;
		hints.ai_socktype = 0;
		hints.ai_protocol = 0;
		if((err = getaddrinfo(gl_base.dst_ip, 0, &hints, &res)) != 0)
		{
			sleep(3);
			perror("getaddrinfo--");
			if(++dnsfialCnt > 3)
			{
				//system("reboot");
			}
			continue;
		}
		PP("'%s'", inet_ntoa(((struct sockaddr_in *) res->ai_addr)->sin_addr));
		resaddr = (struct sockaddr_in*) res->ai_addr;

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(gl_base.sim_port);
		serv_addr.sin_addr = resaddr->sin_addr;
		PP("[%s] HOST=%s, PORT=%d\n", __func__, gl_base.dst_ip, gl_base.sim_port);

		freeaddrinfo(res);
		bzero(&(serv_addr.sin_zero), 8);

		if(ConnectWait(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr), 5) == FAIL)
		{
			perror("connect");
			close(sockfd);
			PP("[%s] connect fail!\n", __func__);
			sleep(3);
			continue;
		}
		else
		{
			gl_ServSocketFd = sockfd;
			gl_tcp_conn_flag = CONNECT;
			PP("\nSever connect\n");

			// Test
			if(sendto(gl_ServSocketFd, tbuff, strlen(tbuff), 0, (struct sockaddr *)&serv_addr, addrlen) < 0) {
				perror("sendto fail");
			}
		}
	}
	//------------------ Response Message Processing -----------------------

	while(gl_tcp_conn_flag == CONNECT)
	{
		len = recvfrom(gl_ServSocketFd, rcvBuf, 1024, 0, (struct sockaddr *)&serv_addr, &addrlen);
				
		if (len > 0 )
		{
			rem_size = len;
			pd_Parse_RcvData(rcvBuf, rem_size);
				
			memset(msg, 0, sizeof(msg));
			snprintf(msg, sizeof(msg), "client-send-seq=%d", ++seq);
			if(sendto(gl_ServSocketFd, msg, strlen(msg), 0, (struct sockaddr *)&serv_addr, addrlen) < 0) {
				perror("sendto fail");
			}
		}
	}
	
	gl_tcp_conn_flag = DISCONNECT;
	close(gl_ServSocketFd);
	PP("Client thread length fail\n");
	sleep(3);
	goto UDP_Client_Loop;
}


void *TH_UDP_SERVER(void *data)
{
	struct sockaddr_in clientaddr, serveraddr;
	int server_sockfd, client_sockfd, sockfd;
	int client_len;
	int nread;
	u8_t rcvBuf[1024];
	int  state=0;
	char client_ipStr[20];
	u8_t msg[512];
	int seq = 1;

	PP("--------- %s Start --------\n", __func__);

Socket_UDP_Loop_1:
	/**********************************
	* soctket
	**********************************/
	if ((server_sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		PP( "[Thread_EweenServer] socket error\n");
		sleep(3);
		goto Socket_UDP_Loop_1;
	}

	bzero((char *)&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(gl_base.sim_port);

Bind_UDP_Loop_2:
	/**********************************
	* bind
	**********************************/
	state = bind (server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (state == -1) {
		PP( "[Thread_EweenServer] bind error\n");
		sleep(3);
		goto Bind_UDP_Loop_2;
	}

	/**********************************
	* recvfrom
	**********************************/
	while(1) {
		client_len = sizeof(clientaddr);
		nread = recvfrom(server_sockfd, rcvBuf, 1024, 0, (struct sockaddr *)&clientaddr, &client_len);

		memset(client_ipStr, 0, 20);
		sprintf(client_ipStr, "%s", inet_ntoa(clientaddr.sin_addr));
		pd_Parse_RcvData(rcvBuf, nread);

		sleep(3);
		memset(msg, 0, sizeof(msg));
		snprintf(msg, sizeof(msg), "sever-send-seq=%d", ++seq);

		if(sendto(server_sockfd, msg, strlen(msg), 0, (struct sockaddr *)&clientaddr, client_len) < 0) {
			perror("sendto fail");
		}
	}
	close (server_sockfd);

}

//=========================================


void *TH_TCP_CLIENT(void *data)
{
	void* dummyPointer; // dummy return pointer
	struct sockaddr_in serv_addr;
	struct addrinfo hints;
	int sockfd;
	char ip_addr[20];
	int ret;
	int i, n;
	int len;
	struct addrinfo* res;
	int err;
	struct sockaddr_in* resaddr;
	int dnsfialCnt = 0;
	int socketfialCnt = 0;

	int nread;
	int read_size;
	char server_ipStr[20];

	char tbuff[20] = "ABCD1234";

	u8_t rcvBuf[TCP_MAXLEN];
	u8_t msg[TCP_MAXLEN];
	char half_flag = 0;
	char cmpl_flag = 0;

	u32_t offset = 0;
	u32_t rem_size = 0;
	u32_t msg_size = 0;
	u32_t half_size = 0;
	u32_t half_rem_size = 0;
	u16_t msg_id;

	int seq = 0;

Socket_Loop:
	dnsfialCnt = 0;
	socketfialCnt = 0;

	while(gl_tcp_conn_flag == DISCONNECT)
	{
		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			perror("socket");
			sleep(3);
			if(++socketfialCnt > 5)
			{
				//system("reboot");
			}
			continue;
		}
		memset((char *) &serv_addr, 0, sizeof(serv_addr));
		memset((char *) &hints, 0, sizeof(hints));
		memset(&ip_addr, 0, sizeof(ip_addr));

		PP("[%s] HOST=%s, PORT=%d\n", __func__, gl_base.dst_ip, gl_base.sim_port);
		res = 0;
		hints.ai_family = AF_INET;
		hints.ai_socktype = 0;
		hints.ai_protocol = 0;
		if((err = getaddrinfo(gl_base.dst_ip, 0, &hints, &res)) != 0)
		{
			sleep(3);
			perror("getaddrinfo--");
			if(++dnsfialCnt > 3)
			{
				//system("reboot");
			}
			continue;
		}
		PP("'%s'", inet_ntoa(((struct sockaddr_in *) res->ai_addr)->sin_addr));
		resaddr = (struct sockaddr_in*) res->ai_addr;

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(gl_base.sim_port);
		serv_addr.sin_addr = resaddr->sin_addr;
		PP("[%s] HOST=%s, PORT=%d\n", __func__, gl_base.dst_ip, gl_base.sim_port);

		freeaddrinfo(res);
		bzero(&(serv_addr.sin_zero), 8);

		if(ConnectWait(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr), 5) == FAIL)
		{
			perror("connect");
			close(sockfd);
			PP("[%s] connect fail!\n", __func__);
			sleep(3);
			continue;
		}
		else
		{
			gl_ServSocketFd = sockfd;
			gl_tcp_conn_flag = CONNECT;
			PP("\nSever connect\n");

			// Test
			pd_Send_TcpData(gl_ServSocketFd, tbuff, 8);
		}
	}
	//------------------ Response Message Processing -----------------------

	while(gl_tcp_conn_flag == CONNECT)
	{
		len = (int) read(gl_ServSocketFd, rcvBuf, 1024);
		//len = recv(gl_ServSocketFd, rcvBuf, nread, 0);
		if (len > 0 )
		{
			rem_size = len;
			pd_Parse_RcvData(rcvBuf, rem_size);
			//pd_Send_TcpData(gl_ServSocketFd, tbuff, 8);
				
			memset(msg, 0, sizeof(msg));
			snprintf(msg, sizeof(msg), "client-send-seq=%d", ++seq);

			pd_Send_TcpData(gl_ServSocketFd, msg, strlen(msg));
		}
		/*
		else {
			gl_tcp_conn_flag = DISCONNECT;
			close(gl_ServSocketFd);
			PP("Client thread length fail\n");
			sleep(3);
			goto Socket_Loop;
		}

		if(gl_SocketFailCnt > 3)
		{
			gl_tcp_conn_flag = DISCONNECT;
			close(gl_ServSocketFd);
			PP("Client thread send fail\n");
			goto Socket_Loop;
		}
		*/
	}
	
	gl_tcp_conn_flag = DISCONNECT;
	close(gl_ServSocketFd);
	PP("Client thread length fail\n");
	sleep(3);
	goto Socket_Loop;
}


void *TH_TCP_SERVER(void *data)
{
	struct sockaddr_in clientaddr, serveraddr;
	int server_sockfd, client_sockfd, sockfd;
	int fd_num;
	int i, maxi, maxfd, client_len;
	int client[FD_GW_USESIZE];
	int reuseaddr_on = 1;
	fd_set readfds, tempfds;
	int  state=0;
	int  msgid;
	u8_t idx;
	struct timeval timeout;

	FILE *fp;
	int nread, ret;

	char client_ipStr[20];

	u8_t rcvBuf[256];
	u8_t msg[512];
	char half_flag = 0;
	char cmpl_flag = 0;
	u32_t offset = 0;
	u32_t rem_size = 0;
	u32_t msg_size = 0;
	u32_t half_size = 0;
	u32_t half_rem_size = 0;
	u16_t msg_id;

	int read_size;
	int len;
	int seq = 0;

Socket_TCP_Loop_1:
	/**********************************
	* soctket Create
	**********************************/
	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		sleep(5);
		goto Socket_TCP_Loop_1;
	}

	bzero((char *)&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(gl_base.sim_port);

Socket_TCP_Loop:
	/**********************************
	* bind
	**********************************/
	state = bind (server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (state == -1) {
		sleep(5);
		goto Socket_TCP_Loop;
	}

	state = listen(server_sockfd, 5);
	if (state == -1) {
		sleep(5);
		goto Socket_TCP_Loop;
	}

	client_sockfd = server_sockfd;
	maxi = -1;
	maxfd = server_sockfd;

	for (i = 0; i < FD_GW_USESIZE; i++) {
		client[i] = -1;
	}
	FD_ZERO(&readfds); //FD_SET 초기화
	FD_SET(server_sockfd, &readfds); //해당 파일디스크립터 fd를 1로 Set

	PP("--------- Thread_TCP_Server Start --------\n");

	while(1)
	{
		tempfds = readfds;

		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		fd_num = select(maxfd+1 , &tempfds, (fd_set *)0, (fd_set *)0, &timeout);
		if (fd_num == -1) {
			perror("select() error");
		}

		if (FD_ISSET(server_sockfd, &tempfds)) {//해당 파일디스크립터  fd가  1인지 확인
			client_len = sizeof(clientaddr);
			client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr, &client_len);
			for (i = 0; i < FD_GW_USESIZE; i++) {
				if (client[i] < 0) {
					client[i] = client_sockfd;
					break;
				}
			}
			FD_SET(client_sockfd,&readfds); //해당 파일디스크립터  fd를  1로 Set
			if (client_sockfd > maxfd) {
				maxfd = client_sockfd;
			}
			if (i > maxi) {
				maxi = i;
			}
			if (--fd_num <= 0) {
				continue;
			}
		}

		for (i = 0; i <= maxi; i++) 
		{
			if ((sockfd = client[i]) < 0) {
				continue;
			}
			if (FD_ISSET(sockfd, &tempfds)) {
				memset(&rcvBuf, 0x00, sizeof(rcvBuf));
				if ((fp=fdopen(sockfd, "r")) == NULL) {
					PP("[Thread_TCP_Server] TCP_SOCKET FD_OPEN Error \n");
					close(sockfd);
					FD_CLR(sockfd, &readfds);
					client[i] = -1;
				}
				else {
					ret = ioctl(sockfd, FIONREAD, &nread);
					if (nread==0) {
						fclose(fp);
						close(sockfd);
						FD_CLR(sockfd, &readfds);
						client[i] = -1;
					}
					else {
						len = recv(sockfd, rcvBuf, nread, 0);
						if (len >0)
						{
							rem_size = nread;
							offset = 0;
							ret = pd_Parse_RcvData(rcvBuf, rem_size);

							sleep(3);
							memset(msg, 0, sizeof(msg));
							snprintf(msg, sizeof(msg), "sever-send-seq=%d", ++seq);
							pd_Send_TcpData(sockfd, msg, strlen(msg));
						}
					}
				}
				if (--fd_num <= 0)
					break;
			}
		}
	}
	PP("--------- Thread_TCP_Server End --------\n");
	close (server_sockfd);
}


/**
 * Main program
 */
int main(int argc, char **argv) 
{
	pthread_t pt[5];
	int p = 0;
	char *str;
	char cfgName[64]= {0, };

	memset(&gl_base, 0, sizeof(gl_base));

	if (argc<4) {
		PP("Usage : \n");
		PP("psim [s or c] [t or u] [port]\n");
		PP("ex : psim s t 10021\n");
		exit(1);
	}

	PP("%s %s %s \n", argv[1], argv[2], argv[3]);

	str = strchr(argv[1], 's');
	if (str != NULL) {
		gl_base.sim_type = SIM_SERVER;
	}
	str = strchr(argv[1], 'c');
	if (str != NULL) {
		gl_base.sim_type = SIM_CLIENT;
	}
	str = strchr(argv[2], 't');
	if (str != NULL) {
		gl_base.sim_proto= SIM_TCP;
	}
	str = strchr(argv[2], 'u');
	if (str != NULL) {
		gl_base.sim_proto= SIM_UDP;
	}
	PP("gl_base.sim_type [%d] gl_base.sim_proto [%d] \n", gl_base.sim_type, gl_base.sim_proto);
	
	if (strlen(argv[3])>0) {
		gl_base.sim_port = atoi(argv[3]);
		PP("gl_base.sim_port = %d \n", gl_base.sim_port);
	}
	if (argc == 5) {
		if (strlen(argv[4])>0) {
			memcpy(gl_base.dst_ip, argv[4], strlen(argv[4]));
			PP("gl_base.dst_ip = %s \n", gl_base.dst_ip);
		}
	}
	//CFG_Init(cfgName); // read cfg.file

	
	switch(gl_base.sim_type) {
		case SIM_SERVER:
			if (gl_base.sim_proto == SIM_TCP) {
				if (pthread_create(&pt[p++], NULL, TH_TCP_SERVER, NULL) < 0) {
					PP("[main] TH_TCP_SERVER create failed\n");
				}
			}
			else {
				if (pthread_create(&pt[p++], NULL, TH_UDP_SERVER, NULL) < 0) {
					PP("[main] TH_UDP_SERVER create failed\n");
				}
			}
		break;
		case SIM_CLIENT:
			if (gl_base.sim_proto == SIM_TCP) {
				if (pthread_create(&pt[p++], NULL, TH_TCP_CLIENT, NULL) < 0) {
					PP("[main] TH_TCP_CLIENT create failed\n");
				}
			}
			else {
				if (pthread_create(&pt[p++], NULL, TH_UDP_CLIENT, NULL) < 0) {
					PP("[main] TH_UDP_CLIENT create failed\n");
				}
			}
		break;
	}

	while(1)
	{
		sleep(1);
	}
}
