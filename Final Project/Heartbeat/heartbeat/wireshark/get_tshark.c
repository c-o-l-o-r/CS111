
/*
 * UDP client
 * 
 * Author: In Hwan "Chris" Baek
 *         chris.inhwan.baek@gmail.com
 *
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUFSIZE 1024

char * 
get_ip_addr()
{
	FILE *fp;
	char *line = NULL;
	char *rv = malloc(256*sizeof(char));
	int len, i;
	system("configure_edison --showWiFiIP > ip_addr.txt");
	
	fp = NULL;
	fp = fopen("ip_addr.txt", "r");
	if (fp == NULL) {
		fprintf(stderr, "failed to open output file. exiting.\n");
		exit(1);
	}

	if (getline(&line, &len, fp) > 0) {
		strcpy(rv, line);
		for (i=0; i < strlen(rv); i++) {
			if (rv[i] == '\n') rv[i]='\0';
		}
	} else {
		fprintf(stderr, "No IP address detected. Exiting.\n");
		exit(1);
	}

	fclose(fp);
	system("rm ip_addr.txt");
	return rv;
}


int 
main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buf[BUFSIZE];
    char ip_addr[BUFSIZE];
	char *my_ip_addr;
    char *line;
    int port, examine_port;
    int line_length;
    int message_size;
    FILE *fp;

    my_ip_addr = get_ip_addr();
    printf("My ip addr is: %s\n", my_ip_addr);
	
    /* READ COMMANDLINE ARGS */
    if (argc != 2) {
		fprintf(stderr, "Error, exiting. Usage: ./get_tshark <port_to_examine>\n");
		exit(1);
    }
    
    examine_port = atoi(argv[1]);
    /* FINISH READ COMMANDLINE ARGS*/


    /* READ INPUT FILE */
    fp = fopen("config_file", "r");
    if (fp == NULL){
		fprintf(stderr, "Error opening config file with name 'config_file'. Exiting.\n");
		exit(1);
    }
    
    printf("Reading input file...\n");
    
    while (getline(&line, &line_length, fp) > 0) {
		if (strstr(line, "host_ip") != NULL) {
	    	sscanf(line, "host_ip: %s\n", ip_addr);
		} else if (strstr(line, "port") != NULL) {
	    	sscanf(line, "port: %d\n", &port);
		}
    }
    fclose(fp);
    /* FINISH READING INPUT FILE */

    printf("Connecting to: %s:%d\n", ip_addr, port);	

    /* SETUP SOCKET COMMS */

    server = gethostbyname(ip_addr);
    socklen_t len = sizeof(server_addr);

    //Set up datagram socket communication with ARPA Internet protocol 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero((char *)&server_addr, sizeof(server_addr));	
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_addr);
    server_addr.sin_port = htons(port);	

    /* FINISH SETUP SOCKET COMMS */
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "ip_addr: %s examine_port: %d", my_ip_addr, examine_port);
    printf("sending message '%s' to get_tshark server.\n", buf);
    
    if (sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&server_addr, len) < 0) {
		fprintf(stderr, "Failed to send.\n");
		exit (0);
    } 

    printf("The server will send a response in approximately 1 minute.\n");

    memset(buf, 0, sizeof(buf));
    if (recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&server_addr, &len) < 0) {			
		fprintf(stderr, "Failed to receive.\n");
		exit (0);	
    }

    printf("Received from server: %s\n", buf);

    while(1) {
		memset(buf,0,sizeof(buf));
		if (recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&server_addr, &len) < 0) {			
	    	fprintf(stderr, "Failed to receive.\n");
	    	exit (0);	
		}

		message_size = strlen(buf);
		printf("Received %i bytes from server: %s\n", message_size, buf);
		
		if (!strcmp(buf, "stop")) break;
    }
    
    close(sockfd);
    return 0;
} 
