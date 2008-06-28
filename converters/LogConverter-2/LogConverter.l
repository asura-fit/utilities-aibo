%{
#include <stdio.h>

int parenCount = 0;
int hasTarget = 0;
%}

%s		CCOMMENT
%s		CPPCOMMENT
%s		LOG
%s		LOGSP
%s		LOG_ARG1
%s		LOG_ARGSP
%s		LOG_ARG
%s		STRING
%s		NORMAL

BEGIN	NORMAL

%%

"/*"			{ BEGIN CCOMMENT; printf("/*"); }
"//"			{ BEGIN CPPCOMMENT; printf("//"); }
<NORMAL>"LOG_INFO"		{ BEGIN LOG; printf("LOG_INFO"); }
<NORMAL>"LOG_DEBUG"		{ BEGIN LOG; printf("LOG_DEBUG"); }
<NORMAL>"LOG_ERROR"		{ BEGIN LOGSP; printf("LOG_ERROR"); }
<NORMAL>"LOG_CRITICAL"		{ BEGIN LOGSP; printf("LOG_CRITICAL"); }
<NORMAL>"LOG_PRINT"		{ BEGIN LOGSP; printf("LOG_PRINT"); }
<NORMAL>"LOG_RAWPRINT"		{ BEGIN LOGSP; printf("LOG_RAWPRINT"); }

<LOG>"("		{ BEGIN LOG_ARG1; printf("("); }
<LOGSP>"("		{ BEGIN LOG_ARGSP; printf("("); }

<LOG_ARG1>[A-Za-z_][A-Za-z0-9_]* {
					printf("%s", yytext);
					hasTarget = 1;
				}
<LOG_ARG1>"\"" {
					if(!hasTarget)
						printf("LOG_DEFAULT ");
					else
						hasTarget = 0;
					printf("");
					printf("\"");
					BEGIN STRING;
				}
<LOG_ARGSP>"\"" {
					hasTarget = 0;
					printf("");
					printf("\"");
					BEGIN STRING;
				}
<LOG_ARG1>"<<"	{
					if(!hasTarget) {
						printf(",");
						hasTarget = 0;
						BEGIN LOG_ARG;
					} else {
						printf("_2_");
					}

				}
<LOG_ARG1>")"	{
					printf(")");
					parenCount = 0;
					hasTarget = 0;
					BEGIN NORMAL;
				}

<LOG_ARG>"("	{ parenCount++; printf("("); }
<LOG_ARG>")"	{
					parenCount--;
					if(parenCount < 0) {
						parenCount = 0;
						BEGIN NORMAL;
					}
					printf(")");
				}
<LOG_ARG>"<<"	{
					if(parenCount == 0)
						printf(",");
				}

<STRING>"\\\""	{	printf("\\\""); }
<STRING>"\","	{	printf("\","); BEGIN LOG_ARG1; }

<CCOMMENT>"*/"	{ BEGIN NORMAL; printf("*/"); }
<CCOMMENT>.		{ printf("%s", yytext); }

<CPPCOMMENT>"\n"	{
					BEGIN NORMAL;
					printf("\n");
				}
<CPPCOMMENT>.	{ printf("%s", yytext); }

.				{ printf("%s", yytext); }

%%

int main(int argc, char* argv[]) {
	yyin = stdin;
	yylex();
	return 0;
}
