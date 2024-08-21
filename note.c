#include <stdio.h>
#define PNAME	 "fqvalue"
#define PVERSION "2.4"

/*
 Function: note   
 Description: provide the help or error information 	  
 Called By: init, openfile, read_line, check_line 
 Input: int num: the class of different situation 
        int c: the section dividing the class, or value provied by other function;
        char *p: the detailed information or runtime error provied by other function;
*/
void note(int num, int c, char *p)
{
	switch (num)
	{
	case 0:							// help information;
		if (c == 0)					// brief;
		{
			fprintf(stderr, "#%s Usage: %s -q <int> <file: .fastq|.gz>\n", PVERSION, PNAME);
			fprintf(stderr, "Try '%s -h' for more information\n", PNAME);
		}else						// detailed;
		{
			fprintf(stderr, "<Name: %s> \n", PNAME);
			fprintf(stderr, "<Version: %s>\n", PVERSION);
			fprintf(stderr, "<Author: Shuaihu>\n");
			fprintf(stderr, "<Email: shuaihu@mail.com>\n");
			fprintf(stderr, "<Usage>\n\t %s -q <int> <file: .fastq|.gz>\n", PNAME);
			fprintf(stderr, "<Option>\n");
			fprintf(stderr, "\t-q <int> setting Quality Start Ascii code manually,\n");
			fprintf(stderr, "\t         usually is 33 or 64;\n");
			fprintf(stderr, "\t-h       show this help information;\n"); 
			fprintf(stderr, "<Notice>\n");
			fprintf(stderr, "\tRead length should be shorted than 2000;\n");
			fprintf(stderr, "\tFilename length should be shorted than 500;\n");
			fprintf(stderr, "\tThe statistics result will be output to the stdout\n");
			fprintf(stderr, "<END>\n");

		}	
		break;
	case 1:							// option-param
		fprintf(stderr, "%s: invalid parameter or option: -%c:\n", PNAME, c);
		break;
	case 2:							// input-file name param
		fprintf(stderr, "%s: too much or little filenames:\n", PNAME);
		break;
	case 3:							// open file
		if (c == 1)
		{
			fprintf(stderr, "%s: file formate error: %s:\n", PNAME, p);
		}else
		{
			fprintf(stderr, "%s: can't open file: %s:\n", PNAME, p);
		}
		break;
	case 4:							// file content
		if (c == 1)
		{
			fprintf(stderr, "%s: unknow character in file: %s", PNAME, p);
		}else
		{
			fprintf(stderr, "%s: file content error:\n", PNAME);
		}
		break;
	default:
		break;
	}
	if (num > 0)
	{
		note(0, 0, NULL);
	}
}
