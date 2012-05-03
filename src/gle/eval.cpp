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

int stk_var[100];
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

void setEvalStack(GLEArrayImpl* stk, int pos, double value) {
	stk->ensure(pos + 1);
	stk->setDouble(pos, value);
}

void setEvalStack(GLEArrayImpl* stk, int pos, const char* value) {
	stk->ensure(pos + 1);
	stk->setObject(pos, new GLEString(value));
}

void setEvalStack(GLEArrayImpl* stk, int pos, int value) {
	stk->ensure(pos + 1);
	stk->setDouble(pos, (double)value);
}

void setEvalStack(GLEArrayImpl* stk, int pos, GLEString* value) {
	stk->ensure(pos + 1);
	stk->setObject(pos, value);
}

void setEvalStackBool(GLEArrayImpl* stk, int pos, bool value) {
	stk->ensure(pos + 1);
	stk->setBool(pos, value);
}

double getEvalStackDouble(GLEArrayImpl* stk, int pos) {
	stk->checkType(pos, GLEObjectTypeDouble);
	return stk->getDouble(pos);
}

char* getEvalStackString(GLEArrayImpl* stk, int pos) {
	return (char*)"";
}

void complain_operator_type(int op, int type) {
	std::ostringstream msg;
	msg << "operator " << gle_operator_to_string(op) << " does not apply to type '" << gle_object_type_to_string((GLEObjectType)type) << "'";
	g_throw_parser_error(msg.str());
}

void eval_binary_operator_string(GLEArrayImpl* stk, int op, GLEString* a, GLEString* b) {
	switch (op) {
		case BIN_OP_PLUS:
			setEvalStack(stk, nstk - 1, a->concat(b));
			break;
		case BIN_OP_EQUALS:
			setEvalStackBool(stk, nstk - 1, a->equalsI(b));
			break;
		case BIN_OP_LT:
			setEvalStackBool(stk, nstk - 1, a->strICmp(b) < 0);
			break;
		case BIN_OP_LE:
			setEvalStackBool(stk, nstk - 1, a->strICmp(b) <= 0);
			break;
		case BIN_OP_GT:
			setEvalStackBool(stk, nstk - 1, a->strICmp(b) > 0);
			break;
		case BIN_OP_GE:
			setEvalStackBool(stk, nstk - 1, a->strICmp(b) >= 0);
			break;
		case BIN_OP_NOT_EQUALS:
			setEvalStackBool(stk, nstk - 1, !a->equalsI(b));
			break;
		case BIN_OP_DOT:
			{
				GLERC<GLEString> dot(new GLEString("."));
				GLERC<GLEString> temp(a->concat(dot.get()));
				setEvalStack(stk, nstk - 1, temp->concat(b));
			}
			break;
	}
}

void eval_binary_operator_double(GLEArrayImpl* stk, int op, double a, double b) {
	switch (op) {
		case BIN_OP_PLUS:
			setEvalStack(stk, nstk - 1, a + b);
			break;
		case BIN_OP_MINUS:
			setEvalStack(stk, nstk - 1, a - b);
			break;
		case BIN_OP_MULTIPLY:
			setEvalStack(stk, nstk - 1, a * b);
			break;
		case BIN_OP_DIVIDE:
			// do not test on divide by zero, otherwise "let"
			// cannot plot functions with divide by zero anymore
			setEvalStack(stk, nstk - 1, a / b);
			break;
		case BIN_OP_POW:
			setEvalStack(stk, nstk - 1, pow(a, b));
			break;
		case BIN_OP_EQUALS:
			setEvalStackBool(stk, nstk - 1, a == b);
			break;
		case BIN_OP_LT:
			setEvalStackBool(stk, nstk - 1, a < b);
			break;
		case BIN_OP_LE:
			setEvalStackBool(stk, nstk - 1, a <= b);
			break;
		case BIN_OP_GT:
			setEvalStackBool(stk, nstk - 1, a > b);
			break;
		case BIN_OP_GE:
			setEvalStackBool(stk, nstk - 1, a >= b);
			break;
		case BIN_OP_NOT_EQUALS:
			setEvalStackBool(stk, nstk - 1, a != b);
			break;
		case BIN_OP_MOD:
			setEvalStack(stk, nstk - 1, gle_round_int(a) % gle_round_int(b));
			break;
		default:
			complain_operator_type(op, GLEObjectTypeDouble);
			break;
	}
}

void eval_binary_operator_bool(GLEArrayImpl* stk, int op, bool a, bool b) {
	switch (op) {
		case BIN_OP_AND:
			setEvalStackBool(stk, nstk - 1, a && b);
			break;
		case BIN_OP_OR:
			setEvalStackBool(stk, nstk - 1, a || b);
			break;
		default:
			complain_operator_type(op, GLEObjectTypeDouble);
			break;
	}
}

void eval_binary_operator(GLEArrayImpl* stk, int op) {
	// a OP b
	GLEMemoryCell* a = stk->get(nstk - 1);
	GLEMemoryCell* b = stk->get(nstk);
	int a_type = gle_memory_cell_type(a);
	int b_type = gle_memory_cell_type(b);
	if (a_type == b_type) {
		switch (a_type) {
			case GLEObjectTypeDouble:
				eval_binary_operator_double(stk, op, a->Entry.DoubleVal, b->Entry.DoubleVal);
				break;
			case GLEObjectTypeString:
				eval_binary_operator_string(stk, op, (GLEString*)a->Entry.ObjectVal, (GLEString*)b->Entry.ObjectVal);
				break;
			case GLEObjectTypeBool:
				eval_binary_operator_bool(stk, op, a->Entry.BoolVal, b->Entry.BoolVal);
				break;
			default:
				complain_operator_type(op, a_type);
				break;
		}
	} else if (op == BIN_OP_PLUS && (a_type == GLEObjectTypeString || b_type == GLEObjectTypeString)) {
		GLERC<GLEString> a_str(stk->getString(nstk - 1));
		GLERC<GLEString> b_str(stk->getString(nstk));
		eval_binary_operator_string(stk, op, a_str.get(), b_str.get());
	} else {
		std::ostringstream msg;
		msg << "operator " << gle_operator_to_string(op)
			<< " does not apply to types '" << gle_object_type_to_string((GLEObjectType)a_type)
			<< "' and '" << gle_object_type_to_string((GLEObjectType)b_type) << "'";
		g_throw_parser_error(msg.str());
	}
	nstk--;
}

void eval_pcode_loop(GLEArrayImpl* stk, int *pcode, int plen) throw(ParserError) {
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
			both.l[0] = *(pcode+(++c));
			both.l[1] = *(pcode+(++c));
			setEvalStack(stk, ++nstk, both.d);
 			dbg gprint("Got float %f %d %f \n",getEvalStackDouble(stk, nstk),nstk,*(pcode+(c)));
			break;
		case 3: /* Floating_point variable number follows */
		case 4: /* string variable number follows */
			i = *(pcode + (++c));
			j = ++nstk;
			stk->ensure(j + 1);
			getVarsInstance()->get(i, stk->get(j));
			break;
		case 5: /* Null terminated string follows (int alligned) */
			c++; nstk++;
			setEvalStack(stk, nstk, eval_str(pcode,&c));
			break;
		/*
			Binary operators 10..29 -----------------------
		*/
		case BIN_OP_PLUS + BINARY_OPERATOR_OFFSET:
		case BIN_OP_MINUS + BINARY_OPERATOR_OFFSET:
		case BIN_OP_MULTIPLY + BINARY_OPERATOR_OFFSET:
		case BIN_OP_DIVIDE + BINARY_OPERATOR_OFFSET:
		case BIN_OP_POW + BINARY_OPERATOR_OFFSET:
		case BIN_OP_EQUALS + BINARY_OPERATOR_OFFSET:
		case BIN_OP_LT + BINARY_OPERATOR_OFFSET:
		case BIN_OP_LE + BINARY_OPERATOR_OFFSET:
		case BIN_OP_GT + BINARY_OPERATOR_OFFSET:
		case BIN_OP_GE + BINARY_OPERATOR_OFFSET:
		case BIN_OP_NOT_EQUALS + BINARY_OPERATOR_OFFSET:
		case BIN_OP_AND + BINARY_OPERATOR_OFFSET:
		case BIN_OP_OR + BINARY_OPERATOR_OFFSET:
		case BIN_OP_MOD + BINARY_OPERATOR_OFFSET:
		case BIN_OP_DOT + BINARY_OPERATOR_OFFSET:
			eval_binary_operator(stk, pcode[c] - BINARY_OPERATOR_OFFSET);
			break;

		/* String Binary operators 30..49 ----------------------- */
		case 31:  /* + */
			nstk--;
			{
				char* result = NULL;
				if (getEvalStackString(stk, nstk) != NULL && getEvalStackString(stk, nstk+1) != NULL) {
					result = (char*)malloc((strlen(getEvalStackString(stk, nstk))+strlen(getEvalStackString(stk, nstk+1))+1)*sizeof(char));
					strcpy(result, getEvalStackString(stk, nstk));
					strcat(result, getEvalStackString(stk, nstk+1));
				} else {
					result = sdup("");
				}
				setEvalStack(stk, nstk, result);
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
			nstk--;
			setEvalStack(stk, nstk, str_i_equals(getEvalStackString(stk, nstk), getEvalStackString(stk, nstk+1)));
			break;
		case 37:  /* <   */
			nstk--;
			setEvalStack(stk, nstk, str_i_cmp(getEvalStackString(stk, nstk), getEvalStackString(stk, nstk+1)) < 0);
			break;
		case 38:  /* <=  */
			nstk--;
			setEvalStack(stk, nstk, str_i_cmp(getEvalStackString(stk, nstk), getEvalStackString(stk, nstk+1)) <= 0);
			break;
		case 39:  /* >   */
			nstk--;
			setEvalStack(stk, nstk, str_i_cmp(getEvalStackString(stk, nstk), getEvalStackString(stk, nstk+1)) > 0);
			break;
		case 40:  /* >=  */
			nstk--;
			setEvalStack(stk, nstk, str_i_cmp(getEvalStackString(stk, nstk), getEvalStackString(stk, nstk+1)) >= 0);
			break;
		case 41:  /* <>  */
			nstk--;
			setEvalStack(stk, nstk, !str_i_equals(getEvalStackString(stk, nstk), getEvalStackString(stk, nstk+1)));
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
			nstk--;
			{
				char* result = NULL;
				if (getEvalStackString(stk, nstk) != NULL && getEvalStackString(stk, nstk+1) != NULL) {
					result = (char*)malloc((strlen(getEvalStackString(stk, nstk))+strlen(getEvalStackString(stk, nstk+1))+2)*sizeof(char));
					strcpy(result, getEvalStackString(stk, nstk));
					strcat(result, ".");
					strcat(result, getEvalStackString(stk, nstk+1));
				} else {
					result = sdup("");
				}
				setEvalStack(stk, nstk, result);
			}
			break;

	    /* look in fn.c and start indexes with 1 */
		/* Built in functions 60..199 ----------------------------- */
		/* note add 60 to the index in fn.cpp */
		case 61: /* unary plus */
			break;
		case 62: /* unary minus */
			setEvalStack(stk, nstk, -getEvalStackDouble(stk, nstk));
			break;
		case 63: /* abs */
			setEvalStack(stk, nstk, fabs(getEvalStackDouble(stk, nstk)));
			break;
		case 64: /* atan */
			setEvalStack(stk, nstk, atan(getEvalStackDouble(stk, nstk)));
			break;
		case 113: /* ACOS 53*/
			setEvalStack(stk, nstk, acos(getEvalStackDouble(stk, nstk)));
			break;
		case 114: /* ASIN 54*/
			setEvalStack(stk, nstk, asin(getEvalStackDouble(stk, nstk)));
			break;
		case 65: /* cos 5*/
			setEvalStack(stk, nstk, cos(getEvalStackDouble(stk, nstk)));
			break;
		case 116: /* ACOT 56*/
			setEvalStack(stk, nstk, 1.0/atan(getEvalStackDouble(stk, nstk)));
			break;
		case 117: /*ASEC 57*/
			setEvalStack(stk, nstk, 1.0/acos(getEvalStackDouble(stk, nstk)));
			break;
		case 118: /*ACSC 58*/
			setEvalStack(stk, nstk, 1.0/asin(getEvalStackDouble(stk, nstk)));
			break;
		case 119: /*cot 59*/
			setEvalStack(stk, nstk, 1.0/tan(getEvalStackDouble(stk, nstk)));
			break;
		case 120: /*sec 60*/
			setEvalStack(stk, nstk, 1.0/cos(getEvalStackDouble(stk, nstk)));
			break;
		case 121: /*csc 61*/
			setEvalStack(stk, nstk, 1.0/sin(getEvalStackDouble(stk, nstk)));
			break;
		case 122: /* cosh 62*/
			setEvalStack(stk, nstk, cosh(getEvalStackDouble(stk, nstk)));
			break;
		case 123: /* sinh 63*/
			setEvalStack(stk, nstk, sinh(getEvalStackDouble(stk, nstk)));
			break;
		case 124: /* tanh 64*/
			setEvalStack(stk, nstk, tanh(getEvalStackDouble(stk, nstk)));
			break;
		case 125: /* coth 65*/
			setEvalStack(stk, nstk, 1.0/tanh(getEvalStackDouble(stk, nstk)));
			break;
		case 126: /* sech 66*/
			setEvalStack(stk, nstk, 1.0/cosh(getEvalStackDouble(stk, nstk)));
			break;
		case 127: /* csch 67*/
			setEvalStack(stk, nstk, 1.0/sinh(getEvalStackDouble(stk, nstk)));
			break;
		#ifdef HAVE_INVERSE_HYP
		case 128: /* acosh 68*/
			setEvalStack(stk, nstk, acosh(getEvalStackDouble(stk, nstk)));
			break;
		case 129: /* asinh 69*/
			setEvalStack(stk, nstk, asinh(getEvalStackDouble(stk, nstk)));
			break;
		case 130: /* atanh 70*/
			setEvalStack(stk, nstk, atanh(getEvalStackDouble(stk, nstk)));
			break;
		case 131: /* acoth 71*/
			setEvalStack(stk, nstk, 1.0/atanh(getEvalStackDouble(stk, nstk)));
			break;
		case 132: /* asech 72*/
			setEvalStack(stk, nstk, 1.0/acosh(getEvalStackDouble(stk, nstk)));
			break;
		case 133: /* acsch 73*/
			setEvalStack(stk, nstk, 1.0/asinh(getEvalStackDouble(stk, nstk)));
			break;
		#endif
		case 134: /* todeg 74*/
			setEvalStack(stk, nstk, getEvalStackDouble(stk, nstk)*180.0/GLE_PI);
			break;
		case 135: /* torad 75*/
			setEvalStack(stk, nstk, getEvalStackDouble(stk, nstk)*GLE_PI/180.0);
			break;
		case 60+FN_EVAL: /* eval */
			polish_eval(getEvalStackString(stk, nstk), &xx);
			setEvalStack(stk, nstk, xx);
			break;
		case 60+FN_ARG: /* arg */
			setEvalStack(stk, nstk, eval_get_extra_arg_f((int)getEvalStackDouble(stk, nstk)));
			break;
		case 60+FN_ARGS: /* arg$ */
			setEvalStack(stk, nstk, sdup((char*)eval_get_extra_arg_s((int)getEvalStackDouble(stk, nstk))));
			break;
		case 60+FN_NARGS: /* narg */
			setEvalStack(stk, ++nstk, g_CmdLine.getNbExtraArgs());
			break;
		case 60+FN_MIN: /* min */
			nstk--;
			if (getEvalStackDouble(stk, nstk+1) < getEvalStackDouble(stk, nstk)) setEvalStack(stk, nstk, getEvalStackDouble(stk, nstk+1));
			break;
		case 60+FN_MAX: /* max */
			nstk--;
			if (getEvalStackDouble(stk, nstk+1) > getEvalStackDouble(stk, nstk)) setEvalStack(stk, nstk, getEvalStackDouble(stk, nstk+1));
			break;
		case 60+FN_SDIV: /* sdiv */
			if (getEvalStackDouble(stk, nstk) == 0.0) setEvalStack(stk, nstk-1, 0.0);
			else setEvalStack(stk, nstk-1, getEvalStackDouble(stk, nstk-1) / getEvalStackDouble(stk, nstk));
			nstk--;
			break;
		case 60+FN_XBAR: /* bar x position */
			nstk -= 1;
			setEvalStack(stk, nstk, graph_bar_pos(getEvalStackDouble(stk, nstk), gle_round_int(getEvalStackDouble(stk, nstk+1)), 1));
			break;
		case 60+FN_XY2ANGLE:
			nstk -= 1;
			xy_polar(getEvalStackDouble(stk, nstk), getEvalStackDouble(stk, nstk+1), &x1, &y1);
			setEvalStack(stk, nstk, y1);
			break;
		case 60+FN_ATAN2:
			nstk -= 1;
			setEvalStack(stk, nstk, myatan2(getEvalStackDouble(stk, nstk), getEvalStackDouble(stk, nstk+1)));
			break;
		case 60+FN_ISNAME:
			setEvalStack(stk, nstk, getGLERunInstance()->is_name(getEvalStackString(stk, nstk)) ? 1.0 : 0.0);
			break;
		case 137: /* pointx */
			{
				GLEPoint pt;
				getGLERunInstance()->name_to_point(getEvalStackString(stk, nstk), &pt);
				setEvalStack(stk, nstk, pt.getX());
			}
			break;
		case 138: /* pointy */
			{
				GLEPoint pt;
				getGLERunInstance()->name_to_point(getEvalStackString(stk, nstk), &pt);
				setEvalStack(stk, nstk, pt.getY());
			}
			break;
		case 139: /* format$ */
			nstk--;
			format_number_to_string(sbuf, getEvalStackString(stk, nstk+1), getEvalStackDouble(stk, nstk));
			setEvalStack(stk, nstk, sdup(sbuf));
			break;
		case 60+FN_GETENV: /* getenv */
			{
				string result;
				GLEGetEnv(string(getEvalStackString(stk, nstk)), result);
				setEvalStack(stk, nstk, sdup(result.c_str()));
			}
			break;
		case 66: /* date$ */
			{
				time_t today;
				time(&today);
				strcpy(sbuf2,ctime(&today));
				strcpy(sbuf,sbuf2);
				strcpy(sbuf+11,sbuf2+20);
				sbuf[strlen(sbuf)-1] = 0;
				// FIXME STACK
				// setdstr(&getEvalStackString(stk, ++nstk),sbuf);
			}
			break;
		case 111: /* device$ */
			g_get_type(sbuf);
			// FIXME STACK
			// setdstr(&getEvalStackString(stk, ++nstk),sbuf);
			break;
		case 115: /* feof(chan) */
			setEvalStack(stk, nstk, f_eof((int) getEvalStackDouble(stk, nstk)));
			break;
		case 67: /* exp */
			setEvalStack(stk, nstk, exp(getEvalStackDouble(stk, nstk)));
			break;
		case 68: /* fix*/
			setEvalStack(stk, nstk, floor(getEvalStackDouble(stk, nstk)));
			break;
		case 69: /* height of named object */
			getGLERunInstance()->name_to_size(getEvalStackString(stk, nstk), &x1, &y1);
			setEvalStack(stk, nstk, y1);
			break;
		case 70: /* int (??int) */
			setEvalStack(stk, nstk, floor(fabs(getEvalStackDouble(stk, nstk)))
				*( (getEvalStackDouble(stk, nstk)>=0)?1:-1 ) );
			break;
		case 112: /* CHR$() */
			sprintf(sbuf,"%c",(int) getEvalStackDouble(stk, nstk));
			// FIXME STACK
			// setdstr(&getEvalStackString(stk, nstk),sbuf);
			break;
		case 71: /* left$ */
			{
				/*int i1 = (int)getEvalStackDouble(stk, nstk);
				int len = strlen(getEvalStackString(stk, nstk-1));
				if (i1 < 0) i1 = 0;
				if (i1 > len) i1 = len;
				char* result = (char*)malloc((i1+1)*sizeof(char));*/
				// FIXME STACK
				// ncpy(result, getEvalStackString(stk, nstk-1), i1);
				// setsstr(&getEvalStackString(stk, --nstk), result);
			}
			break;
		case 72: /* len */
			setEvalStack(stk, nstk, (int)strlen(getEvalStackString(stk, nstk)));
			break;
		case 73: /* log */
			setEvalStack(stk, nstk, log(getEvalStackDouble(stk, nstk)));
			break;
		case 74: /* log10 */
			setEvalStack(stk, nstk, log10(getEvalStackDouble(stk, nstk)));
			break;
		case 75: /* not */
			setEvalStack(stk, nstk, !(getEvalStackDouble(stk, nstk)));
			break;
		case 76: /* num$ */
			sprintf(sbuf,"%g",getEvalStackDouble(stk, nstk));
			setEvalStack(stk, nstk, sdup(sbuf));
			break;
		case 77: /* num1$ */
			sprintf(sbuf,"%g ",getEvalStackDouble(stk, nstk));
			setEvalStack(stk, nstk, sdup(sbuf));
			break;
		case 78: /* pageheight */
			g_get_usersize(&xx, &yy);
			setEvalStack(stk, ++nstk, yy);
			break;
		case 79: /* pagewidth */
			g_get_usersize(&xx, &yy);
			setEvalStack(stk, ++nstk, xx);
			break;
		case 80: /* pos */
			i = (int) getEvalStackDouble(stk, nstk);
			if (i<=0) i = 1;
			ss = getEvalStackString(stk, nstk-2);
			ss2 = str_i_str(ss+i-1,getEvalStackString(stk, nstk-1));
			if (ss2!=NULL) 	i = ss2-ss+1;
			else 		i = 0;
			nstk -= 2;
			setEvalStack(stk, nstk, i);
			break;
		case 81: /* right$ */
			{
				int i1 = (int)getEvalStackDouble(stk, nstk)-1;
				int len = strlen(getEvalStackString(stk, nstk-1));
				if (i1 < 0) i1 = 0;
				if (i1 > len) i1 = len;
				char* result = (char*)malloc((len-i1+1)*sizeof(char));
				strcpy(result, getEvalStackString(stk, nstk-1)+i1);
				// FIXME STACK
				// setsstr(&getEvalStackString(stk, --nstk), result);
			}
			break;
		case 82: /* rnd */
			setEvalStack(stk, nstk, ((double) rand()/(double)RAND_MAX)*getEvalStackDouble(stk, nstk));
			break;
		case 83: /* seg$ */
			{
				int i1 = (int)getEvalStackDouble(stk, nstk-1)-1;
				int len = strlen(getEvalStackString(stk, nstk-2));
				if (i1 < 0) i1 = 0;
				if (i1 > len) i1 = len;
				int n1 = (int)getEvalStackDouble(stk, nstk)-i1;
				if (i1+n1 > len) n1 = len-i1;
				if (n1 < 0) n1 = 0;
				char* result = (char*)malloc((n1+1)*sizeof(char));
				ncpy(result, getEvalStackString(stk, nstk-2)+i1, n1);
				nstk -= 2;
				// FIXME STACK
				// setsstr(&getEvalStackString(stk, nstk), result);
			}
			break;
		case 84: /* sgn */
			if (getEvalStackDouble(stk, nstk)>=0) setEvalStack(stk, nstk, 1);
			else setEvalStack(stk, nstk, -1);
			break;
		case 85: /* sin */
			setEvalStack(stk, nstk, sin(getEvalStackDouble(stk, nstk)));
			break;
		case 86: /* sqr */
			setEvalStack(stk, nstk, getEvalStackDouble(stk, nstk) * getEvalStackDouble(stk, nstk));
			break;
		case 87: /* sqrt */
			setEvalStack(stk, nstk, sqrt(getEvalStackDouble(stk, nstk)));
			break;
		case 88: /* tan */
			setEvalStack(stk, nstk, tan(getEvalStackDouble(stk, nstk)));
			break;
		case 89: /* tdepth */
			g_get_xy(&xx,&yy);
			g_measure(getEvalStackString(stk, nstk),&x1,&x2,&y2,&y1);
			setEvalStack(stk, nstk, y1);
			break;
		case 90: /* theight */
			g_get_xy(&xx,&yy);
			g_measure(getEvalStackString(stk, nstk),&x1,&x2,&y2,&y1);
			setEvalStack(stk, nstk, y2);
			break;
		case 91: /* time$ */
			{
				time_t today;
				time(&today);
				ncpy(sbuf,ctime(&today)+11,9);
				setEvalStack(stk, ++nstk, sbuf);
			}
			break;
		case 60+FN_FILE: /* file$ */
			{
				string tmp_s = getGLERunInstance()->getScript()->getLocation()->getMainName();
				setEvalStack(stk, ++nstk, tmp_s.c_str());
			}
			break;
		case 60+FN_PATH: /* path$ */
			setEvalStack(stk, ++nstk, getGLERunInstance()->getScript()->getLocation()->getDirectory().c_str());
			break;
		case 60+FN_FONT:
			setEvalStack(stk, nstk, check_has_font(getEvalStackString(stk, nstk)));
			break;
		case 92: /* twidth */
			g_measure(getEvalStackString(stk, nstk),&x1,&x2,&y1,&y2);
			setEvalStack(stk, nstk, x2-x1);
			break;
		case 93: /* val */
			setEvalStack(stk, nstk, string_to_number(getEvalStackString(stk, nstk)));
			break;
		case 94: /* width of named object */
			getGLERunInstance()->name_to_size(getEvalStackString(stk, nstk), &x1, &y1);
			setEvalStack(stk, nstk, x1);
			break;
		case 95: /* xend */
			setEvalStack(stk, ++nstk, tex_xend());
			break;
		case 96: /* xgraph */
			setEvalStack(stk, nstk, graph_xgraph(getEvalStackDouble(stk, nstk)));
			break;
		case 97: /* xmax */
			break;
		case 98: /* xmin */
			break;
		case 99: /* xpos */
			g_get_xy(&xx,&yy);
			setEvalStack(stk, ++nstk, xx);
			break;
		case 100: /* yend */
			setEvalStack(stk, ++nstk, tex_yend());
			break;
		case 101: /* ygraph */
			setEvalStack(stk, nstk, graph_ygraph(getEvalStackDouble(stk, nstk)));
			break;
		case 102: /* ymax */
			break;
		case 103: /* ymin */
			break;
		case 104: /* ypos */
			g_get_xy(&xx,&yy);
			setEvalStack(stk, ++nstk, yy);
			break;
		case 105: /* CVTGRAY(.5) */
			{
				GLERC<GLEColor> color(new GLEColor(getEvalStackDouble(stk, nstk)));
				setEvalStack(stk, nstk, color->getDoubleEncoding());
			}
			break;
		case 106: /* CVTINT(2) */
			setEvalStack(stk, nstk, (int) floor(getEvalStackDouble(stk, nstk)));
			break;
		case 108: /* CVTMARKER(m$) */
			/* *** DEBUG *** */
			//printf("\nCase 108\n");
			both.l[0] = pass_marker(getEvalStackString(stk, nstk));
			both.l[1] = 0;
			setEvalStack(stk, nstk, both.d);
			break;
		case 110: /* CVTFONT(m$) */
			/* *** DEBUG *** */
			//printf("\nCase 110\n");
			both.l[0] = pass_font(getEvalStackString(stk, nstk));
			both.l[1] = 0;
			setEvalStack(stk, nstk, both.d);
			break;
		case 60+FN_JUSTIFY: /* JUSTIFY(m$) */
			both.l[0] = pass_justify(getEvalStackString(stk, nstk));
			both.l[1] = 0;
			setEvalStack(stk, nstk, both.d);
			break;         
		case 109: /* CVTCOLOR(c$) */
			{
				GLERC<GLEColor> color(pass_color_var(getEvalStackString(stk, nstk)));
				setEvalStack(stk, nstk, color->getDoubleEncoding());
			}
			break;
		case 107: /* RGB(.4,.4,.2) */
			{
				GLERC<GLEColor> color(new GLEColor(getEvalStackDouble(stk, nstk-2), getEvalStackDouble(stk, nstk-1), getEvalStackDouble(stk, nstk)));
				nstk -= 2;
				setEvalStack(stk, nstk, color->getDoubleEncoding());
			}
			break;
		case 140: /* RGB255(.4,.4,.2) */
			{
				GLERC<GLEColor> color(new GLEColor());
				color->setRGB255(getEvalStackDouble(stk, nstk-2), getEvalStackDouble(stk, nstk-1), getEvalStackDouble(stk, nstk));
				nstk -= 2;
				setEvalStack(stk, nstk, color->getDoubleEncoding());
			}
			break;
		case 60+FN_RGBA:
			{
				GLERC<GLEColor> color(new GLEColor(getEvalStackDouble(stk, nstk-3), getEvalStackDouble(stk, nstk-2), getEvalStackDouble(stk, nstk-1), getEvalStackDouble(stk, nstk)));
				nstk -= 3;
				setEvalStack(stk, nstk, color->getDoubleEncoding());
			}
			break;
		case 60+FN_RGBA255:
			{
				GLERC<GLEColor> color(new GLEColor());
				color->setRGBA255(getEvalStackDouble(stk, nstk-3), getEvalStackDouble(stk, nstk-2), getEvalStackDouble(stk, nstk-1), getEvalStackDouble(stk, nstk));
				nstk -= 3;
				setEvalStack(stk, nstk, color->getDoubleEncoding());
			}
			break;
		case 60+FN_NDATA: /* Number of datapoints in a dateset */
			i = atoi(getEvalStackString(stk, nstk)+1);
			if (i <= 0 || i >= MAX_NB_DATA || dp[i] == NULL)
				throw g_format_parser_error("dataset d%d not defined", i);
			else
				setEvalStack(stk, nstk, (int)dp[i]->np);
			break;
		case 60+FN_DATAXVALUE: /* X value in a dateset */
			nstk -= 1;
			i = atoi(getEvalStackString(stk, nstk)+1);
			j = (int) getEvalStackDouble(stk, nstk+1);
			if (i <= 0 || i >= MAX_NB_DATA || dp[i] == NULL)
				throw g_format_parser_error("dataset d%d not defined", i);
			else
				if (j <= 0 || j > (int)dp[i]->np) {
					throw g_format_parser_error("index out of range: %d (1 ... %d)", j, dp[i]->np);
				} else {
					GLEArrayImpl* array = dp[i]->getDimData(0);
					if (array != 0) {
						setEvalStack(stk, nstk, array->getDouble(j-1));
					}
				}
			break;
		case 60+FN_DATAYVALUE: /* Y value in a dateset */
			nstk -= 1;
			i = atoi(getEvalStackString(stk, nstk)+1);
			j = (int) getEvalStackDouble(stk, nstk+1);
			if (i <= 0 || i >= MAX_NB_DATA || dp[i] == NULL)
				throw g_format_parser_error("dataset d%d not defined", i);
			else
				if (j <= 0 || j > (int)dp[i]->np) {
					throw g_format_parser_error("index out of range: %d (1 ... %d)", j, dp[i]->np);
				} else {
					GLEArrayImpl* array = dp[i]->getDimData(1);
					if (array != 0) {
						setEvalStack(stk, nstk, array->getDouble(j-1));
					}
				}
			break;
		case 60+FN_XG3D:
			setEvalStack(stk, nstk - 2, xg3d(getEvalStackDouble(stk, nstk - 2), getEvalStackDouble(stk, nstk - 1), getEvalStackDouble(stk, nstk)));
            nstk -= 2;
			break;
		case 60+FN_YG3D:
			setEvalStack(stk, nstk - 2, yg3d(getEvalStackDouble(stk, nstk - 2), getEvalStackDouble(stk, nstk - 1), getEvalStackDouble(stk, nstk)));
            nstk -= 2;
			break;
		default:  /* User function, LOCAL_START_INDEX..nnn , or error */
			/* Is it a user defined function */
			if (*(pcode+c) >= LOCAL_START_INDEX) {
				/*
				pass the address of some numbers
				pass address of variables if possible
				*/
				getGLERunInstance()->sub_call(*(pcode+c)-LOCAL_START_INDEX, stk, &nstk);
			} else {
				g_throw_parser_error("unrecognized byte code expression");
			}
		break;
	  }
	}
}

GLESub* eval_subroutine_call(GLEArrayImpl* stk, int *pcode, int *cp) throw(ParserError) {
	if (*(pcode+(*cp)++) != 1) {
		(*cp)--;
		gprint("PCODE, Expecting expression, v=%ld cp=%d \n", *(pcode + (*cp)), *cp);
		return NULL;
	}
	int plen = pcode[(*cp)++];
	eval_pcode_loop(stk, pcode + (*cp), plen-1);
	int sub_code = pcode[(*cp) + plen - 1];
	GLESub* result = NULL;
	if (sub_code >= LOCAL_START_INDEX) {
		result = sub_get(sub_code - LOCAL_START_INDEX);
	}
	*cp = *cp + plen;
	return result;
}

void eval_do_object_block_call(GLEArrayImpl* stk, GLESub* sub, GLEObjectDO* obj) throw(ParserError) {
	GLEObjectDOConstructor* cons = obj->getConstructor();
	obj->makePropertyStore();
	GLEArrayImpl* arr = obj->getProperties()->getArray();
	int first = 0;
	int offset = nstk - sub->getNbParam() + 1;
	if (cons->isSupportScale()) {
		// First two arguments are width and height
		arr->setDouble(0, getEvalStackDouble(stk, offset+0));
		arr->setDouble(1, getEvalStackDouble(stk, offset+1));
		first += 2;
	}
	for (int i = first; i < sub->getNbParam(); i++) {
		if (sub->getParamType(i) == 1) {
			ostringstream dstr;
			dstr << getEvalStackDouble(stk, offset+i);
			arr->setObject(i, new GLEString(dstr.str()));
		} else {
			GLEString* str_i = new GLEString(getEvalStackString(stk, offset+i));
			str_i->addQuotes();
			arr->setObject(i, str_i);
		}
	}
	getGLERunInstance()->sub_call(sub->getIndex(), stk, &nstk);
	nstk--;
	if (nstk < 0) {
		nstk = 0;
	}
}

void evalDoConstant(GLEArrayImpl* stk, int *pcode, int *cp)
{
	union {double d; int l[2];} both;
	both.l[0] = *(pcode+ ++(*cp));
	both.l[1] = 0;
	stk->ensure(1);
	stk->setDouble(1, both.d);
	nstk++;
}

void evalCommon(GLEArrayImpl* stk, int *pcode, int *cp) throw(ParserError) {
	if (pcode[(*cp)] == 8) {
		evalDoConstant(stk, pcode, cp);
		*cp = *cp + 1;
	} else {
		if (pcode[(*cp)++] != 1) {
			g_throw_parser_error("pcode error: expected expression");
		}
		int plen = pcode[(*cp)++];
		eval_pcode_loop(stk, pcode + (*cp), plen);
		*cp = *cp + plen;
	}
	if (nstk != 1) {
		g_throw_parser_error("pcode error: stack underflow in eval");
	}
	nstk = 0;
}

GLEMemoryCell* evalGeneric(GLEArrayImpl* stk, int *pcode, int *cp) throw(ParserError) {
	evalCommon(stk, pcode, cp);
	return stk->get(1);
}

GLERC<GLEString> evalStr(GLEArrayImpl* stk, int *pcode, int *cp, bool allowOther) throw(ParserError) {
	GLERC<GLEString> result;
	evalCommon(stk, pcode, cp);
	int type = stk->getType(1);
	if (type == GLEObjectTypeString) {
		result = (GLEString*)stk->getObject(1);
	} else {
		if (allowOther) {
			result = stk->getString(1);
		} else {
			std::ostringstream msg;
			msg << "found type '" << gle_object_type_to_string((GLEObjectType)stk->getType(1)) << "' but expected 'string'";
			g_throw_parser_error(msg.str());
		}
	}
	return result;
}

void eval(GLEArrayImpl* stk, int *pcode, int *cp, double *oval, GLEString **ostr, int *otyp) throw(ParserError) {
	if (ostr != 0) {
		*ostr = 0;
	}
	evalCommon(stk, pcode, cp);
	int type = stk->getType(1);
	if (type == GLEObjectTypeString) {
		*otyp = 2;
		*ostr = (GLEString*)stk->getObject(1);
	} else {
		*otyp = 1;
		*oval = getEvalStackDouble(stk, 1);
	}
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
	GLERC<GLEArrayImpl> stk(new GLEArrayImpl());
	try {
		string value;
		polish.eval_string(stk.get(), line.c_str(), &value, true);
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
