// Calculate 3.7 BETA
// Written by A. G. Madi
//////////////////////////////
// Calculates Stuff

#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include<stdio.h>
#if defined(_WIN32)
#include<io.h>
#elif defined(__MACH__)
#include<dispatch/dispatch.h>
#else
#include<sys/io.h>
#endif
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>
#include<ctype.h>

#include"eval.h"

#define __C_VERSION__ "3.7 BETA"

void usage();
#if defined(_WIN32) || defined(__MACH__)
char *basename(char *path)
{
	char *p;

	p = path + strlen(path) - 1;
	while(p > path && *p != '\\' && *p != '/')
		p--;
	if(p > path)
		p++;

	return p;
}
#endif

char ArgToDelim(char *Arg)
{
	char delim;
	
	switch(Arg[0])
	{
		case '#':	delim = atoi(&Arg[1]);
					break;
		default:
					if(!strcmp(Arg, "\\t"))
						delim = '\t';
					else if(!strcmp(Arg, "\\n"))
						delim = '\n';
					else
						delim = Arg[0];
	}

	return delim;
}

bool GetField(char String[], int FieldNum, char Delim, char Result[])
{
	// String[]			--		this is the string to convert.
	// LengthOfString	--		guess what this is.
	// FieldNum			--		this is the field number to get.
	// Delim			--		the delimiter.
	// Result[]			--		the field goes back in this.

	int i = 0, DelimCount = 0, ResultCount = 0;
	bool error = false;
	int LengthOfString = strlen(String);

	while(DelimCount < (FieldNum - 1))
	{
		if(String[i++] == Delim)
			DelimCount++;
		if(i >= LengthOfString)
		{
			error = true;
			break;
		}
	}

	while(String[i] != Delim)
	{
		Result[ResultCount++] = String[i++];
		if(i >= LengthOfString)
			break;
	}

	Result[ResultCount] = '\0';
	if(Result[ResultCount-1] == '\n')
		Result[ResultCount-1] = '\0';

	return !error;
}


bool Spinner(int frequency)
{
	static int update = 0, next = 1;
	bool retVal = false;

	if(update >= frequency)
	{
		switch(next)
		{
			case 1:		fprintf(stderr, "|"); retVal = true; next++;
						break;
			case 2:		fprintf(stderr, "/"); retVal = true; next++;
						break;
			case 3:		fprintf(stderr, "-"); retVal = true; next++; 
						break;
			case 4:		fprintf(stderr, "\\"); retVal = true; next = 1;
						break;
		}
		update = 0;
	}
	fprintf(stderr, "\r");
	update++;
	return retVal;
}

void AddComma2(double Number, char *NewNum, int sigs)
{
	char Num[100], format[10], *p;
	long l1;

	sprintf(Num, "%f", Number);
	l1 = atol(Num);
	sprintf(Num, "%ld", l1);
	_strrev(Num);
	int j = 0, three = 0;
	for(unsigned int i = 0; i < strlen(Num); i++)
	{
		if(three == 3 && isdigit(Num[i]))
		{
			NewNum[j++] = ',';
			three = 0;
		}
		NewNum[j++] = Num[i];
		three++;
	}
	NewNum[j] = '\0';
	_strrev(NewNum);
	sprintf(format, "%%.%df", sigs);
	sprintf(Num, format, Number);
	p = strchr(Num, '.');
	if(p)
		strcat(NewNum, p);
}

int main(int argc, char* argv[])
{
	FILE *FileIn, *FileOut;
	char Source[1024], field[1024], delim;
	char expression[1024], temp[256], format[20], *p, ftemp[256];
	long count = 0;
#if defined(_WIN32)
	struct _finddata_t c_file;
	long hFile; bool notdone = true;
#endif

	double answer, tempanswer;
	int i, sigs, ifield;
	bool eval;

	double Total, Average, amount;
	long NumberNeg, NumberPos, NumberZero;

#if defined(_WIN32)
	sprintf( ftemp, "c:\\calc33.tmp" );
#else
	p = getenv("HOME");
	sprintf( ftemp, "%s/.calc33", p );
#endif

#if defined(_WIN32)
	if(strlen(basename(argv[0])) == 7)
#else
	if(strlen(basename(argv[0])) == 3)
#endif
	{
		FileIn = fopen(ftemp, "r");
		if(!FileIn)
			strcpy(expression, "0");
		else
		{
			fgets(expression, 1024, FileIn);
			fclose(FileIn);
		}
		if(expression[strlen(expression)-1] == '\n')
			expression[strlen(expression)-1] = 0;
		if(argv[1])
			strcat(expression, argv[1]);
		for(int i = 2; i < argc; i++)
			strcat(expression, argv[i]);
		while(p = strchr(expression, ',')) 
			strcpy(p, p+1);
		eval = EvaluateComplexExpression(expression, answer, sigs);
		if(!eval)
			printf("%s\n", expression);
		else
		{

			FileOut = fopen( ftemp, "w");
			sprintf(format, "%%.%df", sigs);
			sprintf(temp, format, answer);
			tempanswer = atof(temp);
			AddComma2(tempanswer, temp, sigs);
			printf("Answer: %s\n", temp);

			sprintf(format, "%%.%df", sigs);
			if(FileOut)
			{
				fprintf(FileOut, format, answer);
				fclose(FileOut);
			}
		}

		return 7;
	}

	if((argc < 2) || ((argc == 2) && (argv[1][0] == '-')))
	{
		usage();
		return 1;
	}

	if(!strcmp(argv[1], "-a") || !strcmp(argv[1], "-A"))
	{
		printf("CAUTION: PRE-RELEASE BETA TEST VERSION!\n");
		printf("Algebraic option not yet available\n");
		return 0;
	}

	if(strcmp(argv[1], "-f") && strcmp(argv[1], "-F"))
	{
		char expression[1024];
		double answer;
		int sigs;
		bool eval, sig = true;

		if(strcmp(argv[1], "-s") && strcmp(argv[1], "-S"))
		{
			strcpy(expression, argv[1]);
			for(i = 2; i < argc; i++)
			{
				strcat(expression, argv[i]);
			}
		}
		else
		{
			sig = false;
			strcpy(expression, argv[2]);
			for(i = 3; i < argc; i++)
			{
				strcat(expression, argv[i]);
			}
		}
		while(p = strchr(expression, ',')) 
			strcpy(p, p+1);
		eval = EvaluateComplexExpression(expression, answer, sigs);
		if(!eval)
			printf("%s\n", expression);
		else
		{

			FileOut = fopen(ftemp, "w");
			if(sig)
			{
				sprintf(format, "%%.%df", sigs);
				sprintf(temp, format, answer);
				tempanswer = atof(temp);
				AddComma2(tempanswer, temp, sigs);
				printf("Answer: %s\n", temp);

				sprintf(format, "%%.%df", sigs);
				if(FileOut)
					fprintf(FileOut, format, answer);
			}
			else
			{
				sprintf(format, "%%.0f");
				sprintf(temp, format, answer);
				tempanswer = atof(temp);
				AddComma2(tempanswer, temp, sigs);
				p = strchr(temp, '.');
				if(p)
					*p = 0;
				printf("Answer: %s\n", temp);

				if(FileOut)
					fprintf(FileOut, "%f", answer);
			}
			if(FileOut)
				fclose(FileOut);
		}

		return 7;
	}

	ifield = atoi(argv[2]);
	delim = ArgToDelim(argv[3]);
#if defined(_WIN32)
	if( (hFile = _findfirst( argv[4], &c_file )) == -1L )
	{
		fprintf(stderr, "No files resembling %s found!!!", argv[2]);
		return 1;
	}
	else
	{
		while(notdone)
		{
			FileIn = fopen(c_file.name, "r");
			if(!FileIn)
			{
				fprintf(stderr, "Could not open source file: %s ", c_file.name);
				if( _findnext( hFile, &c_file ) != 0 )
					notdone = false;
				continue;
			}
			if(!c_file.size)
			{
				fprintf(stderr, "Empty source file: %s ", c_file.name);
				if( _findnext( hFile, &c_file ) != 0 )
					notdone = false;
				continue;
			}

			count = NumberNeg = NumberPos = NumberZero = 0; Total = Average = 0.00;
			while(fgets(Source, 1024, FileIn) != NULL)
			{
				GetField(Source, ifield, delim, field);
				amount = atof(field);
				if(amount < 0.00)
					NumberNeg++;
				else if(amount > 0.00)
					NumberPos++;
				else
					NumberZero++;

				Total += amount;

				count++;
				Spinner(10000);

			}
			Average = Total / count;
			fclose(FileIn);
			fprintf(stdout, "Processed %ld records from file %s\n", count, c_file.name);
			fprintf(stdout, "Zero's: %ld\tPositives: %ld\tNegatives: %ld\n", NumberZero, NumberPos, NumberNeg);
			fprintf(stdout, "Total: %f\tAverage: %f\n\n", Total, Average);
			if( _findnext( hFile, &c_file ) != 0 )
					notdone = false;
		}
		_fcloseall();

	}
	_findclose( hFile );
#else
	for(i = 4; i < argc; i++)
	{
		FileIn = fopen(argv[i], "r");
		if(!FileIn)
		{
			fprintf(stderr, "Could not open source file: %s ", argv[i]);
			continue;
		}
		count = NumberNeg = NumberPos = NumberZero = 0; Total = Average = 0.00;
		while(fgets(Source, 1024, FileIn) != NULL)
		{
			GetField(Source, ifield, delim, field);
			amount = atof(field);
			if(amount < 0.00)
				NumberNeg++;
			else if(amount > 0.00)
				NumberPos++;
			else
				NumberZero++;
			Total += amount;

			count++;
			Spinner(10000);

		}
		Average = Total / count;
		fclose(FileIn);
		fprintf(stdout, "Processed %ld records from file %s\n", count, argv[i]);
		fprintf(stdout, "Zero's: %ld\tPositives: %ld\tNegatives: %ld\n", NumberZero, NumberPos, NumberNeg);
		fprintf(stdout, "Total: %f\tAverage: %f\n\n", Total, Average);
	}
#endif

	return 0;
}

void usage()
{
	fprintf(stderr, "Calculate: Version %s by A. G. Madi\n\n", __C_VERSION__);
	fputs("Calculate: Standard Calculations on Floating Point Numbers\n", stderr);
	fputs("\tPlace all negative numbers in parentheses\n", stderr);
	fputs("\tAlways put function arguments in parentheses\n", stderr);
	fputs("\t'pi' and 'e' are valid input values\n", stderr);
	fputs("usage1: Calculate <valid mathematical expression> 2 Significant Digits Default\n", stderr);
	fputs("usage2: Calculate -s <valid mathematical expression> No Significant Digits\n", stderr);
	fputs("usage3: Calculate -f <field containing floats> <delimiter> <file(s)>\n", stderr);
	fputs("\t\t<operation> can be +, -, *, /, % for percent, # for mod, and ^ for power\n", stderr);
	fputs("\t\t<function> can be: acos, asin, atan,\n", stderr);
	fputs("\t\t\tcos, cosh, sin, sinh, tan, tanh,\n", stderr);
	fputs("\t\t\tln, log, exp, sqrt\n", stderr);
	fputs("\t\tExample of valid mathematical expression: (5log(4)*5cos(pi))^2\n", stderr);
	fputs("CAUTION: PRE-RELEASE BETA TEST VERSION\n", stderr);
}



