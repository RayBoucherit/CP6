%{
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#define YY_NO_UNPUT
#define YY_NO_INPUT
%}

%option noyywrap

digit       [0-9]
negint      -{digit}+
posint      {digit}+

%%

{negint}    {yylval.ival = atoi(yytext); return NEGINT;}
{posint}    {yylval.ival = atoi(yytext); return POSINT;}

"("         {return LPAR;}
")"         {return RPAR;}

"+"         {return PLUS;}
"-"         {return MINUS;}
"*"         {return STAR;}
"/"         {return SLASH;}
"%"         {return PERC;}

"<"         {return LESSER;}
"<="        {return LEQUAL;}
"="         {return EQUAL;}
"<>"        {return NEQUAL;}
">="        {return GEQUAL;}
">"         {return GREATER;}

"PEEK"      {return PEEK;}
"RAND"      {return RAND;}
"CARDINAL"  {return CARDINAL;}
"SELF"      {return SELF;}
"SPEED"     {return SPEED;}
"STATE"     {return STATE;}
"GPSX"      {return GPSX;}
"GPSY"      {return GPSY;}
"ANGLE"     {return ANGLE;}
"TARGETX"   {return TARGETX;}
"TARGETY"   {return TARGETY;}
"DISTANCE"  {return DISTANCE;}

"WAIT"      {return WAIT;}
"POKE"      {return POKE;}
"GOTO"      {return GOTO;}
"ENGINE"    {return ENGINE;}
"SHOOT"     {return SHOOT;}

"IF"        {return IF;}
"THEN"      {return THEN;}

[ \t]       {}

[\r\n]      {return LINEBR;}

<<EOF>>     {return END;}

%%
