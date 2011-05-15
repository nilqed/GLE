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

#ifndef __GLE_RUN__
#define __GLE_RUN__

void sub_call(int idx,double *pval,char **pstr,int *npm, int *otyp) throw(ParserError);

void eval(int *pcode,int *cp,double *oval,const char **ostr,int *otyp) throw(ParserError);
GLESub* eval_subroutine_call(int *pcode, int *cp, int* otyp) throw(ParserError);
void eval_do_object_block_call(GLESub* sub, GLEObjectDO* obj) throw(ParserError);

void clear_run();

void g_set_pagesize(const string& papersize) throw (ParserError);
void g_set_margins(const string& margins) throw (ParserError);

GLEFileLocation* get_output_file();
const string& get_input_file();

class GLESub;
class GLESubMap;
class GLEBlocks;

class GLERun {
protected:
	GLEScript* m_Script;
	GLEFileLocation* m_OutFile;
	GLEVars* m_Vars;
	GLEBlocks* m_blockTypes;
	GLERC<GLEObjectRepresention> m_CrObj;
	bool m_AllowBeforeSize[GLE_KW_NB];
public:
	GLERun(GLEScript* script, GLEFileLocation* outfile);
	~GLERun();
	void setBlockTypes(GLEBlocks* blocks);
	GLEBlocks* getBlockTypes();
	void setDeviceIsOpen(bool open);
	void do_pcode(GLESourceLine &SLine,int *srclin, int *pcode, int plen, int *pend, bool& mkdrobjs) throw(ParserError);
	void begin_object(const char* name, GLESub* sub) throw (ParserError);
	void end_object() throw (ParserError);
	void draw_object_static(const string& path, const string& name, int* pcode, int* cp, bool mkdrobjs) throw (ParserError);
	void draw_object_subbyname(GLESub* sub, GLEObjectRepresention* newobj, GLEArrayImpl* path, GLEPoint* orig) throw (ParserError);
	void draw_object_dynamic(int idx, GLEObjectRepresention* newobj, GLEArrayImpl* path, GLEPoint* orig) throw (ParserError);
	void draw_object(const string& name, const char* newname) throw (ParserError);
	void sub_call(GLESub* sub) throw(ParserError);
	void sub_call(int idx, double *pval, char **pstr, int *npm, int *otyp) throw(ParserError);
	void name_set(const char *n, double x1, double y1, double x2, double y2);
	static GLEObjectRepresention* name_to_object(GLEObjectRepresention* obj, GLEArrayImpl* path, GLEJustify* just, unsigned int offs) throw (ParserError);
	GLEObjectRepresention* name_to_object(const char *name, GLEJustify* just) throw(ParserError);
	bool is_name(GLEObjectRepresention* obj, GLEArrayImpl* path, unsigned int offs);
	bool is_name(const char *name);
	void name_to_point(const char *name, GLEPoint* point) throw(ParserError);
	void name_to_size(const char *name, double *wd, double *hi) throw(ParserError);
	void name_join(const char *n1, const char *n2, int marrow, double a1, double a2, double d1, double d2)  throw(ParserError);
	bool box_end() throw (ParserError);
	GLESubMap* getSubroutines();
	inline GLEObjectRepresention* getCRObjectRep() { return m_CrObj.get(); };
	inline void setCRObjectRep(GLEObjectRepresention* obj) { m_CrObj.set(obj); }
	inline GLEVars* getVars() { return m_Vars; }
	inline GLEScript* getScript() { return m_Script; }
	inline GLEGlobalSource* getSource() { return m_Script->getSource(); }
	inline GLEFileLocation* getOutput() { return m_OutFile; }
	inline void allowBeforeSize(int which) { m_AllowBeforeSize[which] = true; }
	inline bool isAllowedBeforeSize(int which) { return m_AllowBeforeSize[which]; }
};

GLERun* getGLERunInstance();

#endif
