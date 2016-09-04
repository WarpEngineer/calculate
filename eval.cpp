#include "eval.h"

#if !defined(_WIN32)

void _strrev(char *sstring)
{
	char *p;
	int l, i, j;

	l = strlen(sstring);	
	p = (char*)malloc(l + 1);
	if(p)
	{
		for(i = 0, j = l-1; i < l; i++)
			p[i] = sstring[j--];
		p[l] = 0;
		strcpy(sstring, p);
		free(p);
	}
}

char* _strlwr(char *sstring)
{
	char *p;
	p = sstring;
	while(*p)
	{
		*p = tolower(*p);
		p++;
	}
	return p;
}
#endif

void StripChar(char String[], char CharsToStrip[], bool ItsAString)
{
	// String[]			--		this is the string to convert.
	// LengthOfString	--		guess what this is.
	// CharsToStrip[]	--		characters or string to strip out.
	// ItsAString		--		true if CharsToStrip[] should be treated as a string, else treat as separate characters.

	int LengthOfString = strlen(String);
	int LengthOfCharsToStrip = strlen(CharsToStrip);

	char *pdest, *copy = new char[LengthOfString + 1];
	int copyCount = 0, result;
	bool bad = false;

	if(ItsAString)
	{
		do	// added
		{	// added
			pdest = strstr(String, CharsToStrip);
			if(pdest != NULL)
			{
				result = pdest - String;
				strncpy(copy, String, result);
				copy[result] = '\0';
				strcat(copy, &String[result + LengthOfCharsToStrip]);
				strcpy(String, copy);
			}
		} while(pdest != NULL);		// added
	}
	else
	{
		pdest = strstr(CharsToStrip, "\\t");
		if(pdest != NULL)
		{
			result = pdest - CharsToStrip;
			CharsToStrip[result] = '\t';
			CharsToStrip[result + 1] = '\t';
		}
		pdest = strstr(CharsToStrip, "\\n");
		if(pdest != NULL)
		{
			result = pdest - CharsToStrip;
			CharsToStrip[result] = '\n';
			CharsToStrip[result + 1] = '\n';
		}
		for(int i = 0; i < LengthOfString; i++)
		{
			bad = false;
			for(int j = 0; j < LengthOfCharsToStrip; j++)
			{
				if(String[i] == CharsToStrip[j])
				{
					bad = true;
					break;
				}
			}

			if(!bad)
				copy[copyCount++] = String[i];
		}

		copy[copyCount] = '\0';
		strcpy(String, copy);
	}

	delete copy;

}

double EvaluateSimpleExpression(char *operation, double a, double b)
{
	if((operation[0] == '+') || (operation[0] == '-') || (operation[0] == '*') || (operation[0] == '/') || (operation[0] == '%') || (operation[0] == '^') || (operation[0] == '#'))
	{
		switch(operation[0])
		{
			case '+':	return (a + b);
			case '-':	return (a - b);
			case '*':	return (a * b);
			case '/':	if(b != 0) return (a / b); else{ operation[0] = 'E'; return 0;}
			case '#':	if(b != 0) return (fmod(a, b)); else{ operation[0] = 'E';  return 0;}
			case '^':	return pow(a, b);
			case '%':	return a / 100.0;
		}
	}
	else
	{
		double answer, argument1;
		
		argument1 = a;
		_strlwr(operation);

		if(!strcmp(operation, "acos"))
		{
			answer = acos(argument1);
		}
		else if(!strcmp(operation, "asin"))
		{
			answer = asin(argument1);
		}
		else if(!strcmp(operation, "atan"))
		{
			answer = atan(argument1); 
		}
		else if(!strcmp(operation, "cos"))
		{
			answer = cos(argument1);
		}
		else if(!strcmp(operation, "cosh"))
		{
			answer = cosh(argument1);
		}
		else if(!strcmp(operation, "sin"))
		{
			answer = sin(argument1);
		}
		else if(!strcmp(operation, "sinh"))
		{
			answer = sinh(argument1);
		}
		else if(!strcmp(operation, "tan"))
		{
			answer = tan(argument1);
		}
		else if(!strcmp(operation, "tanh"))
		{
			answer = tanh(argument1);
		}
		else if(!strcmp(operation, "ln"))
		{
			if(argument1 < 0){operation[0] = 'E'; answer = 0;}
			else answer = log(argument1);
		}
		else if(!strcmp(operation, "log"))
		{
			if(argument1 < 0){operation[0] = 'E'; answer = 0;}
			else answer = log10(argument1);
		}
		else if(!strcmp(operation, "sqrt"))
		{
			answer = sqrt(argument1);
		}
		else if(!strcmp(operation, "exp"))
		{
			answer = exp(argument1);
		}
		else 
		{
			operation[0] = 'E'; 
			return 0;
		}
		return answer;
	}
	return 0;
}

void CompressStringArray(char array[][21], int &count)
{
	char **newarray = new char*[count];
	int i, j;

	for(i = 0; i < count; i++)
	{
		newarray[i] = new char[21];
	}

	for(i = 0, j = 0; i < count; i++)
	{
		if(strcmp(array[i], " "))
		{
			strcpy(newarray[j++], array[i]);
		}
	}

	for(i = 0; i < j; i++)
	{
		strcpy(array[i], newarray[i]);
	}

	count = j;

	return;
}


bool EvaluateComplexExpression(char *expression, double &answer, int &SigDigit)
{
	char Exp[100][21], FuncBuff[5];
	int ExpCount = 0, ParenthesesCount = 0, SigCount = 0;
	int i = 0, j; int nill;
	bool ParenthesesFound = false, negativeFound = false, decimalFound = false;

	SigDigit = 0;
	char *pi = "3.1415926535897932";
	char *e = "2.71828182846";
	char *npi = "-3.1415926535897932";
	char *ne = "-2.71828182846";

	StripChar(expression, " ", false);
	_strlwr(expression);

	while(expression[i])
	{
		if((expression[i] == '(') || (expression[i] == ')'))
		{
			Exp[ExpCount][0] = expression[i];
			Exp[ExpCount][1] = 0;
			ExpCount++;

			if((expression[i] == ')') && ((expression[i+1] == '(') || (isdigit(expression[i+1])) || (isalpha(expression[i+1]))))
			{
				Exp[ExpCount][0] = '*';
				Exp[ExpCount][1] = 0;
				ExpCount++;
			}

			if(expression[i] == '(')
			{
				ParenthesesCount++;
				ParenthesesFound = true;
			}
			else if(expression[i] == ')')
				ParenthesesCount--;
		}
		else if(isdigit(expression[i]))
		{
			j = 0;
			if(negativeFound)
			{
				Exp[ExpCount][j++] = '-';
				negativeFound = false;
			}
			while(isdigit(expression[i]) || (expression[i] == '.'))
			{
				if(decimalFound)
				{
					SigCount++;
					if(SigCount > SigDigit)
						SigDigit++;
				}
				if(expression[i] == '.')
					decimalFound = true;
				Exp[ExpCount][j++] = expression[i++];
			}
			SigCount = 0;
			decimalFound = false;
			Exp[ExpCount][j] = 0;
			ExpCount++;
			if((expression[i] == '(') || isalpha(expression[i]))
			{
				Exp[ExpCount][0] = '*';
				Exp[ExpCount][1] = 0;
				ExpCount++;
			}
			i--;
		}
		else if((expression[i] == '+') || (expression[i] == '-') || (expression[i] == '*') || (expression[i] == '/') || (expression[i] == '%') || (expression[i] == '^') || (expression[i] == '#'))
		{
			if(expression[i] == '-')
			{
				if(((expression[i-1]) && (!isdigit(expression[i-1]))) || (!expression[i-1]))
				{
					negativeFound = true;
				}
				else
				{
					Exp[ExpCount][0] = expression[i];
					Exp[ExpCount][1] = 0;
					ExpCount++;
				}
			}
			else
			{
				Exp[ExpCount][0] = expression[i];
				Exp[ExpCount][1] = 0;
				ExpCount++;
			}
		}
		else if(isalpha(expression[i]))
		{
			j = 0;
			while(isalpha(expression[i]))
			{
				FuncBuff[j++] = expression[i++];
			}
			FuncBuff[j] = 0;
			if(strcmp(FuncBuff, "acos") && strcmp(FuncBuff, "asin") && strcmp(FuncBuff, "atan") && strcmp(FuncBuff, "cos")
				&& strcmp(FuncBuff, "cosh") && strcmp(FuncBuff, "sin") && strcmp(FuncBuff, "sinh") && strcmp(FuncBuff, "tan")
				&& strcmp(FuncBuff, "tanh") && strcmp(FuncBuff, "ln") && strcmp(FuncBuff, "log") && strcmp(FuncBuff, "sqrt")
				&& strcmp(FuncBuff, "exp") && strcmp(FuncBuff, "pi") && strcmp(FuncBuff, "e"))
			{
				strcpy(expression, "Illegal Function Call: ");
				strcat(expression, FuncBuff);
				return false;
			}
			if(negativeFound)
			{
				strcpy(Exp[ExpCount++], "-1");
				strcpy(Exp[ExpCount++], "*");
				strcpy(Exp[ExpCount], FuncBuff);
				negativeFound = false;
			}
			else
				strcpy(Exp[ExpCount], FuncBuff);
			ExpCount++;
			if((expression[i] != '(') && (strcmp(FuncBuff, "pi")) && (strcmp(FuncBuff, "e")) && (strcmp(FuncBuff, "-pi")) && (strcmp(FuncBuff, "-e")))
			{
				strcpy(expression, "Missing Parenthesis after: ");
				strcat(expression, FuncBuff);
				return false;
			}
			i--;
		}
		else
		{
			FuncBuff[0] = expression[i];
			FuncBuff[1] = 0;
			strcpy(expression, "Illegal Value: ");
			strcat(expression, FuncBuff);
			return false;
		}

		i++;
	}

	if(ParenthesesCount)
	{
		strcpy(expression, "Unmatched Parentheses");
		return false;
	}

	for(i = 0; i < ExpCount; i++)
	{
		if(!strcmp(Exp[i], "pi"))
			strcpy(Exp[i], pi);
		if(!strcmp(Exp[i], "e"))
			strcpy(Exp[i], e);
		if(!strcmp(Exp[i], "-pi"))
			strcpy(Exp[i], npi);
		if(!strcmp(Exp[i], "-e"))
			strcpy(Exp[i], ne);
	}

	double EvalAns;
	char Eval[100];
	int openparen, closedparen;
	bool openparenFound = false, closedparenFound = false, functionFound = false;

	if(ParenthesesFound)
	{
		if(ExpCount == 3)
		{
			answer = atof(Exp[1]);
			if(SigDigit == 0)
				SigDigit = 2;
			return true;
		}
		openparenFound = false; closedparenFound = false;
		for(i = 0; i < ExpCount; i++)
		{
			if((Exp[i][0] == '('))
			{
				openparen = i;
				openparenFound = true;
			}
			if(Exp[i][0] == ')')
			{
				closedparen = i;
				closedparenFound = true;
				Eval[0] = 0;
				for(j = openparen+1; j < closedparen; j++)
				{
					if((Exp[j][0] == '-') && (strlen(Exp[j]) > 1))
					{
						strcat(Eval, "(");
						strcat(Eval, Exp[j]);
						strcat(Eval, ")");
					}
					else
						strcat(Eval, Exp[j]);
				}
				EvaluateComplexExpression(Eval, EvalAns, nill);
				if((Exp[openparen-1]) && (isalpha(Exp[openparen-1][0])))
				{
					EvalAns = EvaluateSimpleExpression(Exp[openparen-1], EvalAns, 0);
					if((!EvalAns) && (Exp[openparen-1][0] == 'E')){ strcpy(expression, "NAN"); return false;}
					openparen--;
				}
				sprintf(Eval, "%f", EvalAns);
				strcpy(Exp[openparen], Eval);
				for(j = openparen+1; j <= closedparen; j++)
				{
					strcpy(Exp[j], " ");
				}
				CompressStringArray(Exp, ExpCount);
				i = -1;
				continue;
			}
		}
	}

	bool operation = false;
		
	if(ExpCount == 1)
	{
		answer = atof(Exp[0]);
		if(SigDigit == 0)
				SigDigit = 2;
		return true;
	}

	for(i = 0; i < ExpCount; i++)
	{
		if(Exp[i][0] == '^')
		{
			EvalAns = EvaluateSimpleExpression(Exp[i], atof(Exp[i-1]), atof(Exp[i+1]));
			if((!EvalAns) && (Exp[i][0] == 'E')){ strcpy(expression, "NAN"); return false;}
			sprintf(Eval, "%f", EvalAns);
			strcpy(Exp[i-1], Eval);
			strcpy(Exp[i], " ");
			strcpy(Exp[i+1], " ");
			CompressStringArray(Exp, ExpCount);
			i = -1;
			continue;
		}
	}
	for(i = 0; i < ExpCount; i++)
	{
		if((Exp[i][0] == '*') || (Exp[i][0] == '/') || (Exp[i][0] == '#'))
		{
			EvalAns = EvaluateSimpleExpression(Exp[i], atof(Exp[i-1]), atof(Exp[i+1]));
			if((!EvalAns) && (Exp[i][0] == 'E')){ strcpy(expression, "NAN"); return false;}
			sprintf(Eval, "%f", EvalAns);
			strcpy(Exp[i-1], Eval);
			strcpy(Exp[i], " ");
			strcpy(Exp[i+1], " ");
			CompressStringArray(Exp, ExpCount);
			i = -1;
			continue;
		}
		else if(Exp[i][0] == '%')
		{
			EvalAns = EvaluateSimpleExpression(Exp[i], atof(Exp[i-1]), 0);
			if((!EvalAns) && (Exp[i][0] == 'E')){ strcpy(expression, "NAN"); return false;}
			sprintf(Eval, "%f", EvalAns);
			strcpy(Exp[i-1], Eval);
			strcpy(Exp[i], " ");
			CompressStringArray(Exp, ExpCount);
			i = -1;
		}
	}
	for(i = 0; i < ExpCount; i++)
	{
		if((Exp[i][0] == '+') || (Exp[i][0] == '-'))
		{
			if((Exp[i][0] == '-') && (ExpCount == 1))
				break;
			else if((Exp[i][0] == '-') && ( i == 0))
				continue;
			EvalAns = EvaluateSimpleExpression(Exp[i], atof(Exp[i-1]), atof(Exp[i+1]));
			if((!EvalAns) && (Exp[i][0] == 'E')){ strcpy(expression, "NAN"); return false;}
			sprintf(Eval, "%f", EvalAns);
			strcpy(Exp[i-1], Eval);
			strcpy(Exp[i], " ");
			strcpy(Exp[i+1], " ");
			CompressStringArray(Exp, ExpCount);
			i = -1;
			continue;
		}
	}


	answer = EvalAns;
	if(SigDigit == 0)
		SigDigit = 2;

	return true;
}

