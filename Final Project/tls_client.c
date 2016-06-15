#define _GNU_SOURCE
/*	
 *	Demonstration TLS client
 *
 *       Compile with
 *
 *       gcc -Wall -o tls_client tls_client.c -L/usr/lib -lssl -lcrypto
 *
 *       Execute with
 *
 *       ./tls_client <server_INET_address> <port> <client message string>
 *
 *       Generate certificate with 
 *
 *       openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout tls_demonstration_cert.pem -out tls_demonstration_cert.pem
 *
 *	 Developed for Intel Edison IoT Curriculum by UCLA WHI
 */
#include "tls_header.h"
#include <pthread.h>

int port, range, rate;
int receive_length, line_length;
double heart_rate;
char *my_ip_addr;
SSL *ssl;

FILE* fp_output;

pthread_mutex_t mutex;

void* worker_read(void* arg)
{
	char buf[BUFSIZE];
	
	while(1)
	{
		memset(buf,0,sizeof(buf));
		receive_length = SSL_read(ssl, buf, sizeof(buf));
		if(strstr(buf, "new_rate: ") != NULL)
		{
			sscanf(buf, "Heart rate of patient %*s is %*f new_rate: %d", &rate);
			rate = rate;
			printf("New rate %d received from server.\n", rate);
			fprintf(fp_output,"New rate %d received from server.\n", rate);
		}

		printf("Received message '%s' from server.\n\n", buf);
		fprintf(fp_output,"Received message '%s' from server.\n\n", buf);
		fflush(fp_output);
	}
}

int main(int args, char *argv[])
{
    int server;
    int receive_length, line_length;
    char ip_addr[BUFSIZE];
    char *line = NULL;
    char buf[BUFSIZE];
    FILE *fp = NULL;
    SSL_CTX *ctx;
	
	// Thread
	pthread_t* threads[2];
	if (pthread_mutex_init(&mutex, NULL) != 0)
	{
		printf("mutex init failed\n");
		exit(-1);
	}
	
	my_ip_addr = get_ip_addr();
    printf("My ip addr is: %s\n", my_ip_addr);

    /* READ INPUT FILE */
    fp = fopen("config_file", "r");
    if(fp == NULL){
	fprintf(stderr, "Error opening config file with name 'config_file'. Exiting.\n");
	exit(1);
    }
	fp_output = fopen("Lab4-E-2.txt", "w");
    if(fp_output == NULL){
	fprintf(stderr, "Error opening config file with name 'Lab4-E-2.txt'. Exiting.\n");
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

    printf("Connecting to: %s:%d\n", ip_addr, port);
	fprintf(fp_output,"Connecting to: %s:%d\n", ip_addr, port);
	
    /* SET UP TLS COMMUNICATION */
    SSL_library_init();
    ctx = initialize_client_CTX();
    server = open_port(ip_addr, port);
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server);
    /* FINISH SETUP OF TLS COMMUNICATION */

    /* SEND HEART RATE TO SERVER */
    if (SSL_connect(ssl) == -1){ //make sure connection is valid
	fprintf(stderr, "Error. TLS connection failure. Aborting.\n");
	ERR_print_errors_fp(stderr);
	exit(1);
    }
    else {
	printf("Client-Server connection complete with %s encryption\n", SSL_get_cipher(ssl));
	fprintf(fp_output, "Client-Server connection complete with %s encryption\n", SSL_get_cipher(ssl));
	fflush(fp_output);
	display_server_certificate(ssl);
    }

    // Thread
	pthread_create(&threads[0],NULL,worker_read,NULL);
	
	while(1)
	{
		printf("Current settings: rate: %d, range: %d\n", rate, range);
		heart_rate = 
			generate_random_number(AVERAGE_HEART_RATE-(double)range, AVERAGE_HEART_RATE+(double)range);
		memset(buf,0,sizeof(buf)); //clear out the buffer

		//populate the buffer with information about the ip address of the client and the heart rate
		sprintf(buf, "Heart rate of patient %s is %4.2f", my_ip_addr, heart_rate);
		printf("Sending message '%s' to server...\n", buf);
		fprintf(fp_output,"Sending message '%s' to server...\n", buf);
		fflush(fp_output);
		SSL_write(ssl, buf, strlen(buf));

		sleep(rate);
    }
	
	// Thread
	pthread_join(threads[0],NULL);
	
    /* FINISH HEART RATE TO SERVER */

    //clean up operations
    SSL_free(ssl);
    close(server);
    SSL_CTX_free(ctx);
    return 0;
}
