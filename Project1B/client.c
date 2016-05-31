#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <mcrypt.h>
extern int errno;
#define BUFF_SIZE 256
int sockfd, logfd;
int flag_log=0, flag_encript=0;

struct termios saved_attributes;

void reset_input_mode (void)
{
	tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}
void signal_handler(int signal)
{
	if(signal == SIGPIPE)
	{
		fprintf(stdout,"Program terminated due to SIGPIPE Signal");
		exit(2);
	}
	if(signal == SIGINT)
	{
		fprintf(stdout,"Program terminated due to SIGINT Signal\n");
		exit(-4);
	}
	printf("%s:%d\n","Signal Received",signal);
}

void set_input_mode (void);
void read_input(char*);
void *thread1(void*);
void *thread2(void*);
void error(char *msg)
{
    fprintf(stderr,"%s",msg);
    exit(-1);
}
void get_key(char* key)
{
	FILE *fp = fopen("my.key", "r");
	if (fp == NULL) error("Error opening my.key");
	size_t newLen = fread(key, sizeof(char), 99, fp);
	if (newLen == 0) 
	{
		error("Error reading my.key");
	} 
	else 
	{
		key[newLen++] = '\0'; 
	}
	fclose(fp);
}
int main(int argc, char **argv)
{
	// parse options
	int c;
	int port_number=8134;
	
	while (1)
    {
		static struct option long_options[] =
        {
			{"port", required_argument, 0, 'p'},
			{"log", required_argument, 0, 'l'},
			{"encrypt", no_argument, 0, 'e'},
			{0, 0, 0, 0}
        };
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "p:l:e",
                       long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1) break;
		
		switch (c)
        {
		case 'p':
			port_number = atoi(optarg);
			break;
		case 'l':
			logfd = creat(optarg, 0666);
			if (logfd < 0) error("Error creating log file\n");
			flag_log = 1;
			break;
		case 'e':
			flag_encript = 1;
			break;
		default:
			error("Error parsing options\n");
		}
	}
	
	int n;

    struct sockaddr_in serv_addr;
    struct hostent *server;
	
	
	// Open socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket\n");
	
	// Connect to server
    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
	
	// Format server address
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port_number);
	
	// Connect
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting\n");
	
	// 2 threads
	set_input_mode();
	pthread_t t1,t2;

	pthread_create(&t1,NULL,thread1,NULL);
	pthread_create(&t2,NULL,thread2,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	exit(0);
}

void *thread1(void * arg)
{
	signal(SIGINT,&signal_handler);
	signal(SIGPIPE,&signal_handler);
	char *buffer=(char*)malloc(BUFF_SIZE*sizeof(char));
	
	// Obtain key from file
	char key[100];
	if (flag_encript)
	{
		get_key(key);
    }
	while(1)
	{
		bzero(buffer,BUFF_SIZE);
		read_input(buffer);
		
		// Encript 
		if (flag_encript)
		{
			MCRYPT td;
			td = mcrypt_module_open("blowfish", NULL, "ecb", NULL);
			mcrypt_generic_init(td, key, strlen(key), NULL);
			mcrypt_generic(td, buffer, strlen(buffer));
			mcrypt_generic_deinit(td);
			mcrypt_module_close(td);
		}
		
		// Log
		if (flag_log)
		{
			char buffer_temp[BUFF_SIZE+100];
			sprintf(buffer_temp, "SENT %d bytes: %s\n", strlen(buffer), buffer);
			write(logfd, buffer_temp, strlen(buffer_temp));
		}
		
		// Write to socket
		if(write(sockfd,buffer,strlen(buffer)) < 0)
		{
			fprintf(stderr,"Error writing to socket");
			exit(-2);
		}
		sleep(1);
	}
	return 0;
}
void *thread2(void *arg)
{
	char *buffer=(char*)malloc(BUFF_SIZE*sizeof(char));
	
	// Obtain key from file
	char key[100];
	if (flag_encript)
	{
		get_key(key);
    }
	while (1)
	{
		bzero(buffer,BUFF_SIZE);
		if(read(sockfd,buffer,BUFF_SIZE) <= 0)
		{
			fprintf(stderr,"EOF Received from server\n");
			close(sockfd);
			exit(1);
		}
		if(buffer == NULL)
		{
			perror("Buffer null");
			exit(-4);
		}
		
		// Log
		if (flag_log)
		{
			char buffer_temp[BUFF_SIZE+100];
			sprintf(buffer_temp, "RECEIVED %d bytes: %s\n", strlen(buffer), buffer);
			write(logfd, buffer_temp, strlen(buffer_temp));
		}
		
		// Decript 
		if (flag_encript)
		{
			MCRYPT td;
			td = mcrypt_module_open("blowfish", NULL, "ecb", NULL);
			mcrypt_generic_init(td, key, strlen(key), NULL);
			mdecrypt_generic(td, buffer, strlen(buffer));
			mcrypt_module_close(td);
		}
		
		// Print out
		printf("%s\n",buffer);
	}
    return 0;
	
}

void read_input(char *buff)
{
	fflush(NULL);
	char c;	
	int i=0;
	while (i<BUFF_SIZE-2)
	{
		read(STDIN_FILENO, &c, 1);
		if (c == '\004')          /* C-d */
		{ 
			printf("%s\n","^D Received");
			close(sockfd);
			exit(0);
		}
		if(c==10 || c==13)
		{
			c=13;
			write(1,&c,1);
			c=10;
			write(1,&c,1);
			buff[i++]=10;
			break;
		}
		write(1,&c,1);
		buff[i++]=c;

	}

}

void set_input_mode (void)
{
	struct termios tattr;
	char *name;

	/* Make sure stdin is a terminal. */
	if (!isatty (STDIN_FILENO))
	{
		fprintf (stderr, "Not a terminal.\n");
		exit (EXIT_FAILURE);
	}

	/* Save the terminal attributes so we can restore them later. */
	tcgetattr (STDIN_FILENO, &saved_attributes);
	atexit (reset_input_mode);

	/* Set the funny terminal modes. */
	tcgetattr (STDIN_FILENO, &tattr);
	tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
	tattr.c_cc[VMIN] = 1;
	tattr.c_cc[VTIME] = 0;
	tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

