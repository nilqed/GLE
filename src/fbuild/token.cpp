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

#include "basicconf.h"
#include "token.h"
#include "../gle/cutils.h"

void add_tokf(char* pp1,int n,TOKENS tok, int* ntok, char* outbuff, int tj, bool in_quote)
{
	char* token_start;
//	for(vi=1;vi<=*ntok;vi++) printf("(add_tokf) tok[%d]=%s\n",vi,tok[vi]);
	(*ntok)++;
//	tok[*ntok] = outbuff;
	//
	// save the starting position
	//
	token_start = outbuff;
//	printf("tok=%u outbuf=%u\n",tok,outbuff);
//	for(vi=1;vi<=*ntok-1;vi++) printf("(add_tokf) tok[%d]=%s\n",vi,tok[vi]);
//	printf("outbuff=%s pp1=%s\n",outbuff,pp1);
	for (tj=0;tj<n;tj++){
		if (in_quote){
			*(outbuff+tj) = *(pp1+tj);
		}else{
			*(outbuff+tj) = toupper(*(pp1+tj));
		}
		if (*(outbuff+tj) == '"' && *(outbuff+tj-1) != '\\'){
			in_quote = (in_quote ? false :true);
		}
	}
	in_quote = false;
	outbuff = outbuff + (n);
	*(outbuff++) = '\0';
//	printf("a ntok=%d tok[%d]=%s token_start=%s",*ntok,*ntok,tok[*ntok],token_start);
	//fflush(stdout);
	strcpy(tok[*ntok],token_start);
//	printf("(add_tokf) token added tok[%d]=%s\n",*ntok,tok[*ntok]);
//	for(vi=1;vi<=*ntok;vi++) printf("(add_tokf) tok[%d]=%s\n",vi,tok[vi]);
}

static char term_table1[256];
static char term_table2[256];
static char term_table3[256];
static char *term_table;
static int table_loaded;
char *find_non_term();
char *find_non_space();
char *find_term();
int spmode;
/* typedef char (*(*TOKENS)[500]);  */

/*--------------------------------------------------------------------------*/

void token_norm()
{
	if (table_loaded==false) token_init();
	term_table = &term_table1[0];
	spmode = false;
}
void token_space()
{
	if (table_loaded==false) token_init();
	term_table = &term_table2[0];
	spmode = true;
}
void token_equal()
{
	if (table_loaded==false) token_init();
	term_table = &term_table3[0];
	spmode = false;
}
void token_init()
{
	int i;
	term_table = &term_table1[0];
	table_loaded = true;
	{const char *termset=" 	,-+*)(<>=/!^@";
	for (i=0;i<=255;i++) {
		if (strchr(termset,i)!=NULL) {
			term_table1[i]=true;
		}
	}}
	{const char *termset=" 	!";
		for (i=0;i<=255;i++) {
			if (strchr(termset,i)!=NULL) {
				term_table2[i]=true;
			}
		}
	}
	{const char *termset=" 	,+*)(<>=/!^@";
		for (i=0;i<=255;i++) {
			if (strchr(termset,i)!=NULL) {
				term_table3[i]=true;
			}
		}
	}
}

void token(char *lin,TOKENS tok,int *ntok,char *outbuff)
{
	int jj;
	int tj=0;
	char *cp;
	char *p2;
	*ntok = 0;
	bool in_quote = false;
	if (table_loaded==false) token_init();
	cp = lin;
	cp = find_non_space(cp);
//	printf("tok=%u outbuf=%u\n",tok,outbuff);
//	printf("tok[1]=%s\n",tok[1]);
//	for(vi=0;vi<5;vi++) printf("(token)=%d tok[%d]=%s\n",*ntok,vi,tok[vi]);
	while (*cp!=0) {
//	printf("lin =|%s| cp=%s ntok=%d ,i=%d ,jj=%d\n",lin,cp,*ntok,i,jj);
		if (*cp==' ' || *cp=='	'){
			*cp = ' ';
			cp = find_non_space(cp);
		}
		if (*cp == '!') goto endofline;
		p2 = find_term(cp);
		jj = p2-cp+1;
		if (jj==0) goto endofline;
//		for(vi=1;vi<=*ntok;vi++) printf("(token)=%d tok[%d]=%s\n",*ntok,vi,tok[vi]);
//		printf("tok[1]=%s\n",tok[1]);
		add_tokf(cp,jj,tok,ntok,outbuff,tj,in_quote);
		//add_tok(cp,jj);
//		for(i=1;vi<=*ntok;vi++) printf("(token)=%d tok[%d]=%s\n",*ntok,vi,tok[vi]);
		cp = p2 + 1 ;
		if (*ntok>280) subscript();
	}
endofline:;
	if (*ntok>0)
	{
//		if ( (*(*tok)[*ntok])=='\n' ) (*ntok)--;
		if (str_i_equals(tok[*ntok],"\n")) (*ntok)--;
//		if (strcmp((*tok)[*ntok]," ")==0) (*ntok)--;
		if (str_i_equals(tok[*ntok]," ")) (*ntok)--;
//		if (*ntok>0) p2 =  (*tok)[*ntok] + strlen((*tok)[*ntok])  - 1;
		if (*ntok > 0) p2 =  tok[*ntok] + strlen(tok[*ntok])  - 1;
		if (*p2 == 10) *p2 = 0;
	}
//	for(vi=1;vi<=*ntok;vi++) printf("(token) tok[%d]=%s\n",vi,tok[vi]);
}

/*--------------------------------------------------------------------------*/
char *find_non_space(char *cp)
{
	for ( ; *cp!=0 ; cp++ )  {
		if (*cp!=' ' && *cp!='	') break;
	}
	return cp;
}
/*--------------------------------------------------------------------------*/
char *find_term(char *cp)
{
char *start;
	start = cp;
/*	if (*cp=='"') {
		cp++;
		for (; *cp!=0 ; cp++ )  {
			if (*cp=='"') break;
		}
		return cp;
	} */
	for (; *cp!=0 ; cp++ )  {
		if (*cp == '"') {
			cp++;
			for (; *cp!=0 ; cp++ )  {
				if (*cp=='"') if (*(cp-1)!='\\') break;
			}
		}
		if (term_table[(int)*cp]==static_cast<char>(true)) break;
	}
	if (cp>start) return cp-1;
	if (*cp==' ' || *cp=='	') return cp-1;
	if (*cp==0) return cp-1;
	return cp;
}
/*--------------------------------------------------------------------------*/
void subscript(void)
{
//	gprint("Subscript out of range in tokenizer (use shorter lines)\n");
	printf("Subscript out of range in tokenizer (use shorter lines)\n");
}

void token_data(char *lin,TOKENS tk,int *ntok,char *outbuff)
{
	char *s;

	s = strtok(lin," ,=\t\n\x0a\x0c\x0d");
	*ntok = 0;

	for (;s!=NULL;) {
		if (*s == '"' || *s == '!' || *s == ';') goto endofline;
		*ntok += 1;

		strcpy(outbuff,s);
		strcpy(tk[*ntok],outbuff);
		outbuff = outbuff + strlen(outbuff) + 1;
		s = strtok(NULL," ,=\t\n\x0a\x0c\x0d");

	}
/*	printf("%d",*ntok);*/
endofline:;

}

