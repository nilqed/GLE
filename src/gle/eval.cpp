/************************************************************************
 *                                                                      *
 * GLE - Graphics Layout Engine <http://www.gle-graphics.org/>          *
 *                                                                      *
 * Modified BSD License                                                 *
 *                                                                      *
 * Copyright (C) 2009 GLE.                                              *
 *                                                                      *
 * Redistribution and use in source and binary forms, with or without   *
 * modification, are permitted provided that the following conditions   *
 * are met:                                                             *
 *                                                                      *
 *    1. Redistributions of source code must retain the above copyright *
 * notice, this list of conditions and the following disclaimer.        *
 *                                                                      *
 *    2. Redistributions in binary form must reproduce the above        *
 * copyright notice, this list of conditions and the following          *
 * disclaimer in the documentation and/or other materials provided with *
 * the distribution.                                                    *
 *                                                                      *
 *    3. The name of the author may not be used to endorse or promote   *
 * products derived from this software without specific prior written   *
 * permission.                                                          *
 *                                                                      *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR   *
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED       *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   *
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY       *
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   *
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE    *
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS        *
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER *
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR      *
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN  *
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                        *
 *                                                                      *
 ************************************************************************/

#define GRAPHDEF extern

#include "all.h"
#include "tokens/stokenizer.h"
#include "core.h"
#include "glearray.h"
#include "mem_limits.h"
#include "var.h"
#include "cutils.h"
#include "gprint.h"
#include "numberformat.h"
#include "keyword.h"
#include "run.h"
#include "fn.h"
#include "polish.h"
#include "file_io.h"
#include "cmdline.h"
#include "graph.h"
#include "sub.h"
#include "file_io.h"
#ifdef __WIN32__
#include <time.h>
#endif

colortyp colvar;
//
// set this to zero if your compiler complains about acosh atanh and asinh
//
// #define HAVE_INVERSE_HYP <== obsolete should be done in config.i V.L.
//
#define true (!false)
#define false 0
char *eval_str();
void var_getstr(int varnum,char *s);
//int pass_marker(char *s);
int f_eof(int chn);
void gle_as_a_calculator(vector<string>* exprs);
void output_error_cerr(ParserError& err);
double xg3d(double x, double y, double z);
double yg3d(double x, double y, double z);

/*---------------------------------------------------------------------------*/
/* bin = 10..29, binstr = 30..49, fn= 60...139, userfn=LOCAL_START_INDEX..nnn */
/* pcode:,  1=exp,len  2=float,val 3=var,int 4,string_var, 5=string,.../0 */
/*---------------------------------------------------------------------------*/
/* Input is exp-pcode, output is number or string */

const char *binop[] = { "", "+", "-", "*", "/", "^", "=", "<", "<=", ">", ">=", "<>", ".AND.", ".OR." };

struct keyw
{
	char *word;
	int index;
	int ret,np,p[5];
} ;

extern struct keyw keywfn[] ;

double stk[60];
int stk_var[100];
char *stk_str[100];
int stk_strlen[100];
char sbuf[512];
char sbuf2[112];
int nstk=0;
extern int gle_debug;

extern CmdLineObj g_CmdLine;

double string_to_number(const char* str) {
// FIXME support other numeric formats
	char* pend;
	return strtod(str, &pend);
}

void format_number_to_string(char* out, const char* format, double value);

unsigned char float_to_color_comp(double value) {
	int color = (int)floor(value*255 + 0.5);
	if (color < 0) color = 0;
	if (color > 255) color = 255;
	return (unsigned char)color;
}

unsigned char float_to_color_comp_255(double value) {
	int color = (int)floor(value + 0.5);
	if (color < 0) color = 0;
	if (color > 255) color = 255;
	return (unsigned char)color;
}

void eval_get_extra_arg_test(int i, const char* type) throw(ParserError) {
	int max_arg = g_CmdLine.getNbExtraArgs();
	if (max_arg == 0) {
		stringstream s;
		s << "arg" << type << "(" << i << "): no command line arguments given";
		g_throw_parser_error(s.str());
	}
	if (i > max_arg || i <= 0) {
		stringstream s;
		s << "arg" << type << "(" << i << "): argument out of range (1.." << max_arg << ")";
		g_throw_parser_error(s.str());
	}
}

double eval_get_extra_arg_f(int i) throw(ParserError) {
	eval_get_extra_arg_test(i, "");
	const string& arg = g_CmdLine.getExtraArg(i-1);
	if (!is_float(arg)) {
		stringstream s;
		s << "arg(" << i << "): argument not a floating point number: " << arg;
		g_throw_parser_error(s.str());
	}
	return atof(arg.c_str());
}

const char* eval_get_extra_arg_s(int i) throw(ParserError) {
	eval_get_extra_arg_test(i, "$");
	return g_CmdLine.getExtraArg(i-1).c_str();
}

void eval_pcode_loop(int *pcode, int plen, int *otyp) throw(ParserError) {
	if (plen > 1000) {
		gprint("Expression is suspiciously long %d \n",plen);
	}
	union {double d; int l[2];} both;
	char *ss2, *ss;
	double x1, y1, x2, y2;
	double xx, yy;
	int i, j;
	for (int c = 0; c < plen; c++) {
	  // cout << "pos: " << c << " pcode: " << pcode[c] << endl;
	  switch (pcode[c]) {
		/*
		..
		Special commands 1..9  -------------------------------
		..
		*/
		case 1:	/* Start of another expression (function param) */
			c++;	/* skip over exp length */
			break;
		case 2: /* Floating point number follows */
			*otyp = 1;
			both.l[0] = *(pcode+(++c));
			both.l[1] = *(pcode+(++c));
			stk[++nstk] =  both.d;
 			dbg gprint("Got float %f %d %f \n",stk[nstk],nstk,*(pcode+(c)));
			break;
		case 3: /* Floating_point variable number follows */
			*otyp = 1;
			var_get(*(pcode+(++c)),&xx);
			dbg gprint("Got variable value %ld %f \n",*(pcode+(c)),xx);
			stk[++nstk] = xx;
			break;
		case 4: /* string variable number follows */
			*otyp = 2;
			{
				string result;
				var_getstr(*(pcode+(++c)), result); nstk++;
				if (stk_str[nstk]!=NULL) myfree(stk_str[nstk]);
				stk_str[nstk] = sdup(result.c_str());
			}
 			break;
		case 5: /* Null terminated string follows (int alligned) */
			*otyp = 2;
			c++; nstk++;
			if (stk_str[nstk]!=NULL) myfree(stk_str[nstk]);
			stk_str[nstk] = sdup(eval_str(pcode,&c));
			break;
		/*


		Numeric Binary operators 10..29 -----------------------


		*/
		case 11:  /* + */
			nstk--;
			stk[nstk] = stk[nstk+1] + stk[nstk];
			break;
		case 12:  /* - */
			stk[nstk-1] = stk[nstk-1] - stk[nstk];
			nstk--;
			break;
		case 13:  /* * */
			stk[nstk-1] = stk[nstk-1] * stk[nstk];
			nstk--;
			break;
		case 14:  /* / */
			// do not test on divide by zero, otherwise "let"
			// cannot plot functions with divide by zero anymore
			stk[nstk-1] = stk[nstk-1] / stk[nstk];
			nstk--;
			break;
		case 15:  /* ^ */
			stk[nstk-1] = pow(stk[nstk-1],stk[nstk]);
			nstk--;
			break;
		case 16:  /* = */
			nstk--;
			if (stk[nstk] == stk[nstk+1]) stk[nstk]=true;
			else stk[nstk]=false;
			break;
		case 17:  /* <   */
			nstk--;
			if (stk[nstk] < stk[nstk+1]) stk[nstk]=true;
			else stk[nstk]=false;
			break;
		case 18:  /* <=  */
			nstk--;
			if (stk[nstk] <= stk[nstk+1]) stk[nstk]=true;
			else stk[nstk]=false;
			break;
		case 19:  /* >   */
			nstk--;
			if (stk[nstk] > stk[nstk+1]) stk[nstk]=true;
			else stk[nstk]=false;
			break;
		case 20:  /* >=  */
			nstk--;
			if (stk[nstk] >= stk[nstk+1]) stk[nstk]=true;
			else stk[nstk]=false;
			break;
		case 21:  /*  <>  */
			nstk--;
			if (stk[nstk] != stk[nstk+1]) stk[nstk]=true;
			else stk[nstk]=false;
			break;
		case 22:  /* .AND.  */
			nstk--;
			if (stk[nstk] && stk[nstk+1]) stk[nstk]=true;
			else stk[nstk]=false;
			break;
		case 23:  /* .OR.   */
			nstk--;
			if (stk[nstk] || stk[nstk+1]) stk[nstk]=true;
			else stk[nstk]=false;
			break;
		case 24:  /* % */
			stk[nstk-1] = (int)stk[nstk-1] % (int)stk[nstk];
			nstk--;
			break;
		case 25:  /* . */
			g_throw_parser_error("operator '.' does not apply to a numeric type");
			break;
		/* String Binary operators 30..49 ----------------------- */
		case 31:  /* + */
			*otyp = 2;
			nstk--;
			{
				char* result = NULL;
				if (stk_str[nstk] != NULL && stk_str[nstk+1] != NULL) {
					result = (char*)malloc((strlen(stk_str[nstk])+strlen(stk_str[nstk+1])+1)*sizeof(char));
					strcpy(result, stk_str[nstk]);
					strcat(result, stk_str[nstk+1]);
				} else {
					result = sdup("");
				}
				if (stk_str[nstk] != NULL) myfree(stk_str[nstk]);
				stk_str[nstk] = result;
			}
			break;
		case 32:  /* - */
			g_throw_parser_error("operator '-' does not apply to a string type");
			break;
		case 33:  /* * */
			g_throw_parser_error("operator '*' does not apply to a string type");
			break;
		case 34:  /* / */
			g_throw_parser_error("operator '/' does not apply to a string type");
			break;
		case 35:  /* ^ */
			g_throw_parser_error("operator '^' does not apply to a string type");
			break;
		case 36:  /* = */
			*otyp = 1;
			nstk--;
			stk[nstk] = str_i_equals(stk_str[nstk], stk_str[nstk+1]);
			break;
		case 37:  /* <   */
			*otyp = 1;
			nstk--;
			stk[nstk] = str_i_cmp(stk_str[nstk], stk_str[nstk+1]) < 0;
			break;
		case 38:  /* <=  */
			*otyp = 1;
			nstk--;
			stk[nstk] = str_i_cmp(stk_str[nstk], stk_str[nstk+1]) <= 0;
			break;
		case 39:  /* >   */
			*otyp = 1;
			nstk--;
			stk[nstk] = str_i_cmp(stk_str[nstk], stk_str[nstk+1]) > 0;
			break;
		case 40:  /* >=  */
			*otyp = 1;
			nstk--;
			stk[nstk] = str_i_cmp(stk_str[nstk], stk_str[nstk+1]) >= 0;
			break;
		case 41:  /* <>  */
			*otyp = 1;
			nstk--;
			stk[nstk] = !str_i_equals(stk_str[nstk], stk_str[nstk+1]);
			break;
		case 42:  /* .AND.  */
			g_throw_parser_error("operator 'AND' does not apply to a string type");
			break;
		case 43:  /* .OR.  */
			g_throw_parser_error("operator 'OR' does not apply to a string type");
			break;
		case 44:  /* %  */
			g_throw_parser_error("operator '%' does not apply to a string type");
			break;
		case 45:  /* . */
			*otyp = 2;
			nstk--;
			{
				char* result = NULL;
				if (stk_str[nstk] != NULL && stk_str[nstk+1] != NULL) {
					result = (char*)malloc((strlen(stk_str[nstk])+strlen(stk_str[nstk+1])+2)*sizeof(char));
					strcpy(result, stk_str[nstk]);
					strcat(result, ".");
					strcat(result, stk_str[nstk+1]);
				} else {
					result = sdup("");
				}
				if (stk_str[nstk] != NULL) myfree(stk_str[nstk]);
				stk_str[nstk] = result;
			}
			break;

	    /* look in fn.c and start indexes with 1 */
		/* Built in functions 60..199 ----------------------------- */
		/* note add 60 to the index in fn.cpp */
		case 61: /* unary plus */
			break;
		case 62: /* unary minus */
			stk[nstk] = -stk[nstk];
			break;
		case 63: /* abs */
			stk[nstk] = fabs(stk[nstk]);
			break;
		case 64: /* atan */
			stk[nstk] = atan(stk[nstk]);
			break;
		case 113: /* ACOS 53*/
			stk[nstk] = acos(stk[nstk]);
			break;
		case 114: /* ASIN 54*/
			stk[nstk] = asin(stk[nstk]);
			break;
		case 65: /* cos 5*/
			stk[nstk] = cos(stk[nstk]);
			break;
		case 116: /* ACOT 56*/
			stk[nstk] = 1.0/atan(stk[nstk]);
			break;
		case 117: /*ASEC 57*/
			stk[nstk] = 1.0/acos(stk[nstk]);
			break;
		case 118: /*ACSC 58*/
			stk[nstk] = 1.0/asin(stk[nstk]);
			break;
		case 119: /*cot 59*/
			stk[nstk] = 1.0/tan(stk[nstk]);
			break;
		case 120: /*sec 60*/
			stk[nstk] = 1.0/cos(stk[nstk]);
			break;
		case 121: /*csc 61*/
			stk[nstk] = 1.0/sin(stk[nstk]);
			break;
		case 122: /* cosh 62*/
			stk[nstk] = cosh(stk[nstk]);
			break;
		case 123: /* sinh 63*/
			stk[nstk] = sinh(stk[nstk]);
			break;
		case 124: /* tanh 64*/
			stk[nstk] = tanh(stk[nstk]);
			break;
		case 125: /* coth 65*/
			stk[nstk] = 1.0/tanh(stk[nstk]);
			break;
		case 126: /* sech 66*/
			stk[nstk] = 1.0/cosh(stk[nstk]);
			break;
		case 127: /* csch 67*/
			stk[nstk] = 1.0/sinh(stk[nstk]);
			break;
		#ifdef HAVE_INVERSE_HYP
		case 128: /* acosh 68*/
			stk[nstk] = acosh(stk[nstk]);
			break;
		case 129: /* asinh 69*/
			stk[nstk] = asinh(stk[nstk]);
			break;
		case 130: /* atanh 70*/
			stk[nstk] = atanh(stk[nstk]);
			break;
		case 131: /* acoth 71*/
			stk[nstk] = 1.0/atanh(stk[nstk]);
			break;
		case 132: /* asech 72*/
			stk[nstk] = 1.0/acosh(stk[nstk]);
			break;
		case 133: /* acsch 73*/
			stk[nstk] = 1.0/asinh(stk[nstk]);
			break;
		#endif
		case 134: /* todeg 74*/
			stk[nstk] = stk[nstk]*180.0/GLE_PI;
			break;
		case 135: /* torad 75*/
			stk[nstk] = stk[nstk]*GLE_PI/180.0;
			break;
		case 60+FN_EVAL: /* eval */
			*otyp = 1;
			polish_eval(stk_str[nstk], &xx);
			stk[nstk] = xx;
			break;
		case 60+FN_ARG: /* arg */
			*otyp = 1;
			stk[nstk] = eval_get_extra_arg_f((int)stk[nstk]);
			break;
		case 60+FN_ARGS: /* arg$ */
			*otyp = 2;
			stk_str[nstk] = sdup((char*)eval_get_extra_arg_s((int)stk[nstk]));
			break;
		case 60+FN_NARGS: /* narg */
			*otyp = 1;
			stk[++nstk] = g_CmdLine.getNbExtraArgs();
			break;
		case 60+FN_MIN: /* min */
			*otyp = 1; nstk--;
			if (stk[nstk+1] < stk[nstk]) stk[nstk] = stk[nstk+1];
			break;
		case 60+FN_MAX: /* max */
			*otyp = 1; nstk--;
			if (stk[nstk+1] > stk[nstk]) stk[nstk] = stk[nstk+1];
			break;
		case 60+FN_SDIV: /* sdiv */
			*otyp = 1; nstk--;
			if (stk[nstk+1] == 0.0) stk[nstk] = 0.0;
			else stk[nstk] /= stk[nstk+1];
			break;
		case 60+FN_XBAR: /* bar x position */
			*otyp = 1; nstk -= 1;
			stk[nstk] = graph_bar_pos(stk[nstk], gle_round_int(stk[nstk+1]), 1);
			break;
		case 60+FN_XY2ANGLE:
			*otyp = 1; nstk -= 1;
			xy_polar(stk[nstk], stk[nstk+1], &x1, &y1);
			stk[nstk] = y1;
			break;
		case 60+FN_ATAN2:
			*otyp = 1; nstk -= 1;
			stk[nstk] = myatan2(stk[nstk], stk[nstk+1]);
			break;
		case 60+FN_ISNAME:
			*otyp = 1;
			stk[nstk] = getGLERunInstance()->is_name(stk_str[nstk]) ? 1.0 : 0.0;
			break;
		case 137: /* pointx */
			*otyp = 1;
			{
				GLEPoint pt;
				getGLERunInstance()->name_to_point(stk_str[nstk], &pt);
				stk[nstk] = pt.getX();
			}
			break;
		case 138: /* pointy */
			*otyp = 1;
			{
				GLEPoint pt;
				getGLERunInstance()->name_to_point(stk_str[nstk], &pt);
				stk[nstk] = pt.getY();
			}
			break;
		case 139: /* format$ */
			nstk--;
			*otyp = 2;
			format_number_to_string(sbuf, stk_str[nstk+1], stk[nstk]);
			if (stk_str[nstk] != NULL) myfree(stk_str[nstk]);
			stk_str[nstk] = sdup(sbuf);
			break;
		case 60+FN_GETENV: /* getenv */
			*otyp = 2;
			{
				string result;
				GLEGetEnv(string(stk_str[nstk]), result);
				stk_str[nstk] = sdup(result.c_str());
			}
			break;
		case 66: /* date$ */
			{
				*otyp = 2;
				time_t today;
				time(&today);
				strcpy(sbuf2,ctime(&today));
				strcpy(sbuf,sbuf2);
				strcpy(sbuf+11,sbuf2+20);
				sbuf[strlen(sbuf)-1] = 0;
				setdstr(&stk_str[++nstk],sbuf);
			}
			break;
		case 111: /* device$ */
			*otyp = 2;
			g_get_type(sbuf);
			setdstr(&stk_str[++nstk],sbuf);
			break;
		case 115: /* feof(chan) */
			stk[nstk] = f_eof((int) stk[nstk]);
			break;
		case 67: /* exp */
			stk[nstk] = exp(stk[nstk]);
			break;
		case 68: /* fix*/
			stk[nstk] = floor(stk[nstk]);
			break;
		case 69: /* height of named object */
			*otyp = 1;
			getGLERunInstance()->name_to_size(stk_str[nstk], &x1, &y1);
			stk[nstk] = y1;
			break;
		case 70: /* int (??int) */
			stk[nstk] = floor(fabs(stk[nstk]))
				*( (stk[nstk]>=0)?1:-1 ) ;
			break;
		case 112: /* CHR$() */
			*otyp = 2;
			sprintf(sbuf,"%c",(int) stk[nstk]);
			setdstr(&stk_str[nstk],sbuf);
			break;
		case 71: /* left$ */
			*otyp = 2;
			{
				int i1 = (int)stk[nstk];
				int len = strlen(stk_str[nstk-1]);
				if (i1 < 0) i1 = 0;
				if (i1 > len) i1 = len;
				char* result = (char*)malloc((i1+1)*sizeof(char));
				ncpy(result, stk_str[nstk-1], i1);
				setsstr(&stk_str[--nstk], result);
			}
			break;
		case 72: /* len */
			*otyp = 1;
			stk[nstk] = strlen(stk_str[nstk]);
			break;
		case 73: /* log */
			stk[nstk] = log(stk[nstk]);
			break;
		case 74: /* log10 */
			stk[nstk] = log10(stk[nstk]);
			break;
		case 75: /* not */
			stk[nstk] = !(stk[nstk]);
			break;
		case 76: /* num$ */
			*otyp = 2;
			sprintf(sbuf,"%g",stk[nstk]);
			if (stk_str[nstk] != NULL) myfree(stk_str[nstk]);
			stk_str[nstk] = sdup(sbuf);
			break;
		case 77: /* num1$ */
			*otyp = 2;
			sprintf(sbuf,"%g ",stk[nstk]);
			if (stk_str[nstk] != NULL) myfree(stk_str[nstk]);
			stk_str[nstk] = sdup(sbuf);
			break;
		case 78: /* pageheight */
			*otyp = 1;
			g_get_usersize(&xx, &yy);
			stk[++nstk] = yy;
			break;
		case 79: /* pagewidth */
			*otyp = 1;
			g_get_usersize(&xx, &yy);
			stk[++nstk] = xx;
			break;
		case 80: /* pos */
			*otyp = 1;
			i = (int) stk[nstk];
			if (i<=0) i = 1;
			ss = stk_str[nstk-2];
			ss2 = str_i_str(ss+i-1,stk_str[nstk-1]);
			if (ss2!=NULL) 	i = ss2-ss+1;
			else 		i = 0;
			nstk -= 2;
			stk[nstk] = i;
			break;
		case 81: /* right$ */
			*otyp = 2;
			{
				int i1 = (int)stk[nstk]-1;
				int len = strlen(stk_str[nstk-1]);
				if (i1 < 0) i1 = 0;
				if (i1 > len) i1 = len;
				char* result = (char*)malloc((len-i1+1)*sizeof(char));
				strcpy(result, stk_str[nstk-1]+i1);
				setsstr(&stk_str[--nstk], result);
			}
			break;
		case 82: /* rnd */
			stk[nstk] = ((double) rand()/(double)RAND_MAX)*stk[nstk];
			break;
		case 83: /* seg$ */
			*otyp = 2;
			{
				int i1 = (int)stk[nstk-1]-1;
				int len = strlen(stk_str[nstk-2]);
				if (i1 < 0) i1 = 0;
				if (i1 > len) i1 = len;
				int n1 = (int)stk[nstk]-i1;
				if (i1+n1 > len) n1 = len-i1;
				if (n1 < 0) n1 = 0;
				char* result = (char*)malloc((n1+1)*sizeof(char));
				ncpy(result, stk_str[nstk-2]+i1, n1);
				nstk -= 2;
				setsstr(&stk_str[nstk], result);
			}
			break;
		case 84: /* sgn */
			if (stk[nstk]>=0) stk[nstk] = 1;
			else stk[nstk] = -1;
			break;
		case 85: /* sin */
			stk[nstk] = sin(stk[nstk]);
			break;
		case 86: /* sqr */
			stk[nstk] = stk[nstk] * stk[nstk];
			break;
		case 87: /* sqrt */
			stk[nstk] = sqrt(stk[nstk]);
			break;
		case 88: /* tan */
			stk[nstk] = tan(stk[nstk]);
			break;
		case 89: /* tdepth */
			*otyp = 1;
			g_get_xy(&xx,&yy);
			g_measure(stk_str[nstk],&x1,&x2,&y2,&y1);
			stk[nstk] = y1;
			break;
		case 90: /* theight */
			*otyp = 1;
			g_get_xy(&xx,&yy);
			g_measure(stk_str[nstk],&x1,&x2,&y2,&y1);
			stk[nstk] = y2;
			break;
		case 91: /* time$ */
			{
				*otyp = 2;
				time_t today;
				time(&today);
				ncpy(sbuf,ctime(&today)+11,9);
				setdstr(&stk_str[++nstk],sbuf);
			}
			break;
		case 60+FN_FILE: /* file$ */
			*otyp = 2;
			{
				string tmp_s = getGLERunInstance()->getScript()->getLocation()->getMainName();
				setdstr(&stk_str[++nstk], tmp_s.c_str());
			}
			break;
		case 60+FN_PATH: /* path$ */
			*otyp = 2;
			setdstr(&stk_str[++nstk], getGLERunInstance()->getScript()->getLocation()->getDirectory().c_str());
			break;
		case 60+FN_FONT:
			*otyp = 1;
			stk[nstk] = check_has_font(stk_str[nstk]);
			break;
		case 92: /* twidth */
			*otyp = 1;
			g_measure(stk_str[nstk],&x1,&x2,&y1,&y2);
			stk[nstk] = x2-x1;
			break;
		case 93: /* val */
			*otyp = 1;
			stk[nstk] = string_to_number(stk_str[nstk]);
			break;
		case 94: /* width of named object */
			*otyp = 1;
			getGLERunInstance()->name_to_size(stk_str[nstk], &x1, &y1);
			stk[nstk] = x1;
			break;
		case 95: /* xend */
			*otyp = 1;
			stk[++nstk] = tex_xend();
			break;
		case 96: /* xgraph */
			*otyp = 1;
			stk[nstk] = graph_xgraph(stk[nstk]);
			break;
		case 97: /* xmax */
			break;
		case 98: /* xmin */
			break;
		case 99: /* xpos */
			*otyp = 1;
			g_get_xy(&xx,&yy);
			stk[++nstk] = xx;
			break;
		case 100: /* yend */
			stk[++nstk] = tex_yend();
			*otyp = 1;
			break;
		case 101: /* ygraph */
			stk[nstk] = graph_ygraph(stk[nstk]);
			*otyp = 1;
			break;
		case 102: /* ymax */
			break;
		case 103: /* ymin */
			break;
		case 104: /* ypos */
			g_get_xy(&xx,&yy);
			*otyp = 1;
			stk[++nstk] = yy;
			break;
		case 105: /* CVTGRAY(.5) */
			{
				GLERC<GLEColor> color(new GLEColor(stk[nstk]));
				stk[nstk] = color->getDoubleEncoding();
			}
			break;
		case 106: /* CVTINT(2) */
			*otyp = 1;
			both.l[0] = (int) floor(stk[nstk]);
			both.l[1] = 0;
			memcpy(&stk[nstk],&both.d,sizeof(double));
			break;
		case 108: /* CVTMARKER(m$) */
			*otyp = 1;
			/* *** DEBUG *** */
			//printf("\nCase 108\n");
			both.l[0] = pass_marker(stk_str[nstk]);
			both.l[1] = 0;
			memcpy(&stk[nstk],&both.d,sizeof(double));
			break;
		case 110: /* CVTFONT(m$) */
			*otyp = 1;
			/* *** DEBUG *** */
			//printf("\nCase 110\n");
			both.l[0] = pass_font(stk_str[nstk]);
			both.l[1] = 0;
			memcpy(&stk[nstk],&both.d,sizeof(double));
			break;
		case 60+FN_JUSTIFY: /* JUSTIFY(m$) */
			*otyp = 1;
			both.l[0] = pass_justify(stk_str[nstk]);
			both.l[1] = 0;
			memcpy(&stk[nstk], &both.d, sizeof(double));
			break;         
		case 109: /* CVTCOLOR(c$) */
			*otyp = 1;
			{
				GLERC<GLEColor> color(pass_color_var(stk_str[nstk]));
				stk[nstk] = color->getDoubleEncoding();
			}
			break;
		case 107: /* RGB(.4,.4,.2) */
			{
				GLERC<GLEColor> color(new GLEColor(stk[nstk-2], stk[nstk-1], stk[nstk]));
				nstk -= 2;
				stk[nstk] = color->getDoubleEncoding();
			}
			break;
		case 140: /* RGB255(.4,.4,.2) */
			{
				GLERC<GLEColor> color(new GLEColor());
				color->setRGB255(stk[nstk-2], stk[nstk-1], stk[nstk]);
				nstk -= 2;
				stk[nstk] = color->getDoubleEncoding();
			}
			break;
		case 60+FN_RGBA:
			{
				GLERC<GLEColor> color(new GLEColor(stk[nstk-3], stk[nstk-2], stk[nstk-1], stk[nstk]));
				nstk -= 3;
				stk[nstk] = color->getDoubleEncoding();
			}
			break;
		case 60+FN_RGBA255:
			{
				GLERC<GLEColor> color(new GLEColor());
				color->setRGBA255(stk[nstk-3], stk[nstk-2], stk[nstk-1], stk[nstk]);
				nstk -= 3;
				stk[nstk] = color->getDoubleEncoding();
			}
			break;
		case 60+FN_NDATA: /* Number of datapoints in a dateset */
			*otyp = 1;
			i = atoi(stk_str[nstk]+1);
			if (i <= 0 || i >= MAX_NB_DATA || dp[i] == NULL)
				throw g_format_parser_error("dataset d%d not defined", i);
			else
				stk[nstk] = dp[i]->np;
			break;
		case 60+FN_DATAXVALUE: /* X value in a dateset */
			*otyp = 1; nstk -= 1;
			i = atoi(stk_str[nstk]+1);
			j = (int) stk[nstk+1];
			if (i <= 0 || i >= MAX_NB_DATA || dp[i] == NULL)
				throw g_format_parser_error("dataset d%d not defined", i);
			else
				if (j <= 0 || j > (int)dp[i]->np) {
					throw g_format_parser_error("index out of range: %d (1 ... %d)", j, dp[i]->np);
				} else {
					GLEArrayImpl* array = dp[i]->getDimData(0);
					if (array != 0) {
						stk[nstk] = array->getDouble(j-1);
					}
				}
			break;
		case 60+FN_DATAYVALUE: /* Y value in a dateset */
			*otyp = 1; nstk -= 1;
			i = atoi(stk_str[nstk]+1);
			j = (int) stk[nstk+1];
			if (i <= 0 || i >= MAX_NB_DATA || dp[i] == NULL)
				throw g_format_parser_error("dataset d%d not defined", i);
			else
				if (j <= 0 || j > (int)dp[i]->np) {
					throw g_format_parser_error("index out of range: %d (1 ... %d)", j, dp[i]->np);
				} else {
					GLEArrayImpl* array = dp[i]->getDimData(1);
					if (array != 0) {
						stk[nstk] = array->getDouble(j-1);
					}
				}
			break;
		case 60+FN_XG3D:
			stk[nstk - 2] = xg3d(stk[nstk - 2], stk[nstk - 1], stk[nstk]);
            nstk -= 2;
			break;
		case 60+FN_YG3D:
			stk[nstk - 2] = yg3d(stk[nstk - 2], stk[nstk - 1], stk[nstk]);
            nstk -= 2;
			break;
		default:  /* User function, LOCAL_START_INDEX..nnn , or error */
			/* Is it a user defined function */
			if (*(pcode+c) >= LOCAL_START_INDEX) {
				/*
				pass the address of some numbers
				pass address of variables if possible
				*/
				getGLERunInstance()->sub_call(*(pcode+c)-LOCAL_START_INDEX,stk,stk_str,&nstk,otyp);
			} else {
				g_throw_parser_error("unrecognized byte code expression");
			}
		break;
	  }
	}
}

GLESub* eval_subroutine_call(int *pcode, int *cp, int* otyp) throw(ParserError) {
	if (*(pcode+(*cp)++) != 1) {
		(*cp)--;
		gprint("PCODE, Expecting expression, v=%ld cp=%d \n", *(pcode + (*cp)), *cp);
		return NULL;
	}
	int plen = pcode[(*cp)++];
	eval_pcode_loop(pcode + (*cp), plen-1, otyp);
	int sub_code = pcode[(*cp) + plen - 1];
	GLESub* result = NULL;
	if (sub_code >= LOCAL_START_INDEX) {
		result = sub_get(sub_code - LOCAL_START_INDEX);
	}
	*cp = *cp + plen;
	return result;
}

void eval_do_object_block_call(GLESub* sub, GLEObjectDO* obj) throw(ParserError) {
	int otyp = 1;
	GLEObjectDOConstructor* cons = obj->getConstructor();
	obj->makePropertyStore();
	GLEArrayImpl* arr = obj->getProperties()->getArray();
	int first = 0;
	int offset = nstk - sub->getNbParam() + 1;
	if (cons->isSupportScale()) {
		// First two arguments are width and height
		arr->setDouble(0, stk[offset+0]);
		arr->setDouble(1, stk[offset+1]);
		first += 2;
	}
	for (int i = first; i < sub->getNbParam(); i++) {
		if (sub->getParamType(i) == 1) {
			ostringstream dstr;
			dstr << stk[offset+i];
			arr->setObject(i, new GLEString(dstr.str()));
		} else {
			GLEString* str_i = new GLEString(stk_str[offset+i]);
			str_i->addQuotes();
			arr->setObject(i, str_i);
		}
	}
	getGLERunInstance()->sub_call(sub->getIndex(), stk, stk_str, &nstk, &otyp);
	nstk--;
	if (nstk < 0) {
		nstk = 0;
	}
}

void eval(int *pcode, int *cp, double *oval, const char **ostr, int *otyp) throw(ParserError) {
	/*
	..
	evaluate an expression
	..
	pcode......a pointer to the pcode to execute
	cp.........Current point in this line of pcode
	oval.......place to put result number
	ostr.......place to put result string
	otyp.......place to put result type, 1=num, 2=str
	*/
	if (ostr != NULL) {
		/* make sure output string is valid */
		*ostr = "";
	}
	if (pcode[(*cp)] == 8) {
		/*  Single constant  */
		union {double d; int l[2];} both;
		both.l[0] = *(pcode+ ++(*cp));
		both.l[1] = 0;
		dbg gprint("Constant %ld \n",both.l[0]);
		memcpy(oval,&both.d,sizeof(both.d));
		++(*cp);
		return;
	}
	if (pcode[(*cp)++] != 1) {
		(*cp)--;
		gprint("PCODE, Expecting expression, v=%ld cp=%d \n", *(pcode + (*cp)), *cp);
		return;
	}
	int plen = pcode[(*cp)++];
	eval_pcode_loop(pcode + (*cp), plen, otyp);
	dbg gprint("RESULT ISa ==== %d [1] %f   [nstk] %f \n",nstk,stk[1],stk[nstk]);
	*oval = 0;
	if (*otyp == 1) {
		/* this is a number */
		*oval = stk[nstk];
		dbg gprint("Evaluated number = {%f} \n",*oval);
	} else if  (*otyp == 2  && stk_str[nstk] != NULL) {
		if (ostr != NULL) {
			*ostr = stk_str[nstk];
			dbg gprint("Evaluated string = {%s} \n", *ostr);
		}
	} else {
		/*
		function call
		gprint("ERROR in TYPE specification in EVAL otype=%d\n",*otyp);
		gprint("nstk=%d  cde=%d\n",nstk,cde);
		*/
	}
	dbg gprint("RESULT ISb ==== %d [1] %f   [nstk] %f \n",nstk,stk[1],stk[nstk]);
	dbg gprint("oval %g \n",*oval);
	nstk--;
	if (nstk < 0) {
 		gprint("Stack stuffed up in EVAL %d \n",nstk);
		gprint("oval=%f  ostr=%s otype=%d\n",*oval,*ostr,*otyp);
		nstk = 0;
	}
	*cp = *cp + plen;
}

void debug_polish(int *pcode,int *zcp)
{
	int *cp,cpval;
	int plen,c,cde;
	cpval = *zcp;
	cp = &cpval;
	if (*(pcode+(*cp)++)!=1) {
		gprint("Expecting expression, v=%d \n",(int) *(pcode+--(*cp)) );
		return;
	}
	plen = *(pcode+*(cp));
	gprint("Expression length %d current point %d \n",plen,(int) *cp);
	if (plen>1000) gprint("Expession is suspiciously int %d \n",plen);
	for (c=(*cp)+1;(c-*cp)<=plen;c++) {
	  cde = *(pcode+c);
	gprint("Code=%d ",cde);
		if (cde==0) {
			gprint("# ZERO \n");
		} else if (cde==1) {
			gprint("# Expression, length ??? \n");
			c++;
		} else if (cde==2) {
			gprint("# Floating point number %8x \n",*(pcode+(++c)));
			c++;	/* because it's a DOUBLE which is a quad word */
		} else if (cde==3) {
			gprint("# Variable \n");  c++;
		} else if (cde==4) {
			gprint("# String Variable \n"); c++;
		} else if (cde==5) {
			c++;
			gprint("# String constant {%s} \n",eval_str(pcode,&c));
		} else if (cde<29) {
			gprint("# Binary operator {%s} \n",binop[cde-10]);
		} else if (cde<49) {
			gprint("# Binary string op {%s} \n",binop[cde-30]);
		} else if (cde<LOCAL_START_INDEX) {
			gprint("# Built in function (with salt) {%s} \n",keywfn[cde-60].word);
		} else {
			gprint("# User defined function %d \n",cde);
		}

	}
}

char *eval_str(int *pcode,int *plen)
{
	char *s;
	int sl;
	s = (char *) (pcode+*plen);
	sl = strlen(s)+1;
	sl = ((sl + 3) & 0xfffc);
	*plen = *plen + sl/4 - 1;
	return s;
}

void setdstr(char **s, const char *in)
{
	if (*s != NULL) myfree(*s);
	*s = sdup(in);
}

void setsstr(char **s, const char *in)
{
	if (*s != NULL) myfree(*s);
	*s = (char*)in;
}

void gle_as_a_calculator_eval(GLEPolish& polish, const string& line) {
	try {
		string value;
		polish.eval_string(line.c_str(), &value, true);
		cout << "  " << value << endl;
	} catch (ParserError err) {
		// do not use the gprint version as no GLE file is "active"
		output_error_cerr(err);
	}
}

void gle_as_a_calculator(vector<string>* exprs) {
	g_select_device(GLE_DEVICE_DUMMY);
	g_clear();
	sub_clear(false);
	clear_run();
	f_init();
	var_def("PI", GLE_PI);
	GLEPolish polish;
	polish.initTokenizer();
	string line;
	if (exprs != NULL) {
		for (vector<string>::size_type i = 0; i < exprs->size(); i++) {
			cout << "> " << (*exprs)[i] << endl;
			gle_as_a_calculator_eval(polish, (*exprs)[i]);
		}
	} else {
		while (true) {
			cout << "> "; fflush(stdout);
			ReadFileLineAllowEmpty(cin, line);
			str_trim_both(line);
			if (line == "") break;
			gle_as_a_calculator_eval(polish, line);
		}
	}
}
