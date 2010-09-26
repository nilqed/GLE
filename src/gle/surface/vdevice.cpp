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

#include "../all.h"
#include "../tokens/Tokenizer.h"
#include "../gprint.h"
#include "../core.h"
#include "vdevice.h"

#define SOLID_LINE 1
#define DASHED_LINE 2

int pass_justify(const char *s);

int ingraphmode;
/*---------------------------------------------------------------------------*/
extern int gle_speed;
extern int nobigfile;
float v_scale, v_xscale, v_yscale;
#define xsizecm 21.0
#define ysizecm 18.0
#define sx(v) ( (int) ((v) * v_xscale)+1)
#define sy(v) ( v_maxy - ((int) ((v) * v_yscale)))
#define rx(v) ( (int) ((v) * v_xscale)+1)
#define ry(v) ( v_maxy - ((int) ((v) * v_yscale)))
int v_graphmode;
int v_fillstyle=1,v_fillcolor;
int vv_lstyle,v_lwidth;
int v_maxy;
FILE *gf;
extern char input_file[];
char *bgidir();

/*---------------------------------------------------------------------------*/

void v_open(float width, float height) {
}

/*---------------------------------------------------------------------------*/

void v_close() {
}

/*---------------------------------------------------------------------------*/

void v_lstyle(const char *s) {
//      if (*s == 0) s = "1";
//      vv_lstyle = DASHED_LINE;
//      if (strcmp(s,"")==0) vv_lstyle = SOLID_LINE;
//      if (strcmp(s,"1")==0) vv_lstyle = SOLID_LINE;
//      setlinestyle(vv_lstyle,0,v_lwidth);
//      if (!nobigfile) fprintf(gf,"set lstyle %s\n",s);
//      gerr();
	g_set_line_style(s);
}
/*---------------------------------------------------------------------------*/

void v_line(float zx,float zy) {
//      lineto(sx(zx),sy(zy));
//      if (!nobigfile) fprintf(gf,"aline %g %g \n",zx,zy);
	g_line(zx, zy);
}

void v_move(float zx,float zy) {
//      moveto(sx(zx),sy(zy));
//      if (!nobigfile) fprintf(gf,"amove %g %g \n",zx,zy);
	g_move(zx, zy);
}
/*---------------------------------------------------------------------------*/

void v_color(const char *s) {
/*
        int i;
        i = 0;
        if (*s == 0) s = "BLACK";
        if (strcmp(s,"RED")==0) i = 12;
        if (strcmp(s,"BLUE")==0) i = 9;
        if (strcmp(s,"GREEN")==0) i = 10;
        if (strcmp(s,"YELLOW")==0) i = 14;
        if (strcmp(s,"MAGENTA")==0) i = 5;
        if (strcmp(s,"BLACK")==0) i = 0;
        if (strcmp(s,"WHITE")==0) i = getmaxcolor();
        if (i>=getmaxcolor()) i = 0;
        setcolor(i);
        if (!nobigfile) fprintf(gf,"set color %s\n",s);
*/
	if (s != NULL && s[0] != 0) {
		int color = pass_color_var(s);
		g_set_color(color);
	}
}

void v_gsave() {
//      if (!nobigfile) fprintf(gf,"gsave \n");
	g_gsave();
}

void v_grestore() {
//      if (!nobigfile) fprintf(gf,"grestore \n");
	g_grestore();
}

void v_text(char *s) {
//      if (!nobigfile) fprintf(gf,"text %s\n",s);
//      outtext(s);
	g_text(s);
}

void v_set_hei(float g) {
//      if (!nobigfile) fprintf(gf,"set hei %g\n",g);
	g_set_hei(g);
}

void v_set_just(const char *s) {
/*        if (strcmp(s,"RC")==0) settextjustify(2,0);
        if (strcmp(s,"TC")==0) settextjustify(1,2);
        if (strcmp(s,"BC")==0) settextjustify(1,0);
        if (strcmp(s,"CC")==0) settextjustify(1,1);
        if (strcmp(s,"LEFT")==0) settextjustify(0,0);
        if (!nobigfile) fprintf(gf,"set just %s\n",s);
*/
	int just = pass_justify(s);
	g_set_just(just);
}

void v_rotate(float g) {
//      if (!nobigfile) fprintf(gf,"rotate %g \n",g);
	g_gsave();
	g_rotate(g);
}

void v_marker(char *m) {
//      settextjustify(1,1);
//      outtext("o");
//      if (!nobigfile) fprintf(gf,"marker %s\n",m);
//	g_marker((int) both.l,y);
}
