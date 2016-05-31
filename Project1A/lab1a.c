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


extern int errno;
#define BUFF_SIZE 1000
#define SIZE 1024
struct termios saved_attributes;
int fd1[2],fd2[2];
pid_t cpid;
void exit_report(int status);
void exit_report_kill(int status, int sig);
void reset_input_mode (void);
void set_input_mode (void);
void read_input(char*);
void read_input_shell(char*);
void display(char *buff);
void *thread1(void*);
void *thread2(void*);
void createPipes();
void createShell();
int flag_shell = 0;
int flag_shell_eof_disable = 0;

void signal_handler(int signal)
{
	if(signal == SIGPIPE)
	{
		fprintf(stdout,"Program terminated due to SIGPIPE Signal");
		flag_shell_eof_disable=1;
		exit_report_kill(2,SIGPIPE);
	}
	if(signal == SIGINT)
	{
		fprintf(stdout,"Program terminated due to SIGINT Signal\n");
		exit_report_kill(2,SIGINT);
	}
	printf("%s:%d\n","Signal Received",signal);
}

int main(int argc, char **argv)
{
	// parse options
	int c;
	while (1)
    {
		static struct option long_options[] =
        {
			{"shell", no_argument, 0, 's'},
			{0, 0, 0, 0}
        };
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "s",
                       long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1) break;
		
		switch (c)
        {
		case 's':
			flag_shell=1;
			break;
		default:
			fprintf(stderr, "Error parsing options\n");
			exit(1);
		}
	}
	
	// Handle for each option
	if (flag_shell)
	{
		// -shell options
		
		createPipes();
		cpid = fork();
		set_input_mode();
		if (cpid == 0) 
		{
			createShell();
		}
		//parent code will read from fd2 and write into fd1
		else 
		{
			close(fd1[0]);
			close(fd2[1]);
			pthread_t t1,t2;
			pthread_create(&t1,NULL,thread1,NULL);
			pthread_create(&t2,NULL,thread2,NULL);
			pthread_join(t1,NULL);
			pthread_join(t2,NULL);
			exit_report(0);
		}
	}
	else
	{
		// no options
		char* buff=(char*)malloc(BUFF_SIZE*sizeof(char));
		if (buff==NULL)
		{
			fprintf(stderr, "malloc failed\n");
			exit(-1);
		}
		
		set_input_mode ();
		while(1)
		{

			read_input(buff);
			display(buff);
		}
		return EXIT_SUCCESS;
	}
}

// Exit and report child status
void exit_report(int status)
{
	int child_status;
	int wc = wait(&child_status);
	fprintf(stderr,"child exited with status %d\n",child_status);
	exit(status);
}

// Exit and report child status. Also kill child process immediately
void exit_report_kill(int status, int sig)
{
	kill(cpid, sig);
	int child_status;
	int wc = wait(&child_status);
	fprintf(stderr,"child exited with status %d\n",status);
	exit(status);
}

void *thread1(void * arg)
{
	signal(SIGINT,&signal_handler);
	signal(SIGPIPE,&signal_handler);

	char *buff=(char*)malloc(BUFF_SIZE*sizeof(char));
	int i=0;
	while(1)
	{
		memset(buff,'\0',SIZE);
		read_input_shell(buff);
		if(write(fd1[1],buff,SIZE) < 0)
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
	char *buff=(char*)malloc(SIZE*sizeof(char));
	
	while(1)
	{
		memset(buff,'\0',SIZE);
		if(read(fd2[0],buff,SIZE) <= 0)
		{
			if (!flag_shell_eof_disable)
			{
				fprintf(stderr,"EOF Received from Shell\n");
				exit(1);
			}
		}
		if(buff == NULL)
		{
			exit(-4);
		}
		printf("%s",buff);
	}
}

void read_input(char *buff)
{
	char c;
	int i=0;
	int mem=BUFF_SIZE;
	while(1)
	{
		// Exceed buffer size, realloc
		if (i>=mem-3)
		{
			mem*=2;
			buff=(char*)realloc(buff,mem*sizeof(char));
			if (buff==NULL)
			{
				fprintf(stderr, "realloc failed\n");
				exit(-1);
			}
		}
		
		read (STDIN_FILENO, &c, 1);
		if (c == '\004')          /* C-d */
		{
			display(buff);
			exit(EXIT_SUCCESS);
		}
		else
		{
			buff[i++]=c;
			if(c==10 || c==13)
			{
				if(i<BUFF_SIZE-1)
				{
					buff[--i]=13;
					buff[i++]=10;
				}
			}
		}

	}
}

void read_input_shell(char *buff)
{
	fflush(NULL);
	char c;	
	int i=0;
	int mem=BUFF_SIZE;
	while(1)
	{
		// Exceed buffer size, realloc
		if (i>=mem-3)
		{
			mem*=2;
			buff=(char*)realloc(buff,mem*sizeof(char));
			if (buff==NULL)
			{
				fprintf(stderr, "realloc failed\n");
				exit(-1);
			}
		}
		
		read (STDIN_FILENO, &c, 1);
		if (c == '\004')          /* C-d */
		{ 
			printf("%s\n","^D Received");
			flag_shell_eof_disable=1;
			exit_report_kill(0,SIGHUP);
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

void display(char *buff)
{
	int i=0;
	while(buff[i]!='\0')
	{
		printf("%c",buff[i]);
		i++;
	}
	printf("\n");
}

void set_input_mode (void)
{
	struct termios tattr;
	/* Make sure stdin is a terminal. */
	if (!isatty (STDIN_FILENO))
	{
		fprintf (stderr, "Not a terminal.\n");
		exit(EXIT_FAILURE);
	}

	/* Save the terminal attributes to restore them later. */
	tcgetattr (STDIN_FILENO, &saved_attributes);
	atexit (reset_input_mode);

	tcgetattr (STDIN_FILENO, &tattr);
	tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
	tattr.c_cc[VMIN] = 1;
	tattr.c_cc[VTIME] = 0;
	tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

void reset_input_mode (void)
{
	tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
	
	if (flag_shell)
	{
		close(fd1[0]);
		close(fd2[1]);
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
	execl("/bin/sh","/bin/bash",(char*)0);
}

