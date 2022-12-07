#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_PORT 5100

int main(int argc, char **argv)
{
	int ssock;
	struct sockaddr_in servaddr;
	char mesg[BUFSIZ];
    int connect_fd;
    fd_set readfd;

	if(argc != 2) {
		printf("Usage : %s <IP address>\n", argv[0]);
		return -1;
	}

	if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) { 
		perror("socket()");
		return -1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &(servaddr.sin_addr.s_addr));
	servaddr.sin_port = htons(TCP_PORT);

	if(connect(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect()");
		return -1;
	}

    FD_ZERO(&readfd);
    while(1)
    {
            memset(mesg,0,BUFSIZ);
            FD_SET(0,&readfd);
            FD_SET(ssock,&readfd);

            select(ssock+1,&readfd,NULL,NULL,NULL);

            if(FD_ISSET(ssock,&readfd)) {
                    if((read(ssock,mesg, sizeof(mesg))) <=0) {
                            perror("recv()");
                            return -1;
                    //close(ssock);
                    }
                    printf("Received data : %s ", mesg);
            }

            if(FD_ISSET(0,&readfd)) {
                    fgets(mesg,BUFSIZ,stdin);
                    sendto(ssock, mesg,strlen(mesg),0, (struct sockaddr *)&servaddr, sizeof(servaddr));
                    }
    }


    shutdown(ssock, SHUT_WR);

	close(ssock);				

	return 0;
    
}
