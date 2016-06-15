
/*
 * UDP client
 * 
 * Author: BANSAL, Pranshu
 *         pban1993@gmail.com
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

double AVERAGE_HEART_RATE = 70.0;

double generate_random_number(double lower, double upper)
{
    double random = ( (double)rand() / (double)RAND_MAX );
    double diff = upper - lower;
    double r = random * diff;
    return lower + r;
}

char * get_ip_addr()
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
    int pid;
    double heart_rate;
    struct sockaddr_in server_addr;
    struct hostent *server;

    my_ip_addr = get_ip_addr();
    printf("My ip addr is: %s\n", my_ip_addr);

    /* READ INPUT FILE */
    fp = fopen("config_file", "r");
    if (fp == NULL){
		fprintf(stderr, "Error opening config file with name 'config_file'. Exiting.\n");
		exit(1);
    }

    printf("Reading input file...\n");
    while (getline(&line, &line_length, fp) > 0) {
		if (strstr(line, "host_ip") != NULL){
	    	sscanf(line, "host_ip: %s\n", ip_addr);
		} else if (strstr(line, "port") != NULL) {
	    	sscanf(line, "port: %d\n", &port);
		} else if (strstr(line, "range") != NULL) {
	    	sscanf(line, "range: %d\n", &range); 
		} else if (strstr(line, "rate") != NULL) {
	    	sscanf(line, "rate: %d\n", &rate);
		} else {
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
    while (1) {
		printf("Current settings: rate: %d, range: %d\n", rate, range);
		heart_rate = generate_random_number(AVERAGE_HEART_RATE-(double)range, 
					AVERAGE_HEART_RATE+(double)range);
		memset(buf,0,sizeof(buf)); //clear out the buffer

		//populate the buffer with information about the ip address of the client and the heart rate
		sprintf(buf, "Heart rate of patient %s is %4.2f", my_ip_addr, heart_rate);
		printf("Sending message '%s' to server...\n", buf);

		if (sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&server_addr, len) < 0) {
		    fprintf(stderr, "Failed to send.\n");
		    exit (1);
		}
		
		/* FINISH HEART RATE TO SERVER */
		memset(buf,0,sizeof(buf));
		if (recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&server_addr, &len) < 0) {			
		    fprintf(stderr, "Failed to receive.\n");
		    exit (0);	
		}
		
		if (strstr(buf, "new_rate: ") != NULL) {
		    sscanf(buf, "Heart rate of patient %*s is %*f new_rate: %d", &rate);
		    rate = rate;
		    printf("New rate %d received from server.\n", rate);
		}

		printf("Received message '%s' from server.\n\n", buf);
		sleep(rate);
    }

    close(sockfd);
    return 0;
} 
