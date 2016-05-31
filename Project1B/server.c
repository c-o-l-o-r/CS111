#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <netinet/in.h>
#include <mcrypt.h>
extern int errno;
#define BUFF_SIZE 256
struct termios saved_attributes;
int fd1[2],fd2[2], sockfd, newsockfd;
pid_t cpid;
int flag_encript=0;

void reset_input_mode (void);
void set_input_mode (void);
void read_input(char*);
void display(char *buff);
void *thread1(void*);
void *thread2(void*);
void createPipes();
void createShell();
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
		kill(SIGINT,cpid);
		exit(-4);
	}
	printf("%s:%d\n","Signal Received",signal);
}

void error(char *msg)
{
    perror(msg);
    exit(0);
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
			{"encrypt", no_argument, 0, 'e'},
			{0, 0, 0, 0}
        };
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "p:e",
                       long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1) break;
		
		switch (c)
        {
		case 'p':
			port_number = atoi(optarg);
			break;
		case 'e':
			flag_encript = 1;
			break;
		default:
			fprintf(stderr, "Error parsing options\n");
			exit(1);
		}
	}
	
	int clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	
	// Socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	
	// Format server address
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port_number);
	
	// Bind
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
			  sizeof(serv_addr)) < 0) 
			  error("ERROR on binding");
	listen(sockfd,5);
	
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) 
		error("ERROR on accept");
	
	// Accept done, create shell
	createPipes();
	cpid = fork();

	if (cpid == 0) 
	{
		createShell();
	}
	//parent code will read from fd2 and write into fd1
	else 
	{
		close(fd1[0]);
		close(fd2[1]);
		int status;
		pthread_t t1,t2;

		pthread_create(&t1,NULL,thread1,NULL);
		pthread_create(&t2,NULL,thread2,NULL);
		pthread_join(t1,NULL);
		pthread_join(t2,NULL);
		printf("child exited with status %d\n",1);
		wait(&status);
		printf("child exited with status %d\n",status);
		exit(0);
	}
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
		
		// Read input from client
		if(read(newsockfd,buffer,BUFF_SIZE) <= 0)
		{
			fprintf(stderr,"EOF Received from client\n");
			close(sockfd);
			exit(1);
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
		
		if(write(fd1[1],buffer,BUFF_SIZE) < 0)
		{
			perror("Writing to Shell");
			return NULL;
			exit(-2);
		}
		sleep(1);
	}
}
void *thread2(void *arg)
{
	int i=0;
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
		if(read(fd2[0],buffer,BUFF_SIZE) <= 0)
		{
			fprintf(stderr,"EOF Received from Shell\n");
			exit(2);
		}
		if (buffer == NULL)
		{
			exit(-4);
		}
		
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
		
		// Writing to socket
		if(write(newsockfd,buffer,strlen(buffer)) < 0)
		{
			perror("Writing to socket");
			exit(-2);
		}
	}
}

void createPipes()
{
/* fd1 and fd2 are global variables */
	if( pipe(fd1) == -1 || pipe(fd2) == -1)
	{
		perror("Opening Pipe");
		exit(errno);
	}

}

void createShell()
{
	close(fd1[1]); //close write end
	close(fd2[0]); //close read end
	close(0);
	close(1);
	dup(fd1[0]);
	dup(fd2[1]);
	close(2);
	dup(fd2[1]);
	execl("/bin/sh","/bin/bash",(char*)0);
}
