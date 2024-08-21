#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FNLEN 500 					// file name max length;

void note(int, int, char *);
extern int QSTART;
extern FILE *fp;

/* init: to check option, parameter and open file; */
int init(int argc, char *argv[])
{
	int c, openfile(char *fn);
	
	if (argc == 1)
	{
		note(0, 0, NULL);			// show brief help information;
		return -1;
	}
	opterr = 0;					// let getopt() not print error information;
	while ((c = getopt(argc, argv, "q:r::h")) != -1)
	{
		switch (c)
		{
		case 'h':
			if (argc == 2)
			{
				note(0, 1, NULL);	// show complete help information
				return -1;
			}else
			{
				note(1, 'h', NULL);	// opt-param error: too many param for -h;	
				return 1;
			}
			break;
		case 'q':			
			QSTART = (int)atol(optarg);	
			break;	
		default:
			note(1, optopt, NULL);		// opt-param error;
							// 1. the option is not included; 2. the option included doesn't have param that in need;
			return 1;
			break;
		}
	}
	if (QSTART == 0)
	{
		note(1, 'q', NULL);
		return 1;
	}
	if ((argc - 1) != optind)			// only and must be one single param for input-file name;			
	{
		note(2, 0, NULL);			// lack of input-file param or too many files;
		return 2;
	}
	if (openfile(argv[optind]))
	{
		return 3;
	}
	return 0;
}

/* openfile: to open input file; */
int openfile(char *fn)
{
	char gzfn[FNLEN + 10] = "gzip -dc ";
	char *p = strrchr(fn, '.');		// return the last pointer of "." in filename;
	char suffix[10] = {"\0"};
	int i;

	if (p != NULL)
	{
		for (i = 0; *(++p) != '\0'; i++)
		{
			suffix[i] = *p;
		}
		if (strcmp(suffix, "gz") == 0) 
		{
			strcat(gzfn, fn);
			if ((fp = popen(gzfn, "r")) == NULL)
			{					
				note(3, 0, fn);		// open failed;
				return 1;
			}	
			return 0;
	
		}else if (strcmp(suffix, "fq") == 0 || strcmp(suffix, "fastq") == 0)
		{
			if ((fp = fopen(fn, "r")) == NULL)
			{					
				note(3, 0, fn);		
				return 1;
			}
			return  0;
		}
	}
				// file formate error;
	note(3, 1, fn);			
	return 3;
}
