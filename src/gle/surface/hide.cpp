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

/*
	3d Surface plotting with hidden line removal
*/

#include "../all.h"
#include "../gprint.h"
#include "../core.h"
#include "gsurface.h"
#include "vdevice.h"

#define huge
#define TESTING true

typedef int int32;

void initminmax(void);
void init_user(void);
void nice_ticks(float *dticks, float *gmin,float *gmax , float *t1,float *tn);
void grid_back(int nx, int ny, float z1, float z2);
void skirt(float huge *z,int ix1, int iy1, float minz);
void find_splits(int nx, int ny, int *splitx, int *splity);
void fxy_polar(float dx,float dy,float *radius,float *angle);
void fpolar_xy(float r, float angle, float *dx, float *dy);
void draw_zaxis(struct GLEAxis3D *ax, int nx, int ny, float minz, float maxz);
void draw_axis(struct GLEAxis3D *ax, int nx, int ny, float minz, float maxz);
void draw_maintitle();
void hide(float huge *z,int nx, int ny, float minz, float maxz, struct surface_struct *sff);
void touser3(float x, float y, float z, float *uux, float *uuy, float *uuz);
void touser(float x, float y, float z, float *uux, float *uuy);
void matmove(float i[4][4],float x, float y, float z);
void seth2(int rx1, int ry1, float rz1, int rx2, int ry2, float rz2);
void matscale(float i[4][4],float x, float y, float z);
void matun(float m[4][4]);
void matmul(float a[4][4],float b[4][4]);
void matrx(float i[4][4], float angle);
void matry(float i[4][4], float angle);
void matrz(float i[4][4], float angle);
void vector_line(int x1, float y1, int x2, float y2);
void draw_riselines(int nx, int ny,float minz, float maxz);
void move3d(float x, float y, float z);
void line3d(float x, float y, float z);
void cube(float x, float y, float z1, float z2);
void horizon(float huge *z,int ix1,int iy1,int ix2,int iy2);
void horizon2(float huge *z,int ix1,int iy1,int ix2,int iy2);
void horizonv(float huge *z,int ix1,int iy1,int ix2,int iy2);
void horizonv2(float huge *z,int ix1,int iy1,int ix2,int iy2);
void hclipvec(int x1, float y1, int x2, float y2, int sethi);
void hclipvec2(int x1, float y1, int x2, float y2, int sethi);
void clipline(float x1, float y1, float z1, float x2, float y2, float z2);
void draw_markers(int nx, int ny);
void matshow(const char *s, float m[4][4]);
void show_horizon();
float base;
extern int ngerror;
extern int batch_only;
FILE *fp;
float image[4][4];
static double min_zed;

void init_user() {
	matun(image);
}

float smin_x,smax_x,smin_y,smax_y,smin_z,smax_z;

void initminmax() {
	smin_x = 10e10; smax_x = -10e10;
	smin_y = 10e10; smax_y = -10e10;
	smin_z = 10e10; smax_z = -10e10;
}

void setaminmax(float x, float *x1, float *x2) {
	if (x < *x1) *x1 = x;
	if (x > *x2) *x2 = x;
}

void setminmax(float x,float y,float z) {
	setaminmax(x,&smin_x,&smax_x);
	setaminmax(y,&smin_y,&smax_y);
	setaminmax(z,&smin_z,&smax_z);
}

#define RERR (0.0001)         /* Rounding error */
int MAXH;
#define deg(d) ((d)*3.14159254/180)
float *h,*h2;         /* h2 is the underneath */
float eye_x,eye_y,eye_depth;
float vdist=0; /* 1=eye touching front of picture, infinity=  cube back */
float map_mul,map_sub;
float maxdepth;
float xmargin = 2;
float ymargin = 1.5;
int doy=true;
int dox=true;  /* false */
int nnx;
int vsign=1;
#define returng {v_close(); return;}
static struct surface_struct sf;

void hide(float huge *z,int nx, int ny, float minz, float maxz, struct surface_struct *sff) {
	int i,x,y;
	float r,angle;
	float width,height;
	float ux,uy,uz,scalex,scaley,ux3,uy3,ux2;
	int splitx, splity;
	int x1,x2;

	v_gsave();
	g_set_line_cap(1); /* round */
	sf = *sff; /* Make a local copy of all the paramters */
	init_user();
	eye_x = sf.eye_x;
	eye_y = sf.eye_y;
	vdist = sf.vdist;
	base = sf.screenx;
	xmargin = sf.screenx*2/10.0;
	ymargin = sf.screeny*1.5/10.0;
	width = sf.screenx-.5;
	height = sf.screeny-.5;
	if (sf.title != NULL) height = height -.7;
	dox = sf.xlines_on;
	doy = sf.ylines_on;
	MAXH = sf.maxh;

	if (MAXH==0) MAXH = 5000;
	h = (float*)malloc(MAXH*sizeof(float));
	h2 = (float*)malloc(MAXH*sizeof(float));
	if (h==NULL || h2==NULL) {
		gprint("Was not able to allocate horizon arrays %d \n",MAXH);
		return;
	}
	min_zed  = minz;
	v_open(sf.screenx,sf.screeny);
	nnx = nx;        /* save dimensions for use in subroutine */
	vsign = 1; /* doing top half */
	for (i=0; i<MAXH ;i++) h[i] = 0.0;  /* zero horizon */
	maxdepth = 0;

	/* make it a 10x10x10 cube */
	matmove(image,0.0,0.0,-minz); /* so 0,0,minz = 0,0,0 */
	if (sf.sizez==0) sf.sizez = (sf.sizex+sf.sizey)/2.0;
	matscale(image,sf.sizex/(float) nx,sf.sizey/(float) ny,sf.sizez/(maxz-minz));
	/* positive z comes towards the viewer */

	//matshow("Before rotate", image);
	//cout << "sizex = " << sf.sizex << " sizey = " << sf.sizey << endl;
	//cout << "screenx = " << sf.screenx << " screeny = " << sf.screeny << endl;

	/* 60 50 20 , 80 0 0 */
	matrx(image,deg(sf.xrotate)); /* clockwise holding right hand side */
	matry(image,deg(sf.yrotate)); /* clockwise holding top */
	matrz(image,deg(sf.zrotate)); /* clockwise holding front */

	/* now rotate cube so line from 0,0,0 --> 0,0,1 is vertical on screen */
	touser(0.0,0.0,0.0,&ux,&uy);
	touser(0.0,0.0,1.0,&ux3,&uy3);
	fxy_polar(ux3-ux,uy3-uy,&r,&angle);
	matrz(image,deg(-90.0+angle)); /* clockwise holding front */

	//matshow("After rotate", image);

	/* Normalize image, shift to touch x=0, y=0, z=0 */
	initminmax();
	for (x=0; x<nx; x+=nx-1) {
	 for (y=0; y<ny; y+=ny-1) {
		touser3(x,y,minz,&ux,&uy,&uz);
		setminmax(ux,uy,uz);
		touser3(x,y,maxz,&ux,&uy,&uz);
		setminmax(ux,uy,uz);
	 }
	}

	scalex = 1;
	scaley = 1;
	if (smax_x>width) scalex = width/(smax_x-smin_x);
	if (smax_y>height) scaley = height/(smax_y-smin_y);
	if (scalex<scaley) scaley = scalex;

	image[0][3] = image[0][3] - smin_x + xmargin/scaley;
	image[1][3] = image[1][3] - smin_y + ymargin/scaley;
	image[2][3] = image[2][3] - smax_z;

	initminmax();
	for (x=0; x<nx; x+=nx-1) {
	 for (y=0; y<ny; y+=ny-1) {
		touser3(x,y,minz,&ux,&uy,&uz);
		setminmax(ux,uy,uz);
		touser3(x,y,maxz,&ux,&uy,&uz);
		setminmax(ux,uy,uz);
	 }
	}

	scalex = 1;
	scaley = 1;
	if (smax_x>width) scalex = width/smax_x;
	if (smax_y>height) scaley = height/smax_y;
	if (scalex<scaley) scaley = scalex;
	matscale(image,scaley,scaley,scaley);

	initminmax();
	for (x=0; x<nx; x+=nx-1) {
	 for (y=0; y<ny; y+=ny-1) {
		touser3(x,y,minz,&ux,&uy,&uz);
		setminmax(ux,uy,uz);
		touser3(x,y,maxz,&ux,&uy,&uz);
		setminmax(ux,uy,uz);
	 }
	}

	//matshow("After rescale", image);

	/* Deepest point will be at   z==-(smax_z-smin_z) */
	maxdepth = smin_z;

	v_move(eye_x,eye_y);
	v_set_hei(.3);

	/* decide on mapping of ux onto h[] -------------------------- */
#define maph(ux) (((ux)-map_sub)*map_mul)
#define unmaph(ux) (((ux)/map_mul)+map_sub)

	x1 = 10000;
	x2 = -10000;
	for (x=0; x<nx; x+=nx-1) {
	 for (y=0; y<ny; y+=ny-1) {
		 touser(x,y,minz,&ux,&uy);
		if (ux<x1) x1 = (int)ux;
		if (ux>x2) x2 = (int)ux;
		 touser(x,y,maxz,&ux,&uy);
		if (ux<x1) x1 = (int)ux;
		if (ux>x2) x2 = (int)ux;
	 }
	}
	/* map x1 -- x2  onto 2 -- 998 */
	x1--; x2++;
	map_sub = x1;
	map_mul = (MAXH-100)/((double) x2-x1);
	/* ----------------------------------------------------  */

	/* if the closest line of constant X is not one of the edges
	   then split object at EYE(X)
	*/

	find_splits(nx,ny,&splitx,&splity);

	/* if ux of nx,0,0 is less than ux of 0,ny,0 then */
	/* eye is closer to XAXIS then*/
	/*   (lines of constant x will be horizontal */
	/*   process lines of constant X in usual order, front to back*/
	/*   After each x line, draw lines of constant y between lastx and nextx */
	/* else, do y first and x betweens.*/

	touser(nx,0,0,&ux,&uy);
	touser(0,ny,0,&ux2,&uy);


	/* Now draw bottom half */
	v_color(sf.bot_color);
	v_lstyle(sf.bot_lstyle);
	vsign = -1;        /* reverse tests for bottom half */
	for (i=0; i<MAXH ;i++) h2[i] = 9999.0;  /* zero horizon */

	if (sf.bot_on && !sf.skirt_on) {
	touser(nx,0,0,&ux,&uy);
	touser(0,ny,0,&ux2,&uy);
	if (ux < ux2) {
	 for (x=nx-1; x>=0; x--) {
	  if (dox) for (y=splity; y>0; y--) horizonv2(z,x,y,x,y-1); /* x */
	  if (dox) for (y=splity+1; y<ny; y++) if (y>0) horizonv2(z,x,y-1,x,y);
	  for (y=splity; y>=0; y--) if (x>0) if (doy) horizonv2(z,x,y,x-1,y);
	  for (y=splity+1; y<ny; y++) if (x>0) if (doy) horizonv2(z,x,y,x-1,y);
	 }
	} else {
	 for (y=0; y<ny; y++) {
	  if (doy) for (x=splitx; x>0; x--) horizonv2(z,x,y,x-1,y); /* y */
	  if (doy) for (x=splitx+1; x<nx; x++) if (x>0) horizonv2(z,x-1,y,x,y);
	  for (x=splitx; x>=0; x--) if (y<ny-1) if (dox) horizonv2(z,x,y,x,y+1);
	  for (x=splitx+1; x<nx; x++) if (y<ny-1) if (dox) horizonv2(z,x,y,x,y+1);
	 }
	}
	} /* if bot_on a&& !skirt_on */


/* should clip object nicely at zmin,zmax,  do this inside vector_line */
	vsign = 1;        /* normal tests for top half */
	v_color(sf.top_color);
	v_lstyle(sf.top_lstyle);
	if (sf.top_on) {
	if (ux < ux2) {
	 /* Go from x = splitx to zero, then splitx+1 to nx-1 */
	 for (x=nx-1; x>=0; x--) {
	  if (dox) for (y=splity; y>0; y--) horizonv(z,x,y,x,y-1); /* X */
	  if (dox) for (y=splity+1; y<ny; y++) if (y>0) horizonv(z,x,y-1,x,y); /* Constant X  (this does join, should be earlier )(*/
	  for (y=splity; y>=0; y--) if (x>0) if (doy) horizonv(z,x,y,x-1,y); /* Constant Y */
	  for (y=splity+1; y<ny; y++) if (x>0) if (doy) horizonv(z,x,y,x-1,y); /* Constant Y */
	 }
	} else {
	 /* Go from y = splity to zero, then splity+1 to ny-1 */
	 for (y=0; y<ny; y++) {
	  if (doy) for (x=splitx; x>0; x--) horizonv(z,x,y,x-1,y); /* Constant y */
	  if (doy) for (x=splitx+1; x<nx; x++) if (x>0) horizonv(z,x-1,y,x,y); /* Constant y  (this does join, should be first )(*/
	  for (x=splitx; x>=0; x--) if (y<ny-1) if (dox) horizonv(z,x,y,x,y+1); /* Constant x */
	  for (x=splitx+1; x<nx; x++) if (y<ny-1) if (dox) horizonv(z,x,y,x,y+1); /* Constant x */
	 }
	}
	} /* sf.top_on */


	v_color(sf.top_color);
	v_lstyle(sf.top_lstyle);
	if (sf.skirt_on) { /* set h2 to bottom of top surface */
		for (x=nx-1; x>0; x--) seth2(x,0,z[x],x-1,0,z[x-1]);
		for (y=0; y<ny-1; y++) seth2(nx-1,y,z[nx-1+y* (int32) nx],nx-1,y+1,z[nx-1+(y+1)* (int32) nx]);
	}
/*        show_horizon(); */
	if (sf.skirt_on) {
	  for (y=splity; y>=0; y--) skirt(z,nx-1,y,minz);
	  for (y=splity+1; y<ny; y++) skirt(z,nx-1,y,minz);
	  for (x=splitx; x>=0; x--) skirt(z,x,0,minz);
	  for (x=splitx+1; x<nx; x++) skirt(z,x,0,minz);
	}
	if (sf.skirt_on) { /* set h2 to bottom edge of box */
		for (x=nx-1; x>=0; x--) seth2(x,0,minz,x-1,0,minz);
		for (y= -1; y<ny; y++) seth2(nx-1,y,minz,nx-1,y+1,minz);
	}
	/* now define h2[] if bot wasn't drawn */
	if (!sf.bot_on && !sf.skirt_on) {
		for (x=nx-1; x>0; x--) seth2(x,0,z[x],x-1,0,z[x-1]);
		for (y=0; y<ny-1; y++) seth2(nx-1,y,z[nx-1+y* (int32) nx],nx-1,y+1,z[nx-1+(y+1)* (int32) nx]);
	}

	/* Set back original color */
	v_grestore();

	/* Draw the unit cube for reference   -------------------------- */
	if (sf.cube_on) {
		cube(nx-1,ny-1,minz,maxz);
	}

	draw_maintitle();

/* now lets try and draw the axes */
	draw_axis(&sf.xaxis,nx,ny,minz,maxz);
	draw_axis(&sf.yaxis,nx,ny,minz,maxz);
	draw_zaxis(&sf.zaxis,nx,ny,minz,maxz);

/* The back, right and bottom grid lines */
	grid_back(nx,ny,minz,maxz);

	draw_markers(nx,ny);
	draw_riselines(nx,ny,minz,maxz);

	free(h); free(h2);
	v_close();
}
#define DVAL(a,b) if ((a)==0) a = (b)

void draw_maintitle() {
	v_set_just("BC");
	if (sf.title == NULL) return;
	v_color(sf.title_color);
	DVAL(sf.title_hei,base/30.0);
	v_set_hei(sf.title_hei);
	v_move(sf.screenx/2.0,sf.screeny-sf.title_hei+sf.title_dist);
	v_text((char*)sf.title);
}

void draw_axis(struct GLEAxis3D *ax, int nx, int ny, float minz, float maxz) {
	float ux,uy,ux2,uy2,ux3,uy3,r,a,ta,r2,x,xx,t1,tn;
	char buff[80];
	if (ax->type>1) return;
	if (!ax->on) return;
	if (ax->type==0) {
		touser(0,0,minz,&ux,&uy);
		touser(nx-1,0,minz,&ux2,&uy2);
	} else {
		touser(nx-1,0,minz,&ux,&uy);
		touser(nx-1,ny-1,minz,&ux2,&uy2);
	}
	v_color(ax->color);
	if (!sf.cube_on) {v_move(ux,uy); v_line(ux2,uy2);}
	fxy_polar(ux2-ux,uy2-uy,&r,&a);
	ta = a ;
	a = a - 90;
	if (ax->ticklen == 0) ax->ticklen = base*.001;
	r = ax->ticklen;
	r2 = ax->ticklen+base*.02+ax->dist;
	fpolar_xy(r,a,&ux2,&uy2);
	fpolar_xy(r2,a,&ux3,&uy3);

	DVAL(ax->hei,base/60.0);
	v_set_hei(ax->hei);
	v_set_just("TC");

	nice_ticks(&ax->step, &ax->min, &ax->max, &t1, &tn);

	for (x=t1; x<=.00001+ax->max; x+=ax->step) {
		if (ax->type==0) {
			xx =  (nx-1)*(x-ax->min)/(ax->max - ax->min);
			touser(xx,0,minz,&ux,&uy);
		} else {
			xx =  (ny-1)*(x-ax->min)/(ax->max - ax->min);
			touser(nx-1,xx,minz,&ux,&uy);
		}
		v_move(ux,uy);
		v_line(ux+ux2,uy+uy2);
		v_move(ux+ux3,uy+uy3);
		if (fabs(x)<(.0001*ax->step)) x = 0;
		sprintf(buff,"%g",x);
		g_gsave();
		g_rotate(ta);
		if (ax->nolast && x> (ax->max - .5*(ax->step))) /*  */ ;
		else if (ax->nofirst && x==t1) /* do nothing */;
		else v_text(buff);
		g_grestore();
	}
	v_set_just("TC");
	/* Now draw the title for this axis */
	if (ax->title == NULL) return;
	v_color(ax->title_color);
	DVAL(ax->title_hei,base/40.0);
	v_set_hei(ax->title_hei);
	if (ax->type==0) {
		touser((nx-1)/2.0,0,minz,&ux,&uy);
	} else {
		touser(nx-1,(ny-1)/2.0,minz,&ux,&uy);
	}
	DVAL(ax->title_dist,base/17.0);
	r = ax->title_dist;
	fpolar_xy(r,a,&ux2,&uy2);
	g_gsave();
	v_move(ux+ux2,uy+uy2);
	g_rotate(ta);
	v_text((char*)ax->title);
	g_grestore();
}

void draw_zaxis(struct GLEAxis3D *ax, int nx, int ny, float minz, float maxz) {
	float ux,uy,ux2,uy2,ux3,uy3,r,a,ta,r2,x,t1,tn;
	char buff[80];

	if (!ax->on) return;
	touser(0,0,minz,&ux,&uy);
	touser(0,0,maxz,&ux2,&uy2);
	v_color(ax->color);
	if (!sf.cube_on) {v_move(ux,uy); v_line(ux2,uy2);}
	fxy_polar(ux2-ux,uy2-uy,&r,&a);
	ta = a ;
	a = a + 90;
	if (ax->ticklen == 0) ax->ticklen = base*.001;
	r = ax->ticklen;
	r2 = ax->ticklen+base*.02+ax->dist;
	fpolar_xy(r,a,&ux2,&uy2);
	fpolar_xy(r2,a,&ux3,&uy3);
	DVAL(ax->hei,base/60.0);
	v_set_hei(ax->hei);
	v_set_just("RC");
	nice_ticks(&ax->step, &ax->min, &ax->max, &t1, &tn);
	for (x=t1; x<=.0001+ax->max; x+=ax->step) {
		touser(0,0,x,&ux,&uy);
		v_move(ux,uy);
		v_line(ux+ux2,uy+uy2);
		v_move(ux+ux3,uy+uy3);
		if (fabs(x)<(.0001*ax->step)) x = 0;
		sprintf(buff,"%g",x);
		v_text(buff);
	}
	v_set_just("BC");
	/* Now draw the title for this axis */
	if (ax->title == NULL) return;
	v_color(ax->title_color);
	DVAL(ax->title_hei,base/40.0);
	v_set_hei(ax->title_hei);
	touser(0,0,(maxz-minz)/2.0+minz,&ux,&uy);
	DVAL(ax->title_dist,base/17.0);
	r = ax->title_dist;
	fpolar_xy(r,a,&ux2,&uy2);
	g_gsave();
	v_move(ux+ux2,uy+uy2);
	g_rotate(a-90);
	v_text((char*)ax->title);
	g_grestore();
}

void move3d(float x, float y, float z) {
	float ux,uy;
	touser(x,y,z,&ux,&uy);
	v_move(ux,uy);
       #if (defined DJ || defined EMXOS2)   /* a.r. for surface markers */
	d_move_really(ux,uy);
       #endif
}

void line3d(float x, float y, float z) {
	float ux,uy;
	touser(x,y,z,&ux,&uy);
	v_line(ux,uy);
}

void show_horizon() {
	int i;
	v_color("RED");
	v_move(unmaph(0),h[0]);
	for (i=0;i<900;i++) {
		v_line(unmaph(i),h[i]);
	}
	v_color("BLUE");
	v_move(unmaph(0),h2[0]);
	for (i=0;i<900;i++) {
		v_line(unmaph(i),h2[i]);
	}

}

void skirt(float huge *z,int ix1, int iy1, float minz) {
	/*
	touser(ix1,iy1,z[ix1+iy1* (int32) nnx],&ux,&y1);
	x1 = maph(ux);
	touser(ix1,iy1,minz,&ux,&y2);
	x2 = maph(ux);
	vector_line(x1,y1,x2,y2);
	*/

	clipline(ix1,iy1,z[ix1+iy1* (int32) nnx],ix1,iy1,minz);
}

void horizonv(float huge *z,int ix1,int iy1,int ix2,int iy2) {
	float ux;
	int x1,x2,putback=false;
	float y1,y2;

	if (sf.zcolour[0]!=0) {
	if (z[ix1+iy1* (int32) nnx] <= min_zed  ||
	 z[ix2+iy2* (int32) nnx] <= min_zed) {
		putback = true;
		v_color(sf.zcolour);
	}}


	touser(ix1,iy1,z[ix1+iy1* (int32) nnx],&ux,&y1);
	x1 = (int)maph(ux);
	touser(ix2,iy2,z[ix2+iy2* (int32) nnx],&ux,&y2);
	x2 = (int)maph(ux);
	hclipvec(x1,y1,x2,y2,true);

	if (putback) {
		putback = true;
		v_color(sf.bot_color);
	}

}

int doclipping=true;

void clipline(float x1, float y1, float z1, float x2, float y2, float z2) {
	float ux,uy,ux2,uy2;
	int ix1,ix2;

	touser(x1,y1,z1,&ux,&uy);
	touser(x2,y2,z2,&ux2,&uy2);
	if (!doclipping) { v_move(ux,uy); v_line(ux2,uy2); return;}
	ix1 = (int)maph(ux); ix2 = (int)maph(ux2);

/*        printf("hclipvec %d %g  %d %g \n",ix1,uy,ix2,uy2); scr_getch(); */
	if (abs(ix1-ix2)==1) if (fabs(uy2-uy)>.3) ix1 = ix2;
	hclipvec(ix1,uy,ix2,uy2,false);
	hclipvec2(ix1,uy,ix2,uy2,false);
}

void horizonv2(float huge *z,int ix1,int iy1,int ix2,int iy2) {
	float ux;
	int x1,x2;
	float y1,y2;

	touser(ix1,iy1,z[ix1+iy1* (int32) nnx],&ux,&y1);
	x1 = (int)maph(ux);
	touser(ix2,iy2,z[ix2+iy2* (int32) nnx],&ux,&y2);
	x2 = (int)maph(ux);
	hclipvec2(x1,y1,x2,y2,true);
}

void hclipvec(int x1, float y1, int x2, float y2, int sethi) {
	float v,sy = 0,step;
	int i,sd,sx = 0;
	if (x1==x2) {
		/* printf("VEC %d %g   %d %g h=%g\n",x1,y1,x2,y2,h[x1]); scr_getch();  */
        if (y2<y1) {v = y1; y1 = y2; y2 = v;}
		if (h[x1]<y2) {
			if (h[x1]>y1) y1 = h[x1];
			vector_line(x1,y1,x2,y2);
			if (sethi) h[x1] = y2;
		}
		return;
	}
	step = (y2-y1)/(x2-x1);
	sd = -1;
	if (x1<x2) sd = 1;
	step = step*sd;
	bool visible = false;
	for (i=x1, v=y1; sd*i <= sd*x2; i+=sd, v+=step) {
		if (visible) {
			// FIXME: add range check for h-array?
			if (h[i]>v) {
				if (sethi) vector_line(sx,sy,i-sd,v-step);
				else vector_line(sx,sy,i-sd,v-step);
				visible = false;
			} else if (sethi) h[i] = v;
		} else {
			if (h[i]<=v+.0001) {
				sx = i; sy = v; visible = true;
				if (sethi) h[i] = v;
			}
		}
	}
	/* draw end part of line */
	if (visible) vector_line(sx,sy,x2,y2);
}

void seth2(int rx1, int ry1, float rz1, int rx2, int ry2, float rz2) {
	float vx1,vx2,y1,y2;
	int x1,x2;
	float v,step;
	int i,sd,visible;

     /*        printf("seth2 %d %d %g, %d %d %g \n",rx1,ry1,rz1,rx2,ry2,rz2);
	scr_getch(); */
	touser(rx1,ry1,rz1,&vx1,&y1);
	touser(rx2,ry2,rz2,&vx2,&y2);
	x1 = (int)maph(vx1); x2 = (int)maph(vx2);
	if (x1<0) x1 = 0;
	if (x2<0) x2 = 0;
	if (x1>MAXH) x1 = MAXH-1;
	if (x2>MAXH) x2 = MAXH-1;

       if (x1==x2) {
		if (y2>y1) {v = y1; y1 = y2; y2 = v;}
		if (h2[x1]>y2) h2[x1] = y2;
		return;
       }
       step = (y2-y1)/(x2-x1);
       sd = -1;
       if (x1<x2) sd = 1;
       step = step*sd;
       visible = false;
       for (i=x1, v=y1; sd*i <= sd*x2; i+=sd, v+=step) {
		if (h2[i]>v) h2[i] = v;
       }
}

void vector_line_d(double x1, double y1, double x2, double y2) {
	v_move(unmaph(x1),y1);
	v_line(unmaph(x2),y2);
}

void hclipvec2(int x1, float y1, int x2, float y2, int sethi) {
	if (x1 == x2) {
		if (y2 > y1) {
			double v = y1;
			y1 = y2;
			y2 = v;
		}
		if (h2[x1] > y2) {
			if (h2[x1]<y1) y1 = h2[x1];
			vector_line(x1,y1,x2,y2);
			if (sethi) h2[x1] = y2;
		}
		return;
	}
	GLELinearEquation vec, horiz;
	vec.fit(x1, y1, x2, y2);
	int sx = 0;
	int sd = (x1 < x2) ? 1 : -1;
	bool visible = false;
	for (int i = x1; sd*i <= sd*x2; i+=sd) {
		double v = vec.apply(i);
		if (visible) {
			if (h2[i] < v) {
				// drops below horizon -> invisible
				visible = false;
				// find exact point from where on invisible
				horiz.fit(i-sd, h2[i-sd], i, h2[i]);
				double xp = vec.intersect(&horiz);
				vector_line_d(sx, vec.apply(sx), xp, vec.apply(xp));
			} else if (sethi) h2[i] = v;
		} else {
			if (h2[i]>=v-.0001) {
				// back above horizon -> again visible
				visible = true;
				if (i == x1) {
					sx = i;
				} else {
					horiz.fit(i-sd, h2[i-sd], i, h2[i]);
					sx = vec.intersect(&horiz);
				}
				if (sethi) h2[i] = v;
			}
		}
	}
	/* draw end part of line */
	if (visible) vector_line(sx, vec.apply(sx), x2, y2);
}

void vector_line(int x1, float y1, int x2, float y2) {
	if (x2<0 || x1<0) {
		gprint("Less than zero \n");
	}
	v_move(unmaph(x1),y1);
	v_line(unmaph(x2),y2);
}

void touser(float x, float y, float z, float *uux, float *uuy) {
	float uz,ux,uy,p,q;
	ux = image[0][0]*x + image[0][1]*y + image[0][2]*z + image[0][3];
	uy = image[1][0]*x + image[1][1]*y + image[1][2]*z + image[1][3];
	uz = image[2][0]*x + image[2][1]*y + image[2][2]*z + image[2][3];

	ux -= eye_x;
	uy -= eye_y;
	if (maxdepth!=0) {
		p = vdist;                 /* almost touching infinity */
		q = uz/maxdepth; /* z is negative at deep end , zero at top */
		ux = ux - ux*p*q/(1-p+p*q);
		uy = uy - uy*p*q/(1-p+p*q);
/* (my old transformation)
		ux = uz*(-ux/eye_depth)+ux;
		uy = uz*(-uy/eye_depth)+uy;
*/
	}
	*uux = ux + eye_x;
	*uuy = uy + eye_y;
}

void touser3(float x, float y, float z, float *uux, float *uuy, float *uuz) {
	float uz,ux,uy;
	ux = image[0][0]*x + image[0][1]*y + image[0][2]*z + image[0][3];
	uy = image[1][0]*x + image[1][1]*y + image[1][2]*z + image[1][3];
	uz = image[2][0]*x + image[2][1]*y + image[2][2]*z + image[2][3];

	ux -= eye_x;
	uy -= eye_y;
	*uux = ux + eye_x;
	*uuy = uy + eye_y;
	*uuz = uz;
}

void matscale(float i[4][4],float x, float y, float z) {

	static float c[4][4];
	c[0][0] = x;
	c[1][1] = y;
	c[2][2] = z;
	c[3][3] = 1;
	matmul(i,c);
}

void matmove(float i[4][4],float x, float y, float z) {
	static float c[4][4];
	c[0][0] = 1;
	c[1][1] = 1;
	c[2][2] = 1;
	c[3][3] = 1;
	c[0][3] = x;
	c[1][3] = y;
	c[2][3] = z;
	matmul(i,c);
}

void matmul(float a[4][4],float b[4][4])                /* a = a * b */ {
	static float c[4][4],tot;
	int y,xb,x;
	for (y=0;y<4;y++) {
	  for (xb=0;xb<4;xb++) {
		tot = 0;
		for (x=0;x<4;x++) tot += a[x][y] * b[xb][x];
		c[xb][y] = tot;
	  }
	}
	memcpy(a,c,4*4*sizeof(float));
}

void matun(float m[4][4]) {
	int i,j;
	for (i=0;i<4;i++) for (j=0;j<4;j++) m[i][j] = 0;
	for (i=0;i<4;i++) m[i][i] = 1;
}

void matrz(float i[4][4], float angle) {
	/* Rotate around the Z axis */

	float m[4][4];
	matun(m);
	m[0][0] = cos(angle);
	m[1][1] = m[0][0];
	m[0][1] = sin(angle);
	m[1][0] = -m[0][1];
	matmul(i,m);
}

void matry(float i[4][4], float angle) {
	/* Rotate around the y axis */

	float m[4][4];
	matun(m);
	m[0][0] = cos(angle);
	m[2][2] = m[0][0];
	m[2][0] = sin(angle);
	m[0][2] = -m[2][0];
	matmul(i,m);
}

void matrx(float i[4][4], float angle) {
	/* Rotate around the x axis */

	float m[4][4];
	matun(m);
	m[1][1] = cos(angle);
	m[2][2] = m[1][1];
	m[1][2] = sin(angle);
	m[2][1] = -m[1][2];
	matmul(i,m);
}
#define SX(x) ((nx-1)*(x-sf.xaxis.min)/(sf.xaxis.max-sf.xaxis.min))
#define SY(y) ((ny-1)*(y-sf.yaxis.min)/(sf.yaxis.max-sf.yaxis.min))

void grid_back(int nx, int ny, float z1, float z2) {
	float x,y,z;
	v_color(sf.back_color);
	v_lstyle(sf.back_lstyle);
	doclipping = sf.back_hidden;
	if (sf.back_ystep>0) for (y=sf.yaxis.min; y<=sf.yaxis.max+.00001; y+=sf.back_ystep) {
		clipline(0,SY(y),z1,0,SY(y),z2);
	}
	if (sf.back_zstep>0) for (z=z1; z<=z2;  z+=sf.back_zstep) {
		clipline(0,0,z,0,ny-1,z);
	}

	v_color(sf.right_color);
	v_lstyle(sf.right_lstyle);
	doclipping = sf.right_hidden;
	if (sf.right_xstep>0) for (x=sf.xaxis.min; x<=sf.xaxis.max+.00001; x+=sf.right_xstep) {
		clipline(SX(x),ny-1,z1,SX(x),ny-1,z2);
	}
	if (sf.right_zstep>0) for (z=z1; z<=z2;  z+=sf.right_zstep) {
		clipline(0,ny-1,z,nx-1,ny-1,z);
	}


	v_color(sf.base_color);
	v_lstyle(sf.base_lstyle);
	doclipping = sf.base_hidden;
	if (sf.base_xstep>0) for (x=sf.xaxis.min; x<=sf.xaxis.max+.00001; x+=sf.base_xstep) {
		clipline(SX(x),0,z1,SX(x),ny-1,z1);
	}
	if (sf.base_ystep>0) for (y=sf.yaxis.min; y<=sf.yaxis.max+.00001; y+=sf.base_ystep) {
		clipline(0,SY(y),z1,nx-1,SY(y),z1);
	}
}

void draw_markers(int nx, int ny) {
	float *pnt;
	int i;
	pnt = sf.pntxyz;
	if (*sf.marker == 0) return;
	v_color(sf.marker_color);
	DVAL(sf.marker_hei,base/60.0);
	v_set_hei(sf.marker_hei);
	for (i=0; i<sf.npnts; i+=3) {
		move3d(SX(pnt[i]),SY(pnt[i+1]),pnt[i+2]);
		v_marker(sf.marker);
	}
}

void draw_riselines(int nx, int ny,float minz, float maxz) {
	float *pnt=sf.pntxyz;
	int i;

	if (sf.riselines!=0) {
	 v_color(sf.riselines_color);
	 v_lstyle(sf.riselines_lstyle);
	 for (i=0; i<sf.npnts; i+=3) {
		move3d(SX(pnt[i]),SY(pnt[i+1]),pnt[i+2]);
		line3d(SX(pnt[i]),SY(pnt[i+1]),maxz);
	 }
	}

	if (sf.droplines==0) return;
	v_color(sf.droplines_color);
	v_lstyle(sf.droplines_lstyle);
	for (i=0; i<sf.npnts; i+=3) {
		move3d(SX(pnt[i]),SY(pnt[i+1]),pnt[i+2]);
		line3d(SX(pnt[i]),SY(pnt[i+1]),minz);
	}
}

void cube(float x, float y, float z1, float z2) {
	if (sf.cube_hidden_on) doclipping = true;
	else doclipping = false;
	v_color(sf.cube_color);
	v_lstyle(sf.cube_lstyle);

	/* draw axis behind peaks: don't overwrite peaks with round or square cap! */
	g_set_line_cap(0);

	clipline(x,y,z1,0,y,z1);
	clipline(0,y,z1,0,0,z1);

	clipline(0,0,z1,0,0,z2);
	clipline(0,0,z2,0,y,z2);
	clipline(0,y,z2,0,y,z1);
	clipline(0,y,z2,x,y,z2);
	clipline(x,y,z2,x,y,z1);

	doclipping = false;
	clipline(0,0,z1,x,0,z1);
	clipline(x,0,z1,x,y,z1);

	g_set_line_cap(1);

	if (sf.cube_front_on) {
		clipline(0,0,z2,x,0,z2);
		clipline(x,0,z2,x,0,z1);
		clipline(x,0,z2,x,y,z2);
	}
/*

	move3d(0,0,z1);
	v_color(sf.cube_color);
	v_lstyle(sf.cube_lstyle);
	line3d(x,0,z1);
	line3d(x,y,z1);
	line3d(0,y,z1);
	line3d(0,0,z1);

	line3d(0,0,z2);
	line3d(0,y,z2);
	line3d(0,y,z1);
	move3d(0,y,z2);
	line3d(x,y,z2);
	line3d(x,y,z1);
	if (sf.cube_front_on) {
		move3d(0,0,z2);
		line3d(x,0,z2);
		line3d(x,0,z1);
		move3d(x,0,z2);
		line3d(x,y,z2);
	}
	*/
}

void matshow(const char *s, float m[4][4]) {
	printf("\n! Matrix {%s} \n",s);
	for (int i=0;i<4;i++)
		printf("!        %f %f %f %f\n",m[0][i],m[1][i],m[2][i],m[3][i]);

}

void find_splits(int nx, int ny, int *splitx, int *splity) {
	int lasta,i,isleft = false;
	float ux,uy,ux2,uy2,angle,r;

	lasta = 999;
	*splity = -1;
	*splitx = nx-1;
	for (i = 0; i<ny; i++) {
		touser(nx-1,i,0,&ux,&uy);
		touser(0,i,0,&ux2,&uy2);
		fxy_polar(ux2-ux,uy2-uy,&r,&angle);
		if (angle<90) isleft = true;
		if (angle>=90) isleft = false;
		if (lasta==999) lasta = isleft;
		if (lasta!=isleft) {
			*splity = i-1;
		}
		lasta = isleft;
	}
	lasta=999;
	for (i = 0; i<nx; i++) {
		touser(i,0,0,&ux,&uy);
		touser(i,ny-1,0,&ux2,&uy2);
		fxy_polar(ux2-ux,uy2-uy,&r,&angle);
		if (angle<90) isleft = true;
		if (angle>=90) isleft = false;
		if (lasta==999) lasta = isleft;
		if (lasta!=isleft) {
			*splitx = i-1;
		}
		lasta = isleft;
	}
}

void nice_ticks(float *dticks, float *gmin,float *gmax , float *t1,float *tn) {
	float delta,st,expnt,n;
	int ni;

	delta = *gmax-*gmin;
	if (delta==0) {gprint("Axis range error min=%g max=%g \n",*gmin,*gmax);
		*gmax = *gmin+10;
		delta = 10;
	}
	st = delta/10;
	expnt = floor(log10(st));
	n = st/pow((float)10.0,expnt);
	if (n>5)
		ni = 10;
	else if (n>2)
		ni = 5;
	else if (n>1)
		ni = 2;
	else
		ni = 1;
	if (*dticks==0) *dticks = ni * pow((float)10,expnt);
	if (*gmin - (delta/1000) <=  floor( *gmin/ *dticks) * *dticks)
		*t1 = *gmin;
	else
		*t1 = (floor(*gmin/ *dticks) * *dticks ) + *dticks;

	*tn = *gmax;
	if (( floor( *gmax/ *dticks) * *dticks) < (*gmax - (delta/1000) ))
		*tn = floor(*gmax/ *dticks ) * *dticks;
}
