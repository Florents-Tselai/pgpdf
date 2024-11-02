#include "postgres.h"

#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/jsonb.h"
#include "poppler.h"

#if PG_VERSION_NUM >= 160000
#include <glib.h>
#include "varatt.h"

#endif

#if PG_VERSION_NUM < 130000
#define TYPALIGN_DOUBLE 'd'
#define TYPALIGN_INT 'i'
#endif

PG_MODULE_MAGIC;

static void extract_text_from_pdf(PopplerDocument *document, StringInfo strinfo) {
    int num_pages = poppler_document_get_n_pages(document);
    for (int i = 0; i < num_pages; i++) {
        PopplerPage *page = poppler_document_get_page(document, i);
        if (!page) {
            elog(WARNING, "Failed to get page %d\n", i);
            continue;
        }

        gchar *page_text = poppler_page_get_text(page);
        if (page_text) {
            appendStringInfo(strinfo, "%s", page_text);
            g_free(page_text);
        } else {
            elog(WARNING, "Failed to extract text from page %d\n", i);
        }

        g_object_unref(page);
    }
}

static PopplerDocument *open_pdf_file(const char *file_path, GError **error) {
    return poppler_document_new_from_file(g_strdup_printf("file://%s", file_path), NULL, error);
}

static PopplerDocument *open_pdf_bytes(bytea *bytes, GError **error) {
    return poppler_document_new_from_bytes(g_bytes_new(VARDATA_ANY(bytes), VARSIZE_ANY_EXHDR(bytes)), NULL, error);
}

PG_FUNCTION_INFO_V1(pdf_read_file);

Datum pdf_read_file(PG_FUNCTION_ARGS) {
    char *pdf_file = text_to_cstring(PG_GETARG_TEXT_PP(0));
    GError *error = NULL;
    StringInfo strinfo = makeStringInfo();

    PopplerDocument *document = open_pdf_file(pdf_file, &error);
    if (!document) {
        elog(ERROR, "Error opening PDF document: %s\n", error->message);
    }

    extract_text_from_pdf(document, strinfo);
    g_object_unref(document);

    PG_RETURN_TEXT_P(cstring_to_text(strinfo->data));
}

PG_FUNCTION_INFO_V1(pdf_read_bytes);

Datum pdf_read_bytes(PG_FUNCTION_ARGS) {
    bytea *bytes = PG_GETARG_BYTEA_PP(0);
    GError *error = NULL;
    StringInfo strinfo = makeStringInfo();

    PopplerDocument *document = open_pdf_bytes(bytes, &error);
    if (!document) {
        elog(ERROR, "Error opening PDF document: %s\n", error->message);
    }

    extract_text_from_pdf(document, strinfo);
    g_object_unref(document);

    PG_RETURN_TEXT_P(cstring_to_text(strinfo->data));
}
