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
#include "gle-poppler.h"
#include "cutils.h"


#ifdef HAVE_POPPLER

#include <glib.h>
#include <cairo.h>
#include <poppler.h>

void gle_glib_init(int /* argc */, char** /* argv */) {
	g_type_init();
}

void gle_convert_pdf_to_image(char* pdfData,
		                      int pdfLength,
		                      double resolution,
		                      int device,
		                      char** imageData,
		                      int* imageLength)
{
	GError* err = 0;
	PopplerDocument* doc = poppler_document_new_from_data(pdfData, pdfLength, 0, &err);
	if (doc == 0) {
		std::ostringstream errMsg;
		errMsg << ">> error opening PDF: " << err->message;
		g_object_unref(err);
		g_throw_parser_error(errMsg.str());
	}
	PopplerPage* page = poppler_document_get_page(doc, 0);
    if (page == 0) {
    	g_object_unref(doc);
    	g_throw_parser_error(">> error opening PDF: can't read first page");
    }
    double width, height;
    poppler_page_get_size(page, &width, &height);
    int i_width = gle_round_int(resolution * width);
    int i_height = gle_round_int(resolution * height);
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, i_width, i_height);
    cairo_t* cr = cairo_create(surface);
    cairo_scale(cr, resolution, resolution);
    poppler_page_render(page, cr);
    cairo_surface_write_to_png(surface, "/home/jan/bla.png");
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    g_object_unref(page);
    g_object_unref(doc);
}

#else

// stubs

void gle_glib_init(int /* argc */, char** /* argv */) {
}

#endif


