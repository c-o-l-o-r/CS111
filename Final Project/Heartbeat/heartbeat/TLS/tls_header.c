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
	if(fp == NULL)
	{
		fprintf(stderr, "failed to open output file. exiting.\n");
		exit(1);
	}
	if(getline(&line, &len, fp) > 0)
	{
		strcpy(rv, line);
		for(i=0; i < strlen(rv); i++)
		{
			if(rv[i] == '\n') rv[i]='\0';
		}
	}
	else
	{
		fprintf(stderr, "No IP address detected. Exiting.\n");
		exit(1);
	}

	fclose(fp);
	system("rm ip_addr.txt");
	return rv;
}

int open_port(const char *server_name, int server_port)
{
    int server;
    struct hostent *server_ent;
    struct sockaddr_in socket_address;

    if ((server_ent = gethostbyname(server_name)) == NULL) {
	perror(server_name);
	abort();
    }
    server = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&socket_address, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(server_port);
    socket_address.sin_addr.s_addr = *(long *) (server_ent->h_addr);
    if (connect
	    (server, (struct sockaddr *) &socket_address,
	     sizeof(socket_address)) != 0) {
	close(server);
	perror(server_name);
	abort();
    }
    return server;
}

SSL_CTX *initialize_client_CTX(void)
{
    const SSL_METHOD *client_method;
    SSL_CTX *ctx_method;

    /*
     * Configure OpenSSL algorithms, error handlers, server methods, and context
     */

    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    client_method = SSLv3_client_method();
    ctx_method = SSL_CTX_new(client_method);
    if (ctx_method == NULL) {
	ERR_print_errors_fp(stderr);
	abort();
    }
    return ctx_method;
}


void display_server_certificate(SSL * ssl)
{
    X509 *x509_certificate;
    char *identifier;

    /*
     * 	Acquire assigned certificate 
     */
    x509_certificate = SSL_get_peer_certificate(ssl);

    /*
     * 	Create certificate descriptor message
     */

    if (x509_certificate != NULL) {
	/*
	 * 	Acquire and display server X509 Certificate
	 */
	identifier =
	    X509_NAME_oneline(X509_get_subject_name
		    (x509_certificate), 0, 0);
	printf("Server Certificate Descriptor: %s\n", identifier);
	free(identifier);
	X509_free(x509_certificate);
    } else
	printf("ALERT:Certificate not supplied by server");
}