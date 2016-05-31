#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

void sighandler(int);

int main(int argc, char **argv)
{
	int c;
	int ifd;
	int ofd;
	char* something;
		
	// s_and_c options: 0, no s or c, 1: s only, 2: c only, 3: s and c
	int s_and_c = 0;
	
	while (1)
    {
		static struct option long_options[] =
        {
			{"input", required_argument, 0, 'i'},
			{"output",  required_argument, 0, 'o'},
			{"segfault",  no_argument, 0, 's'},
			{"catch",  no_argument, 0, 'c'},
			{0, 0, 0, 0}
        };
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "i:o:sc",
                       long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1) break;
		
		switch (c)
        {
		case 'i':
			// Input redirection
			ifd = open(optarg, O_RDONLY);
			if (ifd >= 0) 
			{
				close(0);
				dup(ifd);
				close(ifd);
			}
			break;

        case 'o':
  			// Output redirection
			ofd = creat(optarg, 0666);
			if (ofd >= 0) 
			{
				close(1);
				dup(ofd);
				close(ofd);
			}
			break;

		case 'c':
			s_and_c+=2;
			break;
			
        case 's':
			s_and_c+=1;
			break;
		
	default:
		exit(1);
	}
    }

	// Handle s and c options
	switch (s_and_c)
	{
	case 1:
		// Force segmentation fault
		something = NULL;
		something[0] = 'a';
		break;
	case 2:
		// Catch
		signal(SIGSEGV, sighandler);
		break;
	case 3:
		signal(SIGSEGV, sighandler);
		something = NULL;
		something[0] = 'a';
		break;
	}
       
    char buffer[1];
    while (read(0, buffer, 1)) 
    {
        write(1, buffer, 1);
    }
    
    exit(0);
}

void sighandler(int signum)
{
	fprintf(stderr, "Caught a signal, exit\n");
	exit(3);
}
