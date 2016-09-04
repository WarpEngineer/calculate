#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#if defined(_WIN32)
#include<windows.h>
#else
#include<unistd.h>
#endif

#if !defined(_WIN32)
#define Sleep sleep
char* _strlwr(char *sstring);
void _strrev(char *sstring);
#endif

void StripChar(char String[], char CharsToStrip[], bool ItsAString);
bool EvaluateComplexExpression(char *expression, double &answer, int &SigDigit);

