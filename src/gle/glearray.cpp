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
 * 2004 Jan Struyf
 *
 */

#include "all.h"
#include "core.h"
#include "file_io.h"
#include "texinterface.h"
#include "cutils.h"
#include "gprint.h"
#include "tokens/RefCount.h"
#include "glearray.h"

RefCountObject::RefCountObject() {
	owner_count = 0;
}

RefCountObject::~RefCountObject() {
}

GLEObject::GLEObject() {
}

GLEObject::~GLEObject() {
}

int GLEObject::size() {
	return 0;
}

double GLEObject::getDoubleAt(int i) {
	return 0.0;
}

void GLEObject::setDoubleAt(double v, int i) {
}

bool GLEObject::getBoolAt(int i) {
	return false;
}

void GLEObject::setBoolAt(bool v, int i) {
}

GLEObject* GLEObject::getObjectAt(int i) {
	return NULL;
}

void GLEObject::setObjectAt(GLEObject* v, int i) {
}

GLEObjectArray::GLEObjectArray() {
}

GLEObjectArray::~GLEObjectArray() {
}

int GLEObjectArray::size() {
	return m_Elems.size();
}

GLEObject* GLEObjectArray::getObjectAt(int i) {
	if (i > (int)m_Elems.size()) return NULL;
	else return m_Elems[i].get();
}

void GLEObjectArray::setObjectAt(GLEObject* v, int i) {
	resize(i);
	m_Elems[i] = v;
}

void GLEObjectArray::resize(int n) {
	int add = n - m_Elems.size() + 1;
	while (add > 0) {
		m_Elems.push_back(NULL);
		add--;
	}
}

GLEDoubleArray::GLEDoubleArray() {
}

GLEDoubleArray::~GLEDoubleArray() {
}

int GLEDoubleArray::size() {
	return m_Elems.size();
}

double GLEDoubleArray::getDoubleAt(int i) {
	if (i > (int)m_Elems.size()) return 0.0;
	else return m_Elems[i];
}

void GLEDoubleArray::setDoubleAt(double v, int i) {
	resize(i);
	m_Elems[i] = v;
}

double* GLEDoubleArray::toArray() {
	double* res = (double*)myallocz(sizeof(double) * (m_Elems.size()+1));
	for (vector<double>::size_type i = 0; i < m_Elems.size(); i++) {
		res[i] = m_Elems[i];
	}
	return res;
}

void GLEDoubleArray::resize(int n) {
	int add = n - m_Elems.size() + 1;
	while (add > 0) {
		m_Elems.push_back(0.0);
		add--;
	}
}

GLEBoolArray::GLEBoolArray() {
}

GLEBoolArray::~GLEBoolArray() {
}

int GLEBoolArray::size() {
	return m_Elems.size();
}

bool GLEBoolArray::getBoolAt(int i) {
	if (i > (int)m_Elems.size()) return false;
	else return m_Elems[i];
}

void GLEBoolArray::setBoolAt(bool v, int i) {
	resize(i);
	m_Elems[i] = v;
}

int* GLEBoolArray::toArray() {
	int* res = (int*)myallocz(sizeof(int) * (m_Elems.size()+1));
	for (vector<bool>::size_type i = 0; i < m_Elems.size(); i++) {
		res[i] = m_Elems[i] ? 1 : 0;
	}
	return res;
}

void GLEBoolArray::resize(int n) {
	int add = n - m_Elems.size() + 1;
	while (add > 0) {
		m_Elems.push_back(false);
		add--;
	}
}

GLEZData::GLEZData() {
	m_ZMin = 1e300;
	m_ZMax = -1e300;
	m_NX = 0;
	m_NY = 0;
	m_Data = NULL;
}

GLEZData::~GLEZData() {
	if (m_Data != NULL) delete[] m_Data;
}

void GLEZData::read(const string& fname) throw(ParserError) {
	TokenizerLanguage lang;
	StreamTokenizer tokens(&lang);
	string expanded(GLEExpandEnvironmentVariables(fname));
	validate_file_name(expanded, false);
	tokens.open_tokens(expanded.c_str());
	lang.setSpaceTokens(" \t\r,");
	lang.setSingleCharTokens("\n!");
	// Read the header of the z file
	GLERectangle* bounds = getBounds();
	tokens.ensure_next_token("!");
	while (tokens.has_more_tokens()) {
		string& token = tokens.next_token();
		if (token == "\n") {
			break;
		} else if (str_i_equals(token, "NX")) {
			m_NX = tokens.next_integer();
		} else if (str_i_equals(token, "NY")) {
			m_NY = tokens.next_integer();
		} else if (str_i_equals(token, "XMIN")) {
			bounds->setXMin(tokens.next_double());
		} else if (str_i_equals(token, "XMAX")) {
			bounds->setXMax(tokens.next_double());
		} else if (str_i_equals(token, "YMIN")) {
			bounds->setYMin(tokens.next_double());
		} else if (str_i_equals(token, "YMAX")) {
			bounds->setYMax(tokens.next_double());
		} else {
			stringstream str;
			str << "unknown .z header token '" << token << "'";
			throw tokens.error(str.str());
		}
	}
	lang.setLineCommentTokens("!");
	lang.setSingleCharTokens("");
	lang.setSpaceTokens(" \t\n\r,");
	// Allocate data
	if (m_NX == 0 || m_NY == 0) {
		throw tokens.error("data file header should contain valid NX and NY parameters");
	}
	m_Data = new double[m_NX * m_NY];
	for (int y = 0; y < m_NY; y++) {
		for (int x = 0; x < m_NX; x++) {
			double v = tokens.next_double();
			if (v < m_ZMin) m_ZMin = v;
			if (v > m_ZMax) m_ZMax = v;
			m_Data[x + y * m_NX] = v;
		}
	}
}

GLECSVData::GLECSVData() {
	initDelims();
}

GLECSVData::~GLECSVData() {
	delete[] m_delims;
}

bool GLECSVData::read(const std::string& file) {
	bool result = readBlock(file);
	if (!result) return false;
	parseBlock();
	return true;
}

bool GLECSVData::readBlock(const std::string& fileName) {
	ifstream file(fileName.c_str(), ios::in | ios::binary | ios::ate);
	if (file.is_open()) {
		unsigned int size = file.tellg();
		m_buffer.resize(size + 1);
		file.seekg(0, ios::beg);
		file.read((char*)&m_buffer[0], size);
		file.close();
		return true;
	} else {
		return false;
	}
}

void GLECSVData::parseBlock() {
	m_pos = 0;
	m_size = m_buffer.size();
	m_data = &m_buffer[0];
	GLECSVDataStatus status = readCell();
	while (status != GLECSVDataStatusEOF) {


		status = readCell();
	}
}

void GLECSVData::initDelims() {
	unsigned int size = 256;
	m_delims = new bool[size];
	for (unsigned int i = 0; i < size; i++) {
		m_delims[i] = false;
	}
	m_delims[' '] = true;
	m_delims[','] = true;
	m_delims[';'] = true;
	m_delims['\t'] = true;
}

bool GLECSVData::isDelim(GLEBYTE ch) {
	return m_delims[ch];
}

bool GLECSVData::isSpace(GLEBYTE ch) {
	return ch == ' ';
}

bool GLECSVData::isEol(GLEBYTE ch) {
	return ch == '\n' || ch == '\r';
}

bool GLECSVData::isComment(GLEBYTE ch) {
	return false;
}

void GLECSVData::skipTillEol() {
	while (true) {
		GLEBYTE ch = readChar();
		if (ch == 0) {
			return;
		}
		if (isEol(ch)) {
			removeTrailingEOLs();
			return;
		}
	}
}

GLECSVDataStatus GLECSVData::readCellString(GLEBYTE quote) {
	unsigned int cellSize = 1;
	unsigned int cellPos = lastCharPos();
	initWritePos();
	while (true) {
		GLEBYTE ch = readChar();
		writeChar(ch);
		cellSize++;
		if (ch == 0) {
			// Unterminated string constant
			return GLECSVDataStatusEOF;
		}
		if (isEol(ch)) {
			// Unterminated string constant
			return GLECSVDataStatusEOL;
		}
		if (ch == quote) {
			GLEBYTE ch = readChar();
			if (ch != quote) {
				writeChar(ch);
				createCell(cellSize, cellPos);
				// Skip spaces and delims
				return skipSpacesAndFirstDelim(ch);
			}
		}
	}
	return GLECSVDataStatusOK;
}

GLECSVDataStatus GLECSVData::readCell() {
	GLEBYTE ch = readSignificantChar();
	if (ch == '"' || ch == '\'') {
		return readCellString(ch);
	}
	unsigned int cellSize = 0;
	unsigned int cellPos = lastCharPos();
	while (true) {
		if (ch == 0) {
			createCell(cellSize, cellPos);
			return GLECSVDataStatusEOF;
		}
		if (isEol(ch)) {
			createCell(cellSize, cellPos);
			return removeTrailingEOLs();
		}
		if (isDelim(ch)) {
			createCell(cellSize, cellPos);
			return GLECSVDataStatusOK;
		}
		if (isComment(ch)) {
			createCell(cellSize, cellPos);
			skipTillEol();
			return GLECSVDataStatusEOL;
		}
		cellSize++;
		ch = readChar();
	}
	return GLECSVDataStatusOK;
}

void GLECSVData::createCell(unsigned int cellSize, unsigned int cellPos) {
	cout << "Cell: '";
	for (unsigned int i = 0; i < cellSize; i++) {
		cout << (char)m_data[cellPos + i];
	}
	cout << "'" << endl;
}

GLECSVDataStatus GLECSVData::removeTrailingEOLs() {
	GLEBYTE ch;
	do {
		ch = readChar();
		if (ch == 0) {
			return GLECSVDataStatusEOF;
		}
	} while (isEol(ch));
	goBack();
	return GLECSVDataStatusEOL;
}

void GLECSVData::goBack() {
	if (m_pos > 0) {
		m_pos = m_pos - 1;
	}
}

unsigned int GLECSVData::lastCharPos() {
	if (m_pos > 0) {
		return m_pos - 1;
	} else {
		return 0;
	}
}

GLEBYTE GLECSVData::readChar() {
	if (m_pos == m_size) {
		return 0;
	} else {
		GLEBYTE ch = m_data[m_pos];
		m_pos = m_pos + 1;
		return ch;
	}
}

void GLECSVData::initWritePos() {
	m_writePos = m_pos;
}

void GLECSVData::writeChar(GLEBYTE ch) {
	m_data[m_writePos++] = ch;
}

GLEBYTE GLECSVData::readSignificantChar() {
	GLEBYTE ch;
	do {
		ch = readChar();
		if (ch == 0) {
			return 0;
		}
	} while (isSpace(ch));
	return ch;
}

GLECSVDataStatus GLECSVData::skipSpacesAndFirstDelim(GLEBYTE ch) {
	while (true) {
		if (!isSpace(ch)) {
			if (ch == 0) {
				return GLECSVDataStatusEOF;
			} else if (isEol(ch)) {
				return removeTrailingEOLs();
			} else if (isDelim(ch)) {
				return GLECSVDataStatusOK;
			} else {
				goBack();
				return GLECSVDataStatusOK;
			}
		}
		ch = readChar();
	}
	return GLECSVDataStatusOK;
}
