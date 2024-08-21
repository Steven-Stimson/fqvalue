#include <stdio.h>
#include <math.h>

#define PVERSION "2.4"
#define MAXLINE	2002					// maximum line length in file; valid space is 2000 because of '\n' and '\0';1

void note(int, int, char *);

extern FILE *fp;
extern int QSTART;
static char line[4][MAXLINE];				// to store four lines read from fastq-file;

/* read_line: read four lines into line[][]; */
int read_line(void)
{
	int i;
	char *buf;

	for (i = 0; i < 4; i++)
	{
		while ((buf = fgets(line[i], MAXLINE, fp)) != NULL && line[i][0] == '\n')	// skip blank line;
			;
		if (buf == NULL)
		{
			if (i == 0)
			{
				return EOF;
			}else				// error: the four lines is not complete;
			{
				note(4, 0, NULL);
				return 1;					
			}
		}
	}
	return 0;
}

static unsigned int cbase_num[MAXLINE][6];		// ATGCN number for each cycle; the last element is total base of a cycle;
static unsigned long long rlen_sum;			// the sum of all read length; equal to total base number;
static unsigned long long rlen_sq_sum;			// the sum of Squares of all read length;
static unsigned int rnum;				// read's number;

static double cerr_sum[MAXLINE];			// error rate sum of base of each cycle;
static double cerr_sq_sum[MAXLINE];			// sum of Squares of error rate of base of each cycle;
static double rerr_sum;					// sum of all read's error rate;
static double rerr_sq_sum;				// sum of Squares of error rate of base;
static unsigned long long qlty_rank_num[2][3];		// base and read's quality rank: 10 20 30

/* check_line: check the base-line and quality line in line[1][] abd line[3][] and compute character by character; */
int check_line(void)
{
	int i;
	int rlen;					// read length;	
	double err;					// error rate;
	double rerr;					// single read's error rate;
	char index_base[128];

	// for base-line;
	for (i = 0; i < 128; i++)
	{
		index_base[i] = 5;
	}
	index_base['A'] = index_base['a'] = 0;
	index_base['T'] = index_base['t'] = 1;
	index_base['G'] = index_base['g'] = 2;
	index_base['C'] = index_base['c'] = 3;
	index_base['N'] = index_base['n'] = 4;
	for (i = 0; line[1][i] != '\n'; i++)
	{
		cbase_num[i][(int)index_base[(int)line[1][i]]]++;
		if (cbase_num[i][5])
		{
			note(4, 1, line[1]);		// file content error: unknown char or lowercase letter;
			return 1;
		}
	}
	rlen = i;
	rlen_sum += rlen;
	rlen_sq_sum += rlen * rlen;
	rnum++;

	// for quality-line
	rerr = 0.0;
	for (i = 0; i < rlen; i++)
	{
		err = pow(10, -(line[3][i] - QSTART) / 10.0);		// turn Quality into error rate
		cerr_sum[i] += err;
		cerr_sq_sum[i] += err * err;
		rerr += err;
		if (err <= 0.001)					// base quality rank
		{
			qlty_rank_num[0][2]++;
		}else if (err <= 0.01)
		{
			qlty_rank_num[0][1]++;
		}else if (err <= 0.1)
		{
			qlty_rank_num[0][0]++;
		}
	}
	rerr /= rlen;
	rerr_sum += rerr;
	rerr_sq_sum += rerr * rerr;
	if (rerr <= 0.001)						// read quality rank
	{
		qlty_rank_num[1][2]++;
	}else if (rerr <= 0.01)
	{
		qlty_rank_num[1][1]++;
	}else if (rerr <= 0.1)
	{
		qlty_rank_num[1][0]++;
	}
	return 0;
}

/* print: print the result calculated above; */
void print(void)
{
	int i, j, k;
	int cnum = 0;					// cycle number;
	double wcerr_sum = 0.0;				// the sum of average error rate of each cycle;
	double wcerr_sq_sum = 0.0;			// the sum of Squares sum of average error rate of each cycle;
	unsigned long long base_num[5] = {0llu};	// ATGCN total number;

	printf(">Start quality vaule: %d\n", QSTART);
	printf("\n>One-line statistics summary:\n");
	printf("Sequencing data ID\tNumber of Raw reads\tNumber of Raw bases\tRead length\tA%\tT%\tG%\tC%\tN%\tGC%\tBases Q20%\tBases Q30%\tReads Q20%\tReads Q30%\tError Avg\tError SD\n");
	printf(" \t");	//Sequencing data ID
	printf("%u\t", rnum);	// Number of Raw reads
	printf("%llu\t", rlen_sum);	//Number of Raw bases
	printf("%.1f (%.1f)\t", (double)rlen_sum / rnum, sqrt(((double)rlen_sq_sum / rnum) - pow(((double)rlen_sum / rnum), 2)));	//Read length			// avg length and standard deviation;
	for (cnum = 0; cerr_sum[cnum] != 0.0; cnum++)			
	{
		for (j = 0; j < 5; j++)					
		{
			cbase_num[cnum][5] += cbase_num[cnum][j];	// count base number of each cycle and store in the cbase_num[][5];
			base_num[j] += cbase_num[cnum][j];		// count ATGCN total number respectively;
		}
		cerr_sum[cnum] /= cbase_num[cnum][5];			// compute each cycle's avg error rate;
		wcerr_sum += cerr_sum[cnum];				// count sum of avg error rate among cycles;
		wcerr_sq_sum += cerr_sum[cnum] * cerr_sum[cnum];	// count sum of Squares of avg err...
	}	
	for (i = 0; i < 5; i++)
	{
		printf("%-.5f\t", (double)base_num[i] / rlen_sum * 100);
	}
	printf("%-.5f\t", (double)(base_num[2] + base_num[3]) / (rlen_sum - base_num[4]) * 100);	//A T G C N GC%
	for (i = 0; i < 2; i++)		//i= 0 represent bases,1 represents reads;
	{
		for (j = 0; j < 3; j++)	//Q10 Q20 Q30
		{
			for (k = j + 1; k <= 2; k++)
			{
				qlty_rank_num[i][j] += qlty_rank_num[i][k]; // add the rank num, such as make the Q10 = (>30) + (20-30) + （10-20） 
			}
		}
		for (j = 1; j < 3; j++)	//only print Q20 and Q30;
		{
			printf("%-.5f\t", (double)qlty_rank_num[i][j] / (i == 0 ? rlen_sum : rnum) * 100);
		}
	}	
	printf("%-.7f\t%-.7f\n", wcerr_sum / cnum, sqrt((wcerr_sq_sum / cnum) - pow((wcerr_sum / cnum), 2)));	//cycles error avg and error sd;
	printf("\n>Overview:\n");
	printf("#total reads: %u\n", rnum);
	printf("#total bases: %llu\n", rlen_sum);
	printf("#read mean length: %.1f (%.1f)\n", (double)rlen_sum / rnum, sqrt(((double)rlen_sq_sum / rnum) - pow(((double)rlen_sum / rnum), 2)));			// avg length and standard deviation;
	printf("\n   A%%        T%%        G%%        C%%       N%%        AT%%       GC%%\n");
	for (i = 0; i < 5; i++)
	{
		printf("%-10.5f", (double)base_num[i] / rlen_sum * 100);
	}
	printf("%-10.5f%-10.5f\n\n", (double)(base_num[0] + base_num[1]) / (rlen_sum - base_num[4]) * 100, (double)(base_num[2] + base_num[3]) / (rlen_sum - base_num[4]) * 100);
	printf("         Q10%%       Q20%%       Q30%%\n");
	for (i = 0; i < 2; i++)
	{
		i == 0 ? printf("base:  ") : printf("read:  ");
		for (j = 0; j < 3; j++)	// print Q10 Q20 Q30
		{
			printf("%-11.5f", (double)qlty_rank_num[i][j] / (i == 0 ? rlen_sum : rnum) * 100);
		}
		putchar('\n');
	}	
	printf("\n#cycle's average error: %-9.7f (%-9.7f)\n", wcerr_sum / cnum, sqrt((wcerr_sq_sum / cnum) - pow((wcerr_sum / cnum), 2)));
	printf("\n>Cycles:\n");
	printf("         A%%        T%%        G%%        C%%        N%%     Error_avg  Error_SD\n");
	for (i = 0; i < cnum; i++)
	{
		printf("#%-5d", i + 1);
		for (j = 0; j < 5; j++)
		{
			printf("%-10.5f", (double)cbase_num[i][j] / cbase_num[i][5] * 100);
		}
		printf("%-11.7f", cerr_sum[i]);				// already having turn the sum of error rate into average error rate of a cycle above;
		printf("%-9.7f\n", sqrt((cerr_sq_sum[i] / cbase_num[i][5]) - pow(cerr_sum[i], 2)));
	}
	printf("\n>Version: %s\n", PVERSION);
}
