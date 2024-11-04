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

typedef struct varlena pdftype;

#define DatumGetPdfP(X)		((pdftype *) PG_DETOAST_DATUM(X))
#define DatumGetPdfPP(X)	((pdftype *) PG_DETOAST_DATUM_PACKED(X))
#define PdfPGetDatum(X)		PointerGetDatum(X)

#define PG_GETARG_PDF_P(n)	DatumGetUriP(PG_GETARG_DATUM(n))
#define PG_GETARG_PDF_PP(n)	DatumGetUriPP(PG_GETARG_DATUM(n))
#define PG_RETURN_PDF_P(x)	PG_RETURN_POINTER(x)


static PopplerDocument* parse_pdf_file(const char* file_path, GError** error)
{
    return poppler_document_new_from_file(g_strdup_printf("file://%s", file_path), NULL, error);
}

static PopplerDocument* parse_pdf_bytes(bytea* bytes, GError** error)
{
    return poppler_document_new_from_bytes(g_bytes_new(VARDATA_ANY(bytes), VARSIZE_ANY_EXHDR(bytes)), NULL, error);
}

PG_FUNCTION_INFO_V1(pdf_in);

Datum
pdf_in(PG_FUNCTION_ARGS)
{
    char* s = PG_GETARG_CSTRING(0);
    pdftype* vardata;
    PopplerDocument* doc = NULL;
    GError* error = NULL;

    doc = parse_pdf_file(s, &error);
    if (!doc)
        elog(ERROR, "Error parsing PDF document: %s\n", error->message);

    g_object_unref(doc);

    vardata = (pdftype*)cstring_to_text(s);
    PG_RETURN_PDF_P(vardata);
}

PG_FUNCTION_INFO_V1(pdf_out);

Datum
pdf_out(PG_FUNCTION_ARGS)
{
    Datum arg = PG_GETARG_DATUM(0);

    PG_RETURN_CSTRING(TextDatumGetCString(arg));
}


static void extract_text_from_pdf(PopplerDocument* document, StringInfo strinfo)
{
    int num_pages = poppler_document_get_n_pages(document);
    for (int i = 0; i < num_pages; i++)
    {
        PopplerPage* page = poppler_document_get_page(document, i);
        if (!page)
        {
            elog(WARNING, "Failed to get page %d\n", i);
            continue;
        }

        gchar* page_text = poppler_page_get_text(page);
        if (page_text)
        {
            appendStringInfo(strinfo, "%s", page_text);
            g_free(page_text);
        }
        else
        {
            elog(WARNING, "Failed to extract text from page %d\n", i);
        }

        g_object_unref(page);
    }
}

PG_FUNCTION_INFO_V1(pdf_read_file);

Datum pdf_read_file(PG_FUNCTION_ARGS)
{
    char* pdf_file = text_to_cstring(PG_GETARG_TEXT_PP(0));
    GError* error = NULL;
    StringInfo strinfo = makeStringInfo();
    PopplerDocument* document = parse_pdf_file(pdf_file, &error);
    if (!document)
        elog(ERROR, "Error opening PDF document: %s\n", error->message);

    extract_text_from_pdf(document, strinfo);
    g_object_unref(document);

    PG_RETURN_TEXT_P(cstring_to_text(strinfo->data));
}

PG_FUNCTION_INFO_V1(pdf_read_bytes);

Datum pdf_read_bytes(PG_FUNCTION_ARGS)
{
    bytea* bytes = PG_GETARG_BYTEA_PP(0);
    GError* error = NULL;
    StringInfo strinfo = makeStringInfo();

    PopplerDocument* document = parse_pdf_bytes(bytes, &error);
    if (!document)
        elog(ERROR, "Error opening PDF document: %s\n", error->message);

    extract_text_from_pdf(document, strinfo);
    g_object_unref(document);

    PG_RETURN_TEXT_P(cstring_to_text(strinfo->data));
}

PG_FUNCTION_INFO_V1(pdf_to_bytea);

Datum pdf_to_bytea(PG_FUNCTION_ARGS)
{
}

PG_FUNCTION_INFO_V1(bytea_to_pdf);

Datum bytea_to_pdf(PG_FUNCTION_ARGS)
{
}
