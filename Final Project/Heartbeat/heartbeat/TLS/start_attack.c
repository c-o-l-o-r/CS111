
/*
 * UDP client
 * 
 * Author: BANSAL, Pranshu
 *         pban1993@gmail.com
 *
 */
#include "tls_header.h"

int main(int argc, char **argv)
{
    FILE *fp;
    char *line = NULL;
    char ip_addr[BUFSIZE];
	char *my_ip_addr;
    char buf[BUFSIZE];
    int port, range, rate;
    int line_length, message_size;
    int sockfd;
    double heart_rate;
    struct sockaddr_in server_addr;
    struct hostent *server;
	
	my_ip_addr = get_ip_addr();
    printf("My ip addr is: %s\n", my_ip_addr);

    /* READ INPUT FILE */
    fp = fopen("config_file", "r");
    if(fp == NULL){
	fprintf(stderr, "Error opening config file with name 'config_file'. Exiting.\n");
	exit(1);
    }
    printf("Reading input file...\n");
    while(getline(&line, &line_length, fp) > 0){
	if(strstr(line, "host_ip") != NULL){
	    sscanf(line, "host_ip: %s\n", ip_addr);
	}
	else if(strstr(line, "port") != NULL){
	    sscanf(line, "port: %d\n", &port);
	}
	else if(strstr(line, "range") != NULL){
	    sscanf(line, "range: %d\n", &range);
	}
	else if(strstr(line, "rate") != NULL){
	    sscanf(line, "rate: %d\n", &rate);
	}
	else{
	    fprintf(stderr, "Unrecognized line found: %s. Ignoring.\n", line);
	}
    }
    fclose(fp);
    /* FINISH READING INPUT FILE */

    printf("Connecting to: %s:%d with rate %d and range %d\n", ip_addr, port, rate, range);

    /* SETUP SOCKET COMMUNICATIONS */
    server = gethostbyname(ip_addr);
    socklen_t len = sizeof(server_addr);	

    //Set up datagram socket communication with ARPA Internet protocol 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero((char *)&server_addr, sizeof(server_addr));	
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_addr);
    server_addr.sin_port = htons(port);
    /* FINISH SETUP SOCKET COMMUNICATIONS */

    /* SEND HEART RATE TO SERVER */
    sprintf(buf, "IP: %s new_rate: %d", my_ip_addr, 15);
    printf("Sending message '%s' to server...\n", buf);

    if (sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&server_addr, len) < 0) {
	fprintf(stderr, "Failed to send.\n");
	exit (1);
    }

    return 0;
} 
