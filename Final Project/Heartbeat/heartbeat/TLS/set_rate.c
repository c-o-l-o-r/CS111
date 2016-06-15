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

int main(int argc, char **argv)
{
    int port, range, rate;
    int server;
    int receive_length, line_length;
    char ip_addr[BUFSIZE];
	char *my_ip_addr;
    char *line = NULL;
    char buf[BUFSIZE];
    double heart_rate;
    FILE *fp = NULL;
    SSL *ssl;
    SSL_CTX *ctx;
	
	my_ip_addr = get_ip_addr();
    printf("My ip addr is: %s\n", my_ip_addr);

    /* READING COMMAND LINE ARGUMENTS */
    if(argc != 2)
    {
	fprintf(stderr, "Error. Usage: ./set_rate <rate>. Exiting.\n");
	exit(1);
    }
    rate = atoi(argv[1]);
    if(rate < 1 || rate > 5)
    {
	fprintf(stderr, "Error. Rate must be between 0 and 5. You supplied %d. Exiting.\n", rate);
	exit(1);
    }
    /* FINISHED READING COMMAND LINE ARGUMENTS */

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
	else{
	    fprintf(stderr, "Unrecognized line found: %s. Ignoring.\n", line);
	}
    }
    fclose(fp);
    /* FINISH READING INPUT FILE */

    printf("Connecting to: %s:%d\n", ip_addr, port);

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
	display_server_certificate(ssl);
    }

    /* SEND SAMPLE RATE TO SERVER */
    memset(buf,0,sizeof(buf)); //clear out the buffer
    sprintf(buf, "IP: %s new_rate: %d", my_ip_addr, rate);
    printf("Sending message '%s' to server...\n", buf);
    SSL_write(ssl, buf, strlen(buf));

    /* PRINT RESPONSE FROM SERVER */
    memset(buf, 0, sizeof(buf));
    SSL_read(ssl, buf, sizeof(buf));
    printf("Received message '%s' from server\n", buf);
    /* FINISH PRINTING RESPONSE RATE TO SERVER */

    //clean up operations
    SSL_free(ssl);
    close(server);
    SSL_CTX_free(ctx);
    return 0;
}
