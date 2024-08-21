/* Program Name: fqvalue
 * Version: 2.4
 * Description: To do a simple scan on the fastq file and to statistic the total reads, bases, mean length, contents of ATGCN of totality and each cycle, Q10, Q20, Q30, error rate of read and cycle, fluctuation between cycles and so on;
 * Author: Shuaihu Wang
 * Date: 3/2/15
 */

#include <stdio.h>

int init(int argc, char *argv[]);
int read_line(void);
int check_line(void);
void print(void);

int QSTART;					// ASCII code of Quality start value;
FILE *fp;					// point to input file;

int main(int argc, char *argv[])
{
	int flag;

	if (init(argc, argv))			// get option then open file;
	{
		return 1;
	}
	while((flag = read_line()) == 0)	// read four lines every time;
	{
		if (check_line())		// check base-line and quality-line;
		{
			return 1;		// unknown character occured in file;
		}
	}
	if (flag == EOF)			// arrive the end of file;
	{
		print();
	}else					// the file is not complete;
	{
		return 1;
	}
	return 0;	
}
