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

#include "all.h"
#include "gle-interface/gle-interface.h"
#include "var.h"
#include "core.h"

bool gle_memory_cell_equals(GLEMemoryCell* a, GLEMemoryCell* b) {
	if (a->Type != b->Type) return false;
	switch (a->Type) {
		case GLE_MC_UNKNOWN:
			return true;
		case GLE_MC_BOOL:
			return a->Entry.BoolVal == b->Entry.BoolVal;
		case GLE_MC_INT:
			return a->Entry.IntVal == b->Entry.IntVal;
		case GLE_MC_DOUBLE:
			return a->Entry.DoubleVal == b->Entry.DoubleVal;
		case GLE_MC_OBJECT:
			return a->Entry.ObjectVal->equals(b->Entry.ObjectVal);
	}
	return false;
}

void gle_memory_cell_print(GLEMemoryCell* a, ostream& out) {
	switch (a->Type) {
		case GLE_MC_UNKNOWN:
			out << "?";
			break;
		case GLE_MC_BOOL:
			out << (a->Entry.BoolVal ? "true" : "false");
			break;
		case GLE_MC_INT:
			out << a->Entry.IntVal;
			break;
		case GLE_MC_DOUBLE:
			out << a->Entry.DoubleVal;
			break;
		case GLE_MC_OBJECT:
			a->Entry.ObjectVal->print(out);
			break;
	}
}

bool gle_memory_cell_to_double(GLEMemoryCell* a, double* result) {
	switch (a->Type) {
		case GLE_MC_BOOL:
			*result = a->Entry.BoolVal ? 1.0 : 0.0;
			return true;
		case GLE_MC_INT:
			*result = a->Entry.IntVal;
			return true;
		case GLE_MC_DOUBLE:
			*result = a->Entry.DoubleVal;
			return true;
		default:
			*result = 0.0;
			return false;
	}
}

GLEDataObject::GLEDataObject() {
}

GLEDataObject::~GLEDataObject() {
}

int GLEDataObject::getType() {
	return GLEObjectTypeUnknown;
}

bool GLEDataObject::equals(GLEDataObject* obj) {
	return false;
}

void GLEDataObject::print(ostream& out) const {
}

GLEPointDataObject::GLEPointDataObject(double x, double y) : m_point(x, y) {
}

GLEPointDataObject::~GLEPointDataObject() {
}

int GLEPointDataObject::getType() {
	return GLEObjectTypePoint;
}

bool GLEPointDataObject::equals(GLEDataObject* obj) {
	if (obj->getType() != GLEObjectTypePoint) return false;
	GLEPointDataObject* other = (GLEPointDataObject*)obj;
	return m_point.getX() == other->m_point.getX()
		   && m_point.getY() == other->m_point.getY();
}

void GLEPointDataObject::print(ostream& out) const {
	m_point.write(out);
}


GLEString::GLEString() {
	m_Data = NULL;
	m_Length = 0;
	m_Alloc = 0;
	m_Intern = 0;
}

GLEString::GLEString(const char* utf8) {
	m_Data = NULL;
	m_Length = 0;
	m_Alloc = 0;
	m_Intern = 0;
	fromUTF8(utf8);
}

GLEString::GLEString(const string& utf8) {
	m_Data = NULL;
	m_Length = 0;
	m_Alloc = 0;
	m_Intern = 0;
	fromUTF8(utf8);
}

GLEString::~GLEString() {
	if (m_Data != NULL) {
		free(m_Data);
	}
}

void GLEString::fromUTF8(const char* str) {
	fromUTF8(str, strlen(str));
}

void GLEString::fromUTF8(const string& str) {
	fromUTF8(str.c_str(), str.length());
}

/*
	0 0000  1 0001  2 0010  3 0011  4 0100  5 0101  6 0110
	7 0111  8 1000  9 1001  a 1010  b 1011  c 1100  d 1101
	e 1110  f 1111

	Char. number range  | UTF-8 octet sequence
	   (hexadecimal)    | (binary)
	--------------------+---------------------------------------------
	0000 0000-0000 007F | 0xxxxxxx
	0000 0080-0000 07FF | 110xxxxx 10xxxxxx
	0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
	0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/

unsigned int getUTF8NumberOfChars(const char* str, unsigned int len) {
	unsigned int i = 0;
	unsigned int dest = 0;
	while (i < len) {
		unsigned char ch = str[i++];
		if ((ch & 0x80) != 0) {
			/* highest bit is one - 3 possibilities */
			int remain = 0;
			if ((ch & 0xE0) == 0xC0) {
				/* 110x -> two byte unicode */
				remain = 1;
			} else if ((ch & 0xF0) == 0xE0) {
				/* 1110x -> three byte unicode */
				remain = 2;
			} else if ((ch & 0xF8) == 0xF0) {
				/* 1111x -> four byte unicode */
				remain = 3;
			} else if ((ch & 0xFC) == 0xF8) {
				remain = 4;
			} else if ((ch & 0xFE) == 0xFC) {
				remain = 5;
			}
			while ((remain > 0) && (i < len)) {
				remain--;
				ch = str[i++];
				if ((ch & 0xC0) != 0x80) {
					remain = 0;
					i--;
					/* try processing this byte separately */
				}
			}
			dest++;
		} else {
			dest++;
		}
	}
	return dest;
}

void GLEString::fromUTF8(const char* str, unsigned int len) {
	resize(len);
	unsigned int i = 0;
	unsigned int dest = 0;
	while (i < len) {
		unsigned char ch = str[i++];
		if ((ch & 0x80) != 0) {
			/* highest bit is one - 3 possibilities */
			int remain = 0;
			unsigned int unicode = 0;
			if ((ch & 0xE0) == 0xC0) {
				/* 110x -> two byte unicode */
				unicode = ch & 0x1F;
				remain = 1;
			} else if ((ch & 0xF0) == 0xE0) {
				/* 1110x -> three byte unicode */
				unicode = ch & 0x0F;
				remain = 2;
			} else if ((ch & 0xF8) == 0xF0) {
				/* 1111x -> four byte unicode */
				unicode = ch & 0x07;
				remain = 3;
			} else if ((ch & 0xFC) == 0xF8) {
				unicode = ch & 0x03;
				remain = 4;
			} else if ((ch & 0xFE) == 0xFC) {
				unicode = ch & 0x01;
				remain = 5;
			} else {
				unicode = (unsigned int)'?';
			}
			while ((remain > 0) && (i < len)) {
				remain--;
				ch = str[i++];
				if ((ch & 0xC0) != 0x80) {
					unicode = (unsigned int)'?';
					remain = 0;
					i--; /* try processing this byte separately */
				} else {
					unicode <<= 6;
					unicode |= ch & 0x3F;
				}
			}
			m_Data[dest++] = unicode;
		} else {
			m_Data[dest++] = ch;
		}
	}
	m_Length = dest;
}

class GLEStringToUTF8 {
protected:
	const GLEString* m_Str;
	char m_Unicode[5];
	unsigned int m_Pos, m_Sub, m_Max;
public:
	GLEStringToUTF8(const GLEString* s);
	char get();
};

GLEStringToUTF8::GLEStringToUTF8(const GLEString* s) {
	m_Str = s;
	m_Pos = 0;
	m_Sub = 0;
	m_Max = 0;
}

char GLEStringToUTF8::get() {
	if (m_Sub < m_Max) {
		return m_Unicode[m_Sub++];
	} else if (m_Pos >= m_Str->length()) {
		return 0;
	} else {
		m_Sub = 0;
		unsigned int unicode = m_Str->get(m_Pos++);
		if (unicode < 0x80) {
			m_Max = 0;
			return (char)unicode;
		} else if (unicode < 0x800) {
			m_Max = 1;
			m_Unicode[0] = 0x80 | (unicode & 0x3F);
			return 0xC0 | ((unicode >> 6) & 0x1F);
		} else if (unicode < 0x10000) {
			m_Max = 2;
			m_Unicode[0] = 0x80 | ((unicode >> 6) & 0x3F);
			m_Unicode[1] = 0x80 | (unicode & 0x3F);
			return 0xE0 | ((unicode >> 12) & 0xF);
		} else if (unicode < 0x200000) {
			m_Max = 3;
			m_Unicode[0] = 0x80 | ((unicode >> 12) & 0x3F);
			m_Unicode[1] = 0x80 | ((unicode >> 6) & 0x3F);
			m_Unicode[2] = 0x80 | (unicode & 0x3F);
			return 0xF0 | ((unicode >> 18) & 0x7);
		} else if (unicode < 0x4000000) {
			m_Max = 4;
			m_Unicode[0] = 0x80 | ((unicode >> 18) & 0x3F);
			m_Unicode[1] = 0x80 | ((unicode >> 12) & 0x3F);
			m_Unicode[2] = 0x80 | ((unicode >> 6) & 0x3F);
			m_Unicode[3] = 0x80 | (unicode & 0x3F);
			return 0xF8 | ((unicode >> 24) & 0x3);
		} else {
			m_Max = 5;
			m_Unicode[0] = 0x80 | ((unicode >> 24) & 0x3F);
			m_Unicode[1] = 0x80 | ((unicode >> 18) & 0x3F);
			m_Unicode[2] = 0x80 | ((unicode >> 12) & 0x3F);
			m_Unicode[3] = 0x80 | ((unicode >> 6) & 0x3F);
			m_Unicode[4] = 0x80 | (unicode & 0x3F);
			return 0xFC | ((unicode >> 30) & 0x1);
		}
		return 0;
	}
}

ostream& GLEString::toUTF8(ostream& out) const {
	GLEStringToUTF8 conv(this);
	while (1) {
		char ch = conv.get();
		if (ch == 0) break;
		out << ch;
	}
	return out;
}

void GLEString::print(ostream& out) const {
	GLEStringToUTF8 conv(this);
	while (1) {
		char ch = conv.get();
		if (ch == 0) break;
		out << ch;
	}
}

void GLEString::toUTF8(string& out) const {
	out.resize(0);
	GLEStringToUTF8 conv(this);
	while (1) {
		char ch = conv.get();
		if (ch == 0) break;
		out.push_back(ch);
	}
}

string GLEString::toUTF8() const {
	string out;
	GLEStringToUTF8 conv(this);
	while (1) {
		char ch = conv.get();
		if (ch == 0) break;
		out.push_back(ch);
	}
	return out;
}

void GLEString::toUTF8(char* out) const {
	int len = 0;
	GLEStringToUTF8 conv(this);
	while (1) {
		char ch = conv.get();
		if (ch == 0) break;
		out[len++] = ch;
	}
	out[len] = 0;
}

unsigned int GLEString::getI(unsigned int i) const {
	unsigned int ch = get(i);
	if (ch >= 'a' && ch <= 'z') ch -= ('a' - 'A');
	return ch;
}

bool GLEString::isSmallerThanI(const GLEString* s2) const {
	unsigned int l1 = length();
	unsigned int l2 = s2->length();
	unsigned int minlen = l1 < l2 ? l1 : l2;
	unsigned int pos = 0;
	/* as long as ch equal */
	while (pos < minlen && getI(pos) == s2->getI(pos)) pos++;
	if (pos >= minlen) {
		/* at end of shortest string */
		return l1 < l2;
	} else {
		/* at first character that differs */
		return getI(pos) < s2->getI(pos);
	}
}

bool GLEString::equalsI(const char* str) {
	unsigned int s_len = strlen(str);
	if (s_len != length()) return false;
	for (unsigned int i = 0; i < s_len; i++) {
		if ((unsigned int)toupper(str[i]) != getI(i)) return false;
	}
	return true;
}

bool GLEString::equals(GLEDataObject* obj) {
	if (obj->getType() != GLEObjectTypeString) return false;
	GLEString* other = (GLEString*)obj;
	if (m_Length != other->length()) return false;
	for (unsigned int i = 0; i < m_Length; i++) {
		if (get(i) != other->get(i)) return false;
	}
	return true;
}

GLEString* GLEString::substring(unsigned int from, unsigned int to) const {
	if (m_Length == 0) {
		return new GLEString();
	} else {
		if (to >= m_Length-1) to = m_Length-1;
		if (from > to) {
			return new GLEString();
		} else {
			unsigned int size = to - from + 1;
			GLEString* res = new GLEString();
			res->resize(size);
			res->m_Length = size;
			unsigned int idx = 0;
			for (unsigned int pos = from; pos <= to; pos++) {
				res->set(idx++, get(pos));
			}
			return res;
		}
	}
}

GLEArrayImpl* GLEString::split(char bych) const {
	GLEArrayImpl* res = new GLEArrayImpl();
	unsigned int pos = 0;
	unsigned int prevpos = 0;
	while (true) {
		while (pos < m_Length && m_Data[pos] != (unsigned int)bych) pos++;
		if (pos >= m_Length) {
			/* not found */
			res->addObject(substring(prevpos, pos));
			break;
		} else {
			/* found - add previous string and skip */
			res->addObject(substring(prevpos, pos-1));
			prevpos = ++pos;
		}
	}
	return res;
}

void GLEString::join(char bych, GLEArrayImpl* arr, int from, int to) {
	int cnt = 0;
	int totlen = 0;
	if (arr->size() == 0) {
		setSize(0);
		return;
	}
	if (to == -1 || to > (int)arr->size()-1) {
		/* make value of to legal */
		to = arr->size()-1;
	}
	if (from > to) {
		/* result is empty */
		setSize(0);
		return;
	}
	for (int i = from; i <= to; i++) {
		GLEString* s_i = (GLEString*)arr->getObjectUnsafe(i);
		totlen += s_i->length();
		cnt++;
	}
	totlen += cnt-1;
	setSize(totlen);
	int pos = 0;
	for (int i = from; i <= to; i++) {
		GLEString* s_i = (GLEString*)arr->getObjectUnsafe(i);
		if (pos != 0) m_Data[pos++] = (unsigned int)bych;
		for (unsigned int j = 0; j < s_i->length(); j++) {
			m_Data[pos++] = s_i->get(j);
		}
	}
}

void GLEString::addQuotes() {
	resize(m_Length+2);
	for (unsigned int i = m_Length; i > 0; i--) {
		m_Data[i] = m_Data[i-1];
	}
	m_Data[0] = '\"';
	m_Data[m_Length+1] = '\"';
	m_Length += 2;
}

void GLEString::setSize(unsigned int size) {
	resize(size);
	m_Length = size;
}

void GLEString::resize(unsigned int size) {
	if (m_Alloc < size) {
		m_Data = (unsigned int*)realloc(m_Data, size*sizeof(unsigned int));
		m_Alloc = size;
	}
}

int GLEString::getType() {
	return GLEObjectTypeString;
}

GLEString* GLEString::getEmptyString() {
	static GLERC<GLEString> g_EmptyString = new GLEString();
	return g_EmptyString.get();
}

GLEArray::GLEArray() {
}

GLEArray::~GLEArray() {
}

GLEArrayImpl::GLEArrayImpl() {
	m_Data = NULL;
	m_Length = 0;
	m_Alloc = 0;
}

GLEArrayImpl::~GLEArrayImpl() {
	clear();
}

void GLEArrayImpl::clear() {
	if (m_Data != NULL) {
		for (unsigned int i = 0; i < m_Length; i++) {
			GLE_MC_DEL_INTERN(&m_Data[i]);
		}
		free(m_Data);
	}
	m_Data = NULL;
	m_Length = 0;
	m_Alloc = 0;
}

int GLEArrayImpl::getType(unsigned int i) {
	switch (m_Data[i].Type) {
		case GLE_MC_UNKNOWN: return GLEObjectTypeUnknown;
		case GLE_MC_BOOL:    return GLEObjectTypeBool;
		case GLE_MC_INT:     return GLEObjectTypeInt;
		case GLE_MC_DOUBLE:  return GLEObjectTypeDouble;
		case GLE_MC_OBJECT:  return m_Data[i].Entry.ObjectVal->getType();
	}
	return GLEObjectTypeUnknown;
}

void GLEArrayImpl::init(unsigned int i) {
	GLE_MC_DEL_INTERN(&m_Data[i]);
	GLE_MC_INIT(m_Data[i]);
}

void GLEArrayImpl::setObject(unsigned int i, GLEDataObject* v) {
	GLE_MC_SET_OBJECT(&m_Data[i], v);
}

void GLEArrayImpl::addObject(GLEDataObject* v) {
	unsigned int len = m_Length;
	ensure(len+1);
	GLE_MC_SET_OBJECT(&m_Data[len], v);
}

void GLEArrayImpl::set(unsigned int i, const GLEMemoryCell* cell) {
	if (cell->Type == GLE_MC_OBJECT) {
		setObject(i, cell->Entry.ObjectVal);
	} else {
		GLE_MC_DEL_INTERN(&m_Data[i]);
		m_Data[i].Entry = cell->Entry;
		m_Data[i].Type = cell->Type;
	}
}

void GLEArrayImpl::setDouble(unsigned int i, double v) {
	GLE_MC_SET_DOUBLE(&m_Data[i], v);
}

double GLEArrayImpl::getDouble(unsigned int i) {
	if (m_Data[i].Type == GLE_MC_DOUBLE) return m_Data[i].Entry.DoubleVal;
	else return 0.0;
}

int GLEArrayImpl::getInt(unsigned int i) {
	if (m_Data[i].Type == GLE_MC_INT) return m_Data[i].Entry.IntVal;
	else return 0;
}

void GLEArrayImpl::setInt(unsigned int i, int v) {
	GLE_MC_SET_INT(&m_Data[i], v);
}

bool GLEArrayImpl::getBool(unsigned int i) {
	if (m_Data[i].Type == GLE_MC_BOOL) return m_Data[i].Entry.BoolVal;
	else return false;
}

void GLEArrayImpl::setBool(unsigned int i, bool v) {
	GLE_MC_SET_BOOL(&m_Data[i], v);
}

void GLEArrayImpl::setUnknown(unsigned int i) {
	GLE_MC_SET_UNKNOWN(&m_Data[i]);
}

bool GLEArrayImpl::isUnknown(unsigned int i) {
	return m_Data[i].Type == GLE_MC_UNKNOWN;
}

GLEDataObject* GLEArrayImpl::getObject(unsigned int i) {
	if (m_Data[i].Type == GLE_MC_OBJECT) return m_Data[i].Entry.ObjectVal;
	else return NULL;
}

GLERC<GLEString> GLEArrayImpl::getString(unsigned int i) {
	GLERC<GLEString> result;
	GLEMemoryCell* cell = &m_Data[i];
	if (cell->Type == GLE_MC_OBJECT && cell->Entry.ObjectVal->getType() == GLEObjectTypeString) {
		result = static_cast<GLEString*>(cell->Entry.ObjectVal);
	} else {
		ostringstream out;
		gle_memory_cell_print(cell, out);
		result = new GLEString(out.str());
	}
	return result;
}

void GLEArrayImpl::ensure(unsigned int size) {
	if (m_Alloc < size) extend(size);
	while (m_Length < size) {
		GLE_MC_INIT(m_Data[m_Length]);
		m_Length++;
	}
}

void GLEArrayImpl::resize(unsigned int size) {
	ensure(size);
	if (size < m_Length) {
		for (unsigned int i = size; i < m_Length; i++) {
			init(i);
		}
		m_Length = size;
	}
}

void GLEArrayImpl::resizeMemory(unsigned int size) {
	if (m_Alloc < size) {
		m_Data = (GLEMemoryCell*)realloc(m_Data, size*sizeof(GLEMemoryCell));
		m_Alloc = size;
	}
}

void GLEArrayImpl::extend(unsigned int size) {
	unsigned int newSize = m_Alloc;
	while (newSize < size) {
		newSize = 2*newSize + 5;
	}
	resizeMemory(newSize);
}

void GLEArrayImpl::enumStrings(ostream& out) {
	out << "       ";
	for (unsigned int i = 0; i < size(); i++) {
		GLEString* key = (GLEString*)getObjectUnsafe(i);
		out << *key;
		if (i != size()-1) {
			out << ", ";
			if ((i+1) % 3 == 0) out << endl << "       ";
		}
	}
}

int GLEArrayImpl::getType() {
	return GLEObjectTypeArray;
}

GLEStringHash::GLEStringHash() : GLEArrayImpl() {
}

GLEStringHash::~GLEStringHash() {
}

void GLEStringHash::setObjectByKey(const GLEStringKey& key, GLEDataObject* v) {
	GLEStringHashData::const_iterator i = m_Map.find(key);
	if (i != m_Map.end()) {
		GLEArrayImpl::setObject(i->second, v);
	} else {
		int len = size();
		ensure(len+1);
		GLEArrayImpl::setObject(len, v);
		m_Map.insert(pair<GLEStringKey, unsigned int>(key, len));
	}
}

GLEDataObject* GLEStringHash::getObjectByKey(const GLEStringKey& key) {
	GLEStringHashData::const_iterator i = m_Map.find(key);
	if (i != m_Map.end()) {
		return GLEArrayImpl::getObject(i->second);
	} else {
		return NULL;
	}
}

void GLEStringHash::getKeys(GLEArrayImpl* keys) {
	for (GLEStringHashData::const_iterator i = m_Map.begin(); i != m_Map.end(); i++) {
		keys->addObject(i->first.get());
	}
}

GLEDynamicSub::GLEDynamicSub() {
	m_Sub = NULL;
	m_VarValues = NULL;
	m_State = NULL;
}

GLEDynamicSub::GLEDynamicSub(GLESub* sub) {
	m_Sub = sub;
	m_VarValues = NULL;
	m_State = NULL;
}

GLEDynamicSub::~GLEDynamicSub() {
	if (m_VarValues != NULL) delete m_VarValues;
	if (m_State != NULL) free(m_State);
}

int GLEDynamicSub::getType() {
	return GLEObjectTypeDynamicSub;
}

GLEObjectRepresention::GLEObjectRepresention() {
}

GLEObjectRepresention::~GLEObjectRepresention() {
}

GLEObjectRepresention* GLEObjectRepresention::getChildObject(GLEString* elem) {
	if (m_SubObjs.isNull()) {
		return NULL;
	} else {
		GLERC<GLEString> key(elem);
		return (GLEObjectRepresention*)m_SubObjs->getObjectByKey(key);
	}
}

bool GLEObjectRepresention::setChildObject(GLEString* elem, GLEObjectRepresention* obj) {
	if (!m_SubObjs.isNull()) {
		GLERC<GLEString> key(elem);
		m_SubObjs->setObjectByKey(key, obj);
		return true;
	} else {
		return false;
	}
}

void GLEObjectRepresention::enableChildObjects() {
	if (m_SubObjs.isNull()) m_SubObjs = new GLEStringHash();
}

int GLEObjectRepresention::getType() {
	return GLEObjectTypeObjectRep;
}

void GLEObjectRepresention::printNames() {
	GLEStringHash* children = getChilds();
	if (children != NULL) {
		GLEStringHashData* hash = children->getHash();
		for (GLEStringHashData::const_iterator i = hash->begin(); i != hash->end(); i++) {
			GLEString* child_name = i->first.get();
			GLEObjectRepresention* child = (GLEObjectRepresention*)children->getObject(i->second);
			cout << *child_name << ": " << *child->getRectangle() << endl;
			child->printNames();
		}
	}
}

void GLEObjectRepresention::translateChildrenRecursive(GLEPoint* trans) {
	GLEStringHash* children = getChilds();
	if (children != NULL) {
		GLEStringHashData* hash = children->getHash();
		for (GLEStringHashData::const_iterator i = hash->begin(); i != hash->end(); i++) {
			GLEObjectRepresention* child = (GLEObjectRepresention*)children->getObject(i->second);
			child->getRectangle()->translate(trans);
			child->translateChildrenRecursive(trans);
		}
	}
}

void GLEObjectRepresention::copyChildrenRecursive(GLEObjectRepresention* newobj, gmodel* oldstate) {
	GLEStringHash* children = getChilds();
	if (children != NULL) {
		GLEStringHashData* hash = children->getHash();
		for (GLEStringHashData::const_iterator i = hash->begin(); i != hash->end(); i++) {
			GLEString* child_name = i->first.get();
			GLEObjectRepresention* child = (GLEObjectRepresention*)children->getObject(i->second);
			newobj->enableChildObjects();
			GLERC<GLEObjectRepresention> new_child = new GLEObjectRepresention();
			newobj->setChildObject(child_name, new_child.get());
			new_child->getRectangle()->copy(child->getRectangle());
			g_undev(new_child->getRectangle(), oldstate);
			g_dev(new_child->getRectangle());
			child->copyChildrenRecursive(new_child.get(), oldstate);
		}
	}
}