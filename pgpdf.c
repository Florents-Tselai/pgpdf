#include "postgres.h"

#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/jsonb.h"
#include "poppler/glib/poppler.h"

#if PG_VERSION_NUM >= 160000

#include "varatt.h"

#endif

#if PG_VERSION_NUM < 130000
#define TYPALIGN_DOUBLE 'd'
#define TYPALIGN_INT 'i'
#endif

PG_MODULE_MAGIC;

#include <poppler/glib/poppler.h>
#include <glib.h>
#include <stdio.h>


PG_FUNCTION_INFO_V1(pdf_read_file);

Datum pdf_read_file(PG_FUNCTION_ARGS) {
    char *pdf_file = text_to_cstring(PG_GETARG_TEXT_PP(0));
    GError *error = NULL;
    PopplerDocument *document;
    gchar *page_text;
    int num_pages; /* Get the number of pages in the PDF */
    StringInfo strinfo = makeStringInfo();

    /* Open the PDF document */
    document = poppler_document_new_from_file(g_strdup_printf("file://%s", pdf_file), NULL, &error);
    if (!document)
        elog(ERROR, "Error opening PDF document: %s\n", error->message);


    num_pages = poppler_document_get_n_pages(document);

    /* Iterate through pages and extract text */
    for (int i = 0; i < num_pages; i++) {
        PopplerPage *page = poppler_document_get_page(document, i);
        if (!page) {
            elog(WARNING, "Failed to get page %d\n", i);
            continue;
        }

        page_text = poppler_page_get_text(page);
        if (page_text) {
            appendStringInfo(strinfo, "%s", page_text);
            g_free(page_text);
        } else {
            elog(WARNING, "Failed to extract text from page %d\n", i);
        }

        g_object_unref(page);
    }

    /* Close the PDF document */
    g_object_unref(document);

    PG_RETURN_TEXT_P(cstring_to_text(strinfo->data));
}
