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

/*--------------------------------------------------------------*/
/*	SVG Driver for GLE  			*/
/*--------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

#include "all.h"
#include "core.h"
#include "gprint.h"
#include "d_interface.h"
#include "cutils.h"

char *font_getname(int i);

extern int control_d;
extern struct gmodel g;

/*---------------------------------------------------------------------------*/

#define false 0
#define true (!false)
extern bool BLACKANDWHITE;
#define dbg if ((gle_debug & 64)>0)
extern int gle_debug;

/*-----------------------------------------------*/
/* The global variables that CORE keeps track of */
/*-----------------------------------------------*/

string SVGGLEDevice::GetColor()
{
	stringstream s;

	s << "rgb(" << static_cast<int>(g_cur_color.b[B_R]) << "," << static_cast<int>(g_cur_color.b[B_G]) << "," << static_cast<int>(g_cur_color.b[B_B]) << ")";

	return s.str();
}
/*---------------------------------------------------------------------------*/
SVGGLEDevice::SVGGLEDevice() : GLEDevice() {
}
/*---------------------------------------------------------------------------*/
SVGGLEDevice::~SVGGLEDevice() {
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::dfont(char *c)
{
	/* only used for the DFONT driver which builds fonts */
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::message(char *s)
{
#ifdef HAVE_CURSES
	printw("%s\n",s);
#else
	printf("%s\n",s);
#endif
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::devcmd(const char *s)
{
	fprintf(SVGFile,"%s",s);
}
void SVGGLEDevice::source(const char *s)
{
	dbg fprintf(SVGFile,"%% SOURCE, %s",s);
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::get_type(char *t)
{
	strcpy(t,"HARDCOPY, PS, FILLPATH");
	// if (dev_eps) strcat(t,", EPS,");
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::set_path(int onoff)
{
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::newpath()
{
	fprintf(SVGFile," newpath ");
	ps_nvec = 0;
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::pscomment(char* ss)
{
	comments.push_back(ss);
}
/*---------------------------------------------------------------------------*/
FILE* SVGGLEDevice::get_file_pointer(void)
{
	return SVGFile;
}

void SVGGLEDevice::opendev(double width, double height, GLEFileLocation* outputfile, const string& inputfile) throw(ParserError)
{
	Height = height;
	Width  = width;
	m_OutputName.copy(outputfile);
	m_OutputName.addExtension("svg");

	SVGFile = fopen(m_OutputName.getFullPath().c_str(),"w");
	if (SVGFile == NULL) { perror("SVG open file GLE_OUTPUT: ") ; exit(1); }
	printf("[%s", m_OutputName.getName().c_str());

	fprintf(SVGFile,"<?xml version=\"1.0\" standalone=\"yes\"?>\n");
	//fprintf(SVGFile,"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\n");
  	//fprintf(SVGFile,"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n");
	fprintf(SVGFile,"<svg width=\"%gcm\" height=\"%gcm\"\n",width,height);
    fprintf(SVGFile,"xmlns=\"http://www.w3.org/2000/svg\">\n");
}

void SVGGLEDevice::closedev()
{
	g_flush();

	fprintf(SVGFile,"\n</svg>\n");
	fclose(SVGFile);

	printf("]\n");
}


void SVGGLEDevice::move(double zx,double zy)
{
	if ( g.inpath ){
		fprintf(SVGFile," M %g %g",zx,zy);
	}
}

void SVGGLEDevice::line(double zx,double zy)
{
	if ( g.inpath ){
		fprintf(SVGFile," L %g %g",zx,zy);
	}else{
		fprintf(SVGFile,
		"<line x1=\"%gcm\" y1=\"%gcm\" x2=\"%gcm\" y2=\"%gcm\" stroke=\"%s\" stroke-width=\"%gcm\" %s %s %s/>\n",
		g.curx,AY(g.cury),zx,AY(zy),GetColor().c_str(),linewidth,linejoin.c_str(),linecap.c_str(),miterlimit.c_str());
	}
}

void SVGGLEDevice::set_line_cap(int i)
{
	/*  lcap, 0= butt (default), 1=round, 2=projecting square */
	if( i == 0 ){
		linecap = "";
	}else if( i == 1){
		linecap = "stroke-linecap=\"round\"";
	}else if( i == 2){
		linecap = "stroke-linejoin=\"square\"";
	}
	//if (!g.inpath) g_flush();
	//fprintf(SVGFile,"%d setlinecap \n",i);
}

void SVGGLEDevice::set_line_join(int i)
{
	/*  ljoin, 0= miter(default), 1=round, 2=bevel */
	if( i == 0 ){
		linejoin = "";
	}else if( i == 1){
		linejoin = "stroke-linejoin=\"round\"";
	}else if( i == 2){
		linejoin = "stroke-linejoin=\"bevel\"";
	}
//	if (!g.inpath) g_flush();
//	fprintf(SVGFile,"%d setlinejoin \n",i);
}

void SVGGLEDevice::set_line_miterlimit(double d)
{
	stringstream s;
	if( d >= 1.0){
		s << "stroke-miterlimit=\""<<d<<"\"";
	}
	miterlimit = s.str();
	//if (!g.inpath) g_flush();
	//fprintf(SVGFile,"%g setmiterlimit \n",d);
}

void SVGGLEDevice::set_line_width(double w)
{
	if (w==0) w = 0.02;
	if (w<.0002) w = 0;
	linewidth = w;
	//if (!g.inpath) g_flush();
	//fprintf(SVGFile,"%g setlinewidth \n",w);
}

void SVGGLEDevice::set_line_styled(double dd)
{}
void SVGGLEDevice::set_line_style(const char *s)
{
	/* should deal with [] for solid lines */
	static const char *defline[] = {"","","12","41","14","92","1282","9229","4114","54","73","7337","6261","2514"};
	static char ob[200];
	int l;

	if (!g.inpath) g_flush();
	strcpy(ob,"[");
	if (strlen(s)==1) s = defline[*s-'0'];
	l = strlen(s);
	for (i=0;i<l;i++)
		sprintf(ob+strlen(ob),"%g ",(*(s+i)-'0')*g.lstyled);
	strcat(ob,"]");
	//fprintf(SVGFile,"%s 0 setdash \n",ob);
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::fill()
{
	fprintf(SVGFile,"gsave \n");
	ddfill();
	fprintf(SVGFile,"grestore \n");
}
void SVGGLEDevice::ddfill(void)
{
	if (g_cur_fill.b[B_F] == 255) return; /* clear fill, do nothing */
	if (g_cur_fill.b[B_F] == 2) {shade(); return;}
	set_fill();			/*because color and fill are the same*/
	fprintf(SVGFile,"fill \n");
	set_color();
}
void SVGGLEDevice::shade(void)
{
	double step1,step2;
	fprintf(SVGFile,"gsave \n");
	fprintf(SVGFile,"clip \n");
	fprintf(SVGFile,"newpath  \n");
	fprintf(SVGFile,"0 setgray \n");
	step1 = g_cur_fill.b[B_B]/160.0;
	step2 = g_cur_fill.b[B_G]/160.0;

	fprintf(SVGFile,"%g setlinewidth\n",(double) g_cur_fill.b[B_R]/160.0);
	if (step1>0) {
	  fprintf(SVGFile,"%g %g %g { /x exch def \n",-40.0,step1,40.0);
	  fprintf(SVGFile,"x 0 moveto 40 x add 40 lineto stroke\n");
	  fprintf(SVGFile,"} for \n");
	}
	if (step2>0) {
	 fprintf(SVGFile,"%g %g %g { /x exch def \n",0.0,step2,80.0);
	 fprintf(SVGFile,"x 0 moveto -40 x add 40 lineto stroke\n");
	 fprintf(SVGFile,"} for \n");
	}
	fprintf(SVGFile,"grestore \n");
/*	set_line_width(g.lwidth); */
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::fill_ary(int nwk,double *wkx,double *wky)
{
	int i;
	fprintf(SVGFile,"gsave \n");
	fprintf(SVGFile,"newpath \n");
	fprintf(SVGFile,"%g %g moveto \n",wkx[0],wky[0]);
	for (i=1;i<nwk;i++)
		fprintf(SVGFile,"%g %g l \n",wkx[i],wky[i]);
	set_fill();
	fprintf(SVGFile,"fill \n");
	set_color();
	fprintf(SVGFile,"grestore \n");
}
void SVGGLEDevice::line_ary(int nwk,double *wkx,double *wky)
{
	int i;
	fprintf(SVGFile,"gsave \n");
	fprintf(SVGFile,"newpath \n");
	fprintf(SVGFile,"%g %g moveto \n",wkx[0],wky[0]);
	for (i=1;i<nwk;i++)
		fprintf(SVGFile,"%g %g l \n",wkx[i],wky[i]);
	fprintf(SVGFile,"stroke \n");
	fprintf(SVGFile,"grestore \n");
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::stroke()
{
	fprintf(SVGFile,"gsave \n");
	fprintf(SVGFile,"stroke \n");
	fprintf(SVGFile,"grestore \n");
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::clip()
{
	fprintf(SVGFile,"clip \n");
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::set_matrix(double newmat[3][3])
{
//	fprintf(SVGFile,"<g matrix(%g %g %g %g %g %g)>\n ",
//		newmat[0][0],newmat[1][0],newmat[0][1],
//		newmat[1][1],newmat[0][2],newmat[1][2]);
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::reverse() 	/* reverse the order of stuff in the current path */
{
	fprintf(SVGFile,"reversepath \n");
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::closepath()
{
	fprintf(SVGFile,"closepath \n");
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::clear()
{
	g_scale(72.0,72.0);
	g_scale(.393701,.393701);
	/*
	if (!dev_eps) g_translate(1.5,1.01);
	if ((!dev_eps) && (g.userwidth>g.userheight)) {
		fprintf(SVGFile,"%% Flipping coord system \n");
		g_move(0.0,0.0);
		g_rotate(90.0);
		g_translate(0.0,-g.userheight);
		g_move(0.0,0.0);
	}*/
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::flush()
{
	if (g.inpath) return;
	if (g.xinline) {
		//fprintf(SVGFile,"stroke \n");
		ps_nvec = 0;
	}
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::arcto(dbl x1,dbl y1,dbl x2,dbl y2,dbl rrr)
{
	if (g.xinline==false)  move(g.curx,g.cury);
	fprintf(SVGFile,"%g %g %g %g %g arcto clear %g %g l \n",x1,y1,x2,y2,rrr,x2,y2);
	g.xinline = true;
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::arc(dbl r,dbl t1,dbl t2,dbl cx,dbl cy)
{
	double dx,dy;
	double x,y;
	g_get_xy(&x,&y);
	polar_xy(r,t1,&dx,&dy);
	if (!g.inpath) g_move(cx+dx,cy+dy);
	fprintf(SVGFile,"%g %g %g %g %g arc \n",cx,cy,r,t1,t2);
	g.xinline = true;
	if (!g.inpath) g_move(x,y);
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::narc(dbl r,dbl t1,dbl t2,dbl cx,dbl cy)
{
	double dx,dy;
	double x,y;
	g_get_xy(&x,&y);
	polar_xy(r,t1,&dx,&dy);
	if (!g.inpath) g_move(cx+dx,cy+dy);
	fprintf(SVGFile,"%g %g %g %g %g arcn \n",cx,cy,r,t1,t2);
	g.xinline = true;
	if (!g.inpath) g_move(x,y);
}
void SVGGLEDevice::elliptical_arc(dbl rx,dbl ry,dbl t1,dbl t2,dbl cx,dbl cy)
{
	double dx,dy;
	double x,y;

	g_get_xy(&x,&y);
	polar_xy(rx,ry,t1,&dx,&dy);
	if (!g.inpath) g_move(cx+dx,cy+dy);
	fprintf(SVGFile,"%g %g %g %g %g %g ellipse \n",cx,cy,rx,ry,t1,t2);
	g.xinline = true;
	if (!g.inpath) g_move(x,y);
}
void SVGGLEDevice::elliptical_narc(dbl rx,dbl ry,dbl t1,dbl t2,dbl cx,dbl cy)
{
	double dx,dy;
	double x,y;

	g_get_xy(&x,&y);
	polar_xy(rx,ry,t1,&dx,&dy);
	if (!g.inpath) g_move(cx+dx,cy+dy);
	fprintf(SVGFile,"%g %g %g %g %g %g ellipsen\n",cx,cy,rx,ry,t1,t2);
	g.xinline = true;
	if (!g.inpath) g_move(x,y);
}

/*---------------------------------------------------------------------------*/
void SVGGLEDevice::box_fill(dbl x1, dbl y1, dbl x2, dbl y2)
{
	if (g.inpath) xdbox(x1,y1,x2,y2);
	else {
		g_flush();
		fprintf(SVGFile," newpath ");
		xdbox(x1,y1,x2,y2);
		ddfill();
		fprintf(SVGFile,"newpath \n");
/*		set_fill();
		fprintf(SVGFile,"fill \n");
		set_color();
*/
	}
}
void SVGGLEDevice::box_stroke(dbl x1, dbl y1, dbl x2, dbl y2, bool reverse)
{
	if (g.inpath) xdbox(x1,y1,x2,y2);
	else {
		g_flush();
		fprintf(SVGFile," newpath ");
		xdbox(x1,y1,x2,y2);
		fprintf(SVGFile,"stroke \n");
		ps_nvec = 0;
	}
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::xdbox(double x1, double y1, double x2, double y2)
{
	fprintf(SVGFile," %g %g moveto %g %g l %g %g l %g %g l closepath \n"
			,x1,y1,x2,y1,x2,y2,x1,y2);
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::circle_stroke(double zr)
{
	double x,y;
	g_get_xy(&x,&y);
	if (g.inpath)
		fprintf(SVGFile," %g %g %g 0 360 arc \n",x,y,zr);
	else {
		g_flush();
		fprintf(SVGFile," newpath ");
		fprintf(SVGFile," %g %g %g 0 360 arc \n",x,y,zr);
		fprintf(SVGFile,"stroke \n");
	}
}
void SVGGLEDevice::circle_fill(double zr)
{
	double x=g.curx,y=g.cury;
	if (g.inpath)
		fprintf(SVGFile," %g %g %g 0 360 arc \n",x,y,zr);
	else {
		g_flush();
		fprintf(SVGFile,"newpath ");
		fprintf(SVGFile,"%g %g %g 0 360 arc \n",x,y,zr);
		ddfill();
		fprintf(SVGFile,"newpath \n");
/*

		set_fill();
		fprintf(SVGFile,"fill \n");
		set_color();
*/
	}
}

void SVGGLEDevice::ellipse_stroke(double rx, double ry)
{
	double x,y;

	g_get_xy(&x,&y);
	if (g.inpath)
		fprintf(SVGFile," %g %g %g %g 0 360 ellipse \n",x,y,rx,ry);
	else {
		g_flush();
		fprintf(SVGFile," newpath ");
		fprintf(SVGFile," %g %g %g %g 0 360 ellipse \n",x,y,rx,ry);
		fprintf(SVGFile,"stroke \n");
	}
}
void SVGGLEDevice::ellipse_fill(double rx, double ry)
{
	double x=g.curx,y=g.cury;
	if (g.inpath)
		fprintf(SVGFile," %g %g %g %g 0 360 ellipse \n",x,y,rx,ry);
	else {
		g_flush();
		fprintf(SVGFile,"newpath ");
		fprintf(SVGFile," %g %g %g %g 0 360 ellipse \n",x,y,rx,ry);
		ddfill();
		fprintf(SVGFile,"newpath \n");
	}
}

/*---------------------------------------------------------------------------*/
void SVGGLEDevice::bezier(dbl x1,dbl y1,dbl x2,dbl y2,dbl x3,dbl y3)
{
	double x=g.curx,y=g.cury;
	if (g.inpath) {
		if (g.xinline==false)  move(g.curx,g.cury);
		fprintf(SVGFile,"%g %g %g %g %g %g curveto \n"
			,x1,y1,x2,y2,x3,y3);
	} else {
		g_flush();
		if (!g.xinline) fprintf(SVGFile,"%g %g moveto ",x,y);
		fprintf(SVGFile,"%g %g %g %g %g %g curveto \n"
			,x1,y1,x2,y2,x3,y3);
	}
	g.xinline = true;
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::test_SVGFile()
{
	if (SVGFile==NULL) return;
	set_color();
}
void SVGGLEDevice::set_color()
{
	if (BLACKANDWHITE) {
	 	fprintf(SVGFile,"%g setgray \n",((g_cur_color.b[B_R]*3.0/255.0
		+g_cur_color.b[B_G]*2.0/255.0+g_cur_color.b[B_B]/255.0) / 6));
	}// else
	 //	fprintf(SVGFile,"%g %g %g setrgbcolor \n",g_cur_color.b[B_R]/255.0
	//	,g_cur_color.b[B_G]/255.0,g_cur_color.b[B_B]/255.0);
}
void SVGGLEDevice::set_fill()
{
	if (BLACKANDWHITE) {
	 	fprintf(SVGFile,"%g setgray \n",((g_cur_fill.b[B_R]*3.0/255.0
		+g_cur_fill.b[B_G]*2.0/255.0+g_cur_fill.b[B_B]/255.0) / 6));
	} //else
	 //	fprintf(SVGFile,"%g %g %g setrgbcolor \n",g_cur_fill.b[B_R]/255.0
		//,g_cur_fill.b[B_G]/255.0,g_cur_fill.b[B_B]/255.0);
}
/*---------------------------------------------------------------------------*/
void SVGGLEDevice::set_color(int f)
{
	g_flush();
	g_cur_color.l = f;
	set_color();
}
void SVGGLEDevice::set_fill(int f)
{
	g_cur_fill.l = f;
}

void SVGGLEDevice::set_pattern_color(int c) {
}

/*---------------------------------------------------------------------------*/
void SVGGLEDevice::beginclip()
{
	fprintf(SVGFile,"gsave \n");
}
void SVGGLEDevice::endclip()
{
	g_flush();
	fprintf(SVGFile,"grestore \n");
	gmodel* state = (gmodel*) myallocz(SIZEOFSTATE);
	g_get_state(state);
	g_set_state(state);
	myfree(state);
}
/*---------------------------------------------------------------------------*/

struct psfont_struct {char *sname; char *lname;} ;
extern struct psfont_struct psf[];

/*---------------------------------------------------------------------------*/
void SVGGLEDevice::dochar(int font, int cc)
{
	char *s;
	static int this_font;
	static double this_size;


	reapsfont();
	if (font_get_encoding(font)>2) {
		my_char(font,cc);
		return;
	}
	if (this_font!=font || this_size!=g.fontsz) {
		if (g.fontsz<0.00001) {
			gprint("Font size is zero, error ********* \n");
			return;
		}
		s = font_getname(font);
		for (i=0;;i++) {
			if (psf[i].sname==NULL) break;
			dbg printf("font match  {%s} {%s} \n",s,psf[i].sname);
			if (str_i_equals(psf[i].sname,s)) break;
		}
		if (psf[i].sname==NULL) {
			my_char(font,cc);
			return;
		}
		this_font = font;
		this_size = g.fontsz;
		fprintf(SVGFile," %f /%s f ",g.fontsz,psf[i].lname);
	}
	if (g.inpath) {
		if (isalnum(cc) && cc<127) fprintf(SVGFile,"(%c) ps ",cc);
		else  fprintf(SVGFile,"(\\%o) ps ",cc);
	} else {
		if (isalnum(cc) && cc<127) fprintf(SVGFile,"(%c) s ",cc);
		else  fprintf(SVGFile,"(\\%o) s ",cc);
	}
}

void SVGGLEDevice::resetfont() {
}

void SVGGLEDevice::reapsfont(void)  /* add aditional ps fonts,  e.g.  pstr = TimesRoman */
{
	static int init_done;
	FILE *fptr;
	char *s;
	char inbuff[200];
	if (init_done) return;
	init_done = true;

	/* Find last used psf */
	for (i=0;;i++) if (psf[i].sname==NULL) break;

	string fname = fontdir("psfont.dat");
	fptr = fopen(fname.c_str(), "r");
	if (fptr==0) return; /* if not exists then don't bother */

	for (fgets(inbuff,200,fptr);!feof(fptr);fgets(inbuff,200,fptr)) {
		s = strchr(inbuff,'!');
		if (s!=NULL) *s=0;
		s = strtok(inbuff," \t,\n");
		if (s!=NULL) if (*s!='\n') {
			psf[i].sname = sdup(s);
			s = strtok(0," \t,\n");
			psf[i].lname = sdup(s);
			i++;
		}
	}
	psf[i].sname = NULL;
	psf[i].lname = NULL;
}

int SVGGLEDevice::getDeviceType() {
	return GLE_DEVICE_SVG;
}

