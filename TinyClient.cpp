#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <netdb.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <map>

#define SOCKET int
#define SOCK_ERR -1

class HTTPClient
{
public:
	char * host;
	unsigned short port;
	SOCKET sock ;
	SOCKET client_connect(char * host,char*sport,unsigned short port)
	{
		struct addrinfo hints;
		struct addrinfo *result, *rp;
		SOCKET sfd;
		this->host = host;
		this->port = port;
		int s = 0;
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_protocol = 0;
		hints.ai_addr = NULL;
		hints.ai_next = NULL;
		s = getaddrinfo(host,sport,&hints,&result);
		if(s != 0)
		{
			fprintf(stderr,"getaddrinfo error <%s:%s> %s\n",
					host,sport,gai_strerror(s));
			return SOCK_ERR;
		}
		for(rp = result;rp != NULL;rp = rp->ai_next)
		{
			sfd = socket(rp->ai_family,SOCK_STREAM,rp->ai_protocol);
			if(sfd == -1)
			{
				continue;
			}
			if(connect(sfd,rp->ai_addr,rp->ai_addrlen) != -1)
			{
				this->sock = sfd;
				break;
			}
			close(sfd);
		}
		if(rp == NULL)
		{
			fprintf(stderr,"could not connect <%s> <%s>\n",host,sport);
			return SOCK_ERR;
		}
		return sfd;
	}
} ;
int client_put(char*param,char * host,char*sport,unsigned short port)
{
	HTTPClient client;
	char * request = "GET ";
}

int client_post(char*param,char * host,char*sport,unsigned short port)
{
	HTTPClient client;
	char * request = "GET ";
}
int client_get(char*param,char * host,char*sport,unsigned short port)
{
	HTTPClient client;
	char * request = "GET ";
	char * proto = " HTTP/1.1\r\nConnection: keep-alive\r\n"
						"User-Agent: Client.cpp\r\n"
						"Accept: text/html\r\n";
	char * format = "%s%s%s%s%s%s";
	char * reqHost = "Host: ";
	char * rest = "\r\n\r\n" ;
	int len = strlen(request) + strlen(param) + strlen(proto) + strlen(reqHost)+
				strlen(host) + strlen(rest);
	char * req = new char[ len  + 1];
	snprintf(req,len+1,format,request,param,proto,reqHost,host,rest);
	SOCKET acc = client.client_connect(host,sport,port);
	if(acc == SOCK_ERR)
		return -1;
	int ret = write(acc,req,len);
	if(ret != len || ret == SOCK_ERR)
	{
		return -1;
	}
	int bufsize = 4096;
	char * buffer = new char[ bufsize + 1];
	FILE * fp = fopen("response.bin","ab");
	
	do
	{
		ret = read(acc,buffer,bufsize);
		if(ret == -1 )
		{
			break;
		}
		if(fp)
		{
			fwrite(buffer,ret,1,fp);
		}
		buffer[ret] = 0 ;
		printf("%s\n",buffer);
	}
	while(ret == bufsize);
	fclose(fp);
	close(acc);
	return 0;
}


	int main(int argc,char**argv)
	{
		int port = 80;
		int iport = 0;
		char * host = NULL;
		char * mode = NULL;
		char * sport = NULL;
		char * param = NULL;
		if(argc < 2)
		{
			printf("client <mode> <host> <port>  <path>\n");
			exit(0);
		}
		while(--argc)
		{
			if(mode == NULL)
			{
				mode = *++argv;
			}
			else if(host == NULL)
			{
				host = *++argv;
			}
			else if(sport == NULL)
			{
				sport = *++argv;
				if(sscanf(sport,"%d",&iport) == 1)
				{
					port = iport;
				}
			}
			else if(mode != NULL && param  == NULL)
			{
				param = *++argv;
			}
			else
			{
				printf("client <mode> <host> <port>  <path>\n");
			}
		}
		
		if(!strcmp(mode,"GET"))
		{
			client_get(param,host,sport,port);
		}
		else if(!strcmp(mode,"PUT"))
		{
			client_put(param,host,sport,port);
		}
		else if(!strcmp(mode,"POST"))
		{
			client_post(param,host,sport,port);
		}

	}
