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

#include "../all.h"
#include "../core.h"
#include "../file_io.h"
#include "../texinterface.h"
#include "../cutils.h"
#include "../gprint.h"
#include "../tokens/RefCount.h"
#include "../glearray.h"

#define unit_test(a) unit_test_impl(a, #a, __FILE__, __LINE__)

void unit_test_impl(bool expr, const char* exprStr, const char* file, int line) {
	if (!expr) {
		cout << "unit test failed (" << file << ":" << line << "): " << exprStr << std::endl;
		exit(1);
	}
}

void test_csv_reader1() {
	const unsigned int columns[] = { 3, 2, 3, 3, 3, 3, 3, 3, 3 };
	const char* tokens[] = {
			"\"0.123\"", "5.5", "23",
            "België", "25",
            "\"Tussen \"dubbele quotes\"\"", "36", "40",
            "Tussen 'enkele quotes'", "20", "10",
            "\\textbf{hello}", "1", "23.5",
            "\"Comma: ,\"", "你好", "10",
            "\"Drie dubbele quotes \"\"\"\"", "1", "23",
            "\"\"\"", "", "32",
            "A", "B", ""
	};
	const char* input1 = "REM Line with comments\n"
	                     " REM Line with comments\n"
	                     "\"0.123\",5.5    ,23\n"
	                     "België,25          REM Hello\n"
	                     "\"Tussen \"\"dubbele quotes\"\"\",36,40\r\n"
	                     "Tussen 'enkele quotes',20,10\n"
	                     "\\textbf{hello},1,23.5\n"
	                     "\"Comma: ,\",你好, 10\n\r"
	                     "\"Drie dubbele quotes \"\"\"\"\"\"\",1,23\n"
	                     "\"\"\"\",,32\n"
	                     "A,B,";
	GLECSVData reader;
	reader.setDelims(",;\t");
	reader.setCommentIndicator("REM");
	reader.readBuffer(input1);
	GLECSVError* error = reader.getError();
	unit_test(error->errorCode == GLECSVErrorNone);
	unit_test(reader.getNbLines() == 9);
	for (unsigned int i = 0; i < reader.getNbLines(); i++) {
		unit_test(reader.getNbColumns(i) == columns[i]);
	}
	unsigned int pos = 0;
	for (unsigned int i = 0; i < reader.getNbLines(); i++) {
		for (unsigned int j = 0; j < reader.getNbColumns(i); j++) {
			string cellValue = reader.getCellString(i, j);
			unit_test(cellValue == tokens[pos++]);
		}
	}
}

int main(void) {
	test_csv_reader1();
	return 0;
}





