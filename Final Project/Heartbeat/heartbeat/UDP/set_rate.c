
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
	double heart_rate;
	struct sockaddr_in server_addr;
	struct hostent *server;

	my_ip_addr = get_ip_addr();
	printf("My ip addr is: %s\n", my_ip_addr);

	/* READING COMMAND LINE ARGUMENTS */
	if (argc != 2) {
		fprintf(stderr, "Error. Usage: ./set_rate <rate>. Exiting.\n");
		exit(1);
	}
	
	rate = atoi(argv[1]);
	if (rate < 1 || rate > 5) {
		fprintf(stderr, "Error. Rate must be between 0 and 5. You supplied %d. Exiting.\n", rate);
		exit(1);
	}
	/* FINISHED READING COMMAND LINE ARGUMENTS */


	/* READ INPUT FILE */
	fp = fopen("config_file", "r");
	if (fp == NULL) {
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

	printf("Connecting to: %s:%d\n", ip_addr, port, rate, range);

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
	sprintf(buf, "IP: %s new_rate: %d", my_ip_addr, rate);
	printf("Sending message '%s' to server...\n", buf);

	if (sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&server_addr, len) < 0) {
		fprintf(stderr, "Failed to send.\n");
		exit (1);
	}

	return 0;
} 
