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

#define BEGINDEF extern

#define GLE_GRAPH_LM_PLAIN    0
#define GLE_GRAPH_LM_STEPS    1
#define GLE_GRAPH_LM_FSTEPS   2
#define GLE_GRAPH_LM_HIST     3
#define GLE_GRAPH_LM_IMPULSES 4
#define GLE_GRAPH_LM_BAR      5

/* for key command and gx(), gy() */

#define dbg if ((gle_debug & 64)>0)
extern int gle_debug;

class KeyInfo;

void graph_init(void);
void graph_free(void);
void iffree(void *p, const char *s);
void setrange(double x, double y, int m);
void gdraw_key(KeyInfo* info);
void copy_default(int d);
void do_dataset(int d) throw(ParserError);
void do_each_dataset_settings();
void fill_vec(double x1, double y1, double x2, double y2, vector<double>* vec);
void do_smooth(void);
void window_set(bool showError) throw(ParserError);
void reset_axis_ranges();
bool should_autorange_based_on_lets();
void draw_bars(void);
void draw_lines(void);
void draw_fills(void);
void draw_err(void);
void do_let(const string& letcmd, bool nofirst) throw(ParserError);
void do_let(int line, bool nofirst) throw(ParserError);
void request(void);
/*int draw_axis(void *axis);*/
void bar_reset();
void doskip(char *s,int *ct);
void store_window_bounds_to_vars();
void draw_markers(void) throw (ParserError);
void do_dataset_key(int d);
void do_bigfile_compatibility() throw(ParserError);

#define kw(ss) if (str_i_equals(tk[ct],ss))
#define true (!false)
#define false 0

void var_find_dn(int *idx, int *vara, int *nd);
char *un_quote(char *ct);

#define skipspace doskip(tk[ct],&ct)
//#define tok(n)  (*tk)[n]
#define tok(n)  tk[n]
#define next_exp (get_next_exp(tk,ntk,&ct))
#define next_font ((ct+=1),pass_font(tk[ct]))
#define next_marker ((ct+=1),pass_marker(tk[ct]))
#define next_color ((ct+=1),pass_color_var(tk[ct]))
#define next_fill ((ct+=1),pass_color_var(tk[ct]))
#define next_str(s)  (ct+=1,skipspace,strcpy(s,tk[ct]))
#define next_str_cpp(s)  (ct+=1,skipspace,s=tk[ct])
#define next_vstr(s)  (ct+=1,skipspace,mystrcpy(&s,tk[ct]))
#define next_vquote(s) (ct+=1,skipspace,mystrcpy(&s,un_quote(tk[ct])))
#define next_vquote_cpp(s) (ct+=1,skipspace,pass_file_name(tk[ct],s))
#define next_quote(s) (ct+=1,skipspace,strcpy(s,un_quote(tk[ct])))

struct fill_data {
	int da,db;	/* fill from, too */
	int type; 	/* 1= x1,d1, 2=d1,x2, 3=d1,d2, 4=d1 */
	int color;
	double xmin,ymin,xmax,ymax;
};

class GLEDataSet;

void draw_vec(double x1, double y1, double x2, double y2, GLEDataSet* ds);
void draw_mark(double x1, double y1, int i, double sz, double dval, GLEDataSet* ds) throw (ParserError);

/* range of dataset dimension is initialized in window_set */
/* can be different from axis range because "xmin" / "xmax" / "ymin" / "ymax" settings of dn */

class GLEDataSetDimension {
protected:
	int m_Axis, m_Index;
	GLERangeSet m_Range;
	GLEDataSet* m_Data;
public:
	GLEDataSetDimension();
	~GLEDataSetDimension();
	void copy(GLEDataSetDimension* other);
	int getDataDimensionIndex();
	inline GLERangeSet* getRange() { return &m_Range; }
	inline int getAxis() { return m_Axis; }
	inline void setAxis(int axis) { m_Axis = axis; }
	inline int getIndex() { return m_Index; }
	inline void setIndex(int idx) { m_Index = idx; }
	inline void setDataSet(GLEDataSet* set) { m_Data = set; }
	inline GLEDataSet* getDataSet() { return m_Data; }
};

class GLEDataPairs : public GLERefCountObject {
protected:
	vector<double> m_X;
	vector<double> m_Y;
	vector<int> m_M;
public:
	GLEDataPairs();
	GLEDataPairs(double* x, double* y, int* m, int np);
	GLEDataPairs(GLEDataSet* dataSet);
	virtual ~GLEDataPairs();
	void copy(GLEDataSet* dataSet);
	void copyDimension(GLEDataSet* dataSet, unsigned int dim);
	void resize(int np);
	void set(double* x, double* y, int* m, int np);
	void set(unsigned int i, double x, double y, int m);
	void add(double x, double y, int m);
	void noMissing();
	void noNaN();
	void transformLog(bool xlog, bool ylog);
	void untransformLog(bool xlog, bool ylog);
	void noLogZero(bool xlog, bool ylog);
	vector<double>* getDimension(unsigned int i);
	double getMinXInterval();
	inline unsigned int size() { return m_X.size(); }
	inline double getX(int i) { return m_X[i]; }
	inline double getY(int i) { return m_Y[i]; }
	inline int getM(int i) { return m_M[i]; }
	inline double* getX() { return &m_X[0]; }
	inline double* getY() { return &m_Y[0]; }
	inline int* getM() { return &m_M[0]; }

public:
	static void validate(GLEDataSet* data, unsigned int minDim);
	static double getDataPoint(GLEMemoryCell* element, int datasetID, unsigned int dimension, unsigned int arrayIdx);

private:
	void copyDimensionImpl(GLEArrayImpl* data, unsigned int np, int datasetID, unsigned int dim);
};

class GLEAxis;

class GLEDataSet {
public:
	int id;
	int nomiss;
	unsigned int np; /* NUMBER OF POINTS */
	int autoscale;
	bool axisscale;
	bool inverted;
	char lstyle[9];
	char *key_name;
	char *bigfile;
	int key_fill;
	int key_pattern;
	int key_background;
	double errwidth;
	string errup;
	string errdown;
	double herrwidth;
	string herrup;
	string herrdown;
	double msize,mdist,lwidth;
	vector<string>* yv_str;
	int marker;
	int smooth;
	int smoothm;
	int svg_smooth;           /* Savitski Golay filtering true=on */
	int svg_poly;             /* the type of polynomial 2,3,4,5...*/
	int svg_points;           /* the number of points 5,7,9,11.... */
	int svg_iter;             /* numb or time to do svg smoothing */
	int deresolve;            /* Only plot every N points: true = on */
	bool deresolve_avg;       /* dresolve + average points */
	int line_mode;
	int mdata;
	int color;
	double mscale;
	bool line;
	double rx1,ry1,rx2,ry2;
	GLEDataSetDimension dims[2];
	GLEArrayImpl m_data;
	GLEArrayImpl m_dataBackup;
public:
	GLEDataSet(int identifier);
	~GLEDataSet();
	void copy(GLEDataSet* other);
	void backup();
	void restore();
	void initBackup();
	void clearAll();
	bool undefined();
	GLEDataSetDimension* getDimXInv();
	GLEDataSetDimension* getDimYInv();
	GLEAxis* getAxis(int i);
	void clip(double *x, double *y);
	bool contains(double x, double y);
	void checkRanges() throw(ParserError);
	void copyRangeIfRequired(int dimension);
	vector<int> getMissingValues();
	void validateDimensions();
	void validateNbPoints(unsigned int expectedNb, const char* descr = NULL);
	GLEArrayImpl* getDimData(unsigned int dim);
	void fromData(const vector<double>& xp, const vector<double>& yp, const vector<int>& miss);
	inline GLEDataSetDimension* getDim(int i) { return &dims[i]; }
	inline GLEArrayImpl* getData() { return &m_data; }
	inline GLEArrayImpl* getDataBackup() { return &m_dataBackup; }
};

#define GLE_DIM_X 0
#define GLE_DIM_Y 1

class bar_struct {
public:
	int ngrp;
	int from[20];
	int to[20];
	double width,dist;
	double lwidth[20];
	char lstyle[20][9];
	int fill[20];
	int color[20];
	int side[20];
	int top[20];
	int pattern[20];
	int background[20];
	int notop;
	double x3d,y3d;
	bool horiz;
	string style[20];
	bar_struct();
};

#ifdef __TURBOC__
#define MAXTEMP 2000
#else
#define MAXTEMP 10000
#endif

#ifdef GRAPHDEF
#else
#define GRAPHDEF
#endif

#define MAX_NB_FILL 20

GRAPHDEF double graph_x1,graph_y1,graph_x2,graph_y2;  /* in cm */
GRAPHDEF double graph_xmin,graph_ymin,graph_xmax,graph_ymax; /* graph units */
GRAPHDEF char ebuff[400];
GRAPHDEF int etype,eplen;
GRAPHDEF double xbl,ybl;
GRAPHDEF double xlength,ylength;
GRAPHDEF double g_xsize,g_ysize,g_hscale,g_vscale,g_fontsz;
GRAPHDEF double last_vecx,last_vecy;
GRAPHDEF int ndata,g_nobox,g_center;
GRAPHDEF bool g_auto_s_h, g_auto_s_v;
GRAPHDEF bool g_math;
GRAPHDEF double sizex,sizey;
GRAPHDEF double vscale,hscale;
GRAPHDEF double g_discontinuityThreshold;
GRAPHDEF struct fill_data *fd[MAX_NB_FILL];
GRAPHDEF int nfd;
GRAPHDEF int gntmp;

GRAPHDEF GLEDataSet *dp[MAX_NB_DATA];

GRAPHDEF bar_struct *br[20];
void vinit_axis(int i);
void vinit_title_axis();
void draw_bar(double x, double yf, double yt, double wd, bar_struct* barset, int di, GLEDataSet* toDataSet) throw(ParserError);
void draw_user_function_calls(bool underneath) throw(ParserError);
void get_dataset_ranges();
void set_bar_axis_places();
int get_dataset_identifier(const char* ds, bool def = false) throw(ParserError);

double graph_bar_pos(double xpos, int bar, int set) throw(ParserError);
void begin_graph(int *pln, int *pcode, int *cp) throw (ParserError);
void begin_key(int *pln, int *pcode, int *cp) throw (ParserError);
void begin_tab(int *pln, int *pcode, int *cp);
void begin_text(int *pln, int *pcode, int *cp, double w, int just);
void draw_key(int nkd, struct offset_struct* koffset, char *kpos,double khei, int knobox);
string dimension2String(unsigned int dimension);

#define DP_CAST (struct data_struct*)
#define BR_CAST (struct bar_struct*)
#define AX_CAST (GLEAxis*)
#define FD_CAST (struct fill_data*)

class GLEZData;

class GLEColorMap {
public:
	string m_function;
	string m_palette;
	int m_wd, m_hi;
	bool m_color;
	double m_xmin, m_xmax;
	double m_ymin, m_ymax;
	double m_zmin, m_zmax;
	bool m_has_zmin;
	bool m_has_zmax;
	bool m_invert;
	bool m_haspal;
	GLEZData* m_Data;
public:
	GLEColorMap();
	~GLEColorMap();
	void draw(double x0, double y0, double wd, double hi);
	void setXRange(double min, double max);
	void setYRange(double min, double max);
	void setZMin(double val);
	void setZMax(double val);
	void setPalette(const string& pal);
	void readData();
	inline bool hasZMin() { return m_has_zmin; }
	inline bool hasZMax() { return m_has_zmax; }
	inline double getZMin() { return m_zmin; }
	inline double getZMax() { return m_zmax; }
	inline void setFunction(const char* f) { m_function = f; }
	inline void setWidth(int wd) { m_wd = wd; }
	inline void setHeight(int hi) { m_hi = hi; }
	inline const string& getFunction() { return m_function; }
	inline int getWidth() { return m_wd; }
	inline int getHeight() { return m_hi; }
	inline double getXMin() { return m_xmin; }
	inline double getXMax() { return m_xmax; }
	inline double getYMin() { return m_ymin; }
	inline double getYMax() { return m_ymax; }
	inline bool isColor() { return m_color; }
	inline void setColor(bool color) { m_color = color; }
	inline void setInvert(bool inv) { m_invert = inv; }
	inline bool isInverted() { return m_invert; }
	inline bool hasPalette() { return m_haspal; }
	inline const string& getPaletteFunction() { return m_palette; }
	inline GLEZData* getData() { return m_Data; }
};
