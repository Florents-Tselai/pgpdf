#include "postgres.h"

#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/jsonb.h"
#include "utils/datetime.h"
#include "utils/date.h"

#include "poppler.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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

#define PG_GETARG_PDF_P(n)	DatumGetPdfP(PG_GETARG_DATUM(n))
#define PG_GETARG_PDF_PP(n)	DatumGetPdfPP(PG_GETARG_DATUM(n))
#define PG_RETURN_PDF_P(x)	PG_RETURN_POINTER(x)

#define PG_GETARG_POPPLER_DOCUMENT(X) ({ \
    pdftype* pdf = PG_GETARG_PDF_P(X); \
    GError* error = NULL; \
    GBytes* pdf_data = g_bytes_new(VARDATA(pdf), VARSIZE_ANY_EXHDR(pdf)); \
    PopplerDocument* doc = poppler_document_new_from_bytes(pdf_data, NULL, &error); \
    g_bytes_unref(pdf_data); \
    if (!doc) { \
        elog(ERROR, "Error parsing PDF document: %s", error->message); \
        g_clear_error(&error); \
    } \
    doc; \
})

PG_FUNCTION_INFO_V1(pdf_in);

Datum
pdf_in(PG_FUNCTION_ARGS)
{
    Datum filename_t = CStringGetTextDatum(PG_GETARG_CSTRING(0));
    Datum pdf_bytes;
    int32 pdf_bytes_len;

    pdftype* result;
    GBytes* g_bytes = NULL;
    PopplerDocument* doc = NULL;
    GError* error = NULL;

    pdf_bytes = DirectFunctionCall1(pg_read_binary_file_all, filename_t);
    pdf_bytes_len = VARSIZE_ANY_EXHDR(pdf_bytes);

    result = (pdftype*)palloc(VARHDRSZ + pdf_bytes_len);
    SET_VARSIZE(result, VARHDRSZ + pdf_bytes_len);

    memcpy(VARDATA(result), VARDATA_ANY(pdf_bytes), pdf_bytes_len);

    g_bytes = g_bytes_new(VARDATA(result), pdf_bytes_len);

    doc = poppler_document_new_from_bytes(g_bytes, NULL, &error);
    g_bytes_unref(g_bytes);

    if (!doc)
    {
        elog(ERROR, "Error parsing PDF document: %s", error->message);
        pfree(result);
        g_clear_error(&error);
        PG_RETURN_NULL();
    }

    g_object_unref(doc);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(pdf_out);

Datum
pdf_out(PG_FUNCTION_ARGS)
{
    PopplerDocument* doc = PG_GETARG_POPPLER_DOCUMENT(0);
    StringInfo strinfo = makeStringInfo();

    int num_pages = poppler_document_get_n_pages(doc);
    for (int i = 0; i < num_pages; i++)
    {
        PopplerPage* page = poppler_document_get_page(doc, i);
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

    PG_RETURN_CSTRING(strinfo->data);
}


PG_FUNCTION_INFO_V1(pdf_title);
Datum
pdf_title(PG_FUNCTION_ARGS)
{
    PopplerDocument *doc = PG_GETARG_POPPLER_DOCUMENT(0);
    const char *title = poppler_document_get_title(doc);

    if (title == NULL)
        PG_RETURN_NULL();

    PG_RETURN_TEXT_P(cstring_to_text(title));
}

PG_FUNCTION_INFO_V1(pdf_author);
Datum
pdf_author(PG_FUNCTION_ARGS)
{
    PopplerDocument *doc = PG_GETARG_POPPLER_DOCUMENT(0);
    const char *author = poppler_document_get_author(doc);

    if (author == NULL)
        PG_RETURN_NULL();

    PG_RETURN_TEXT_P(cstring_to_text(author));
}

PG_FUNCTION_INFO_V1(pdf_creator);
Datum
pdf_creator(PG_FUNCTION_ARGS)
{
    PopplerDocument *doc = PG_GETARG_POPPLER_DOCUMENT(0);
    const char *creator = poppler_document_get_creator(doc);

    if (creator == NULL)
        PG_RETURN_NULL();

    PG_RETURN_TEXT_P(cstring_to_text(creator));
}

PG_FUNCTION_INFO_V1(pdf_keywords);
Datum
pdf_keywords(PG_FUNCTION_ARGS)
{
    PopplerDocument *doc = PG_GETARG_POPPLER_DOCUMENT(0);
    const char *keywords = poppler_document_get_keywords(doc);

    if (keywords == NULL)
        PG_RETURN_NULL();

    PG_RETURN_TEXT_P(cstring_to_text(keywords));
}

PG_FUNCTION_INFO_V1(pdf_metadata);
Datum
pdf_metadata(PG_FUNCTION_ARGS)
{
    PopplerDocument *doc = PG_GETARG_POPPLER_DOCUMENT(0);
    const char *metadata = poppler_document_get_metadata(doc);

    if (metadata == NULL)
        PG_RETURN_NULL();

    PG_RETURN_TEXT_P(cstring_to_text(metadata));
}

PG_FUNCTION_INFO_V1(pdf_version);
Datum
pdf_version(PG_FUNCTION_ARGS)
{
    PopplerDocument *doc = PG_GETARG_POPPLER_DOCUMENT(0);
    const char *version = poppler_document_get_pdf_version_string(doc);

    if (version == NULL)
        PG_RETURN_NULL();

    PG_RETURN_TEXT_P(cstring_to_text(version));
}

PG_FUNCTION_INFO_V1(pdf_subject);
Datum
pdf_subject(PG_FUNCTION_ARGS)
{
    PopplerDocument *doc = PG_GETARG_POPPLER_DOCUMENT(0);
    const char *subject = poppler_document_get_subject(doc);

    if (subject == NULL)
        PG_RETURN_NULL();

    PG_RETURN_TEXT_P(cstring_to_text(subject));
}

PG_FUNCTION_INFO_V1(pdf_num_pages);

Datum
pdf_num_pages(PG_FUNCTION_ARGS)
{
    PopplerDocument* doc = PG_GETARG_POPPLER_DOCUMENT(0);
    PG_RETURN_INT32(poppler_document_get_n_pages(doc));
}


PG_FUNCTION_INFO_V1(pdf_page);

Datum
pdf_page(PG_FUNCTION_ARGS)
{
    PopplerDocument* doc = PG_GETARG_POPPLER_DOCUMENT(0);
    int32 i = PG_GETARG_INT32(1);
    PopplerPage* page = poppler_document_get_page(doc, i);
    PG_RETURN_TEXT_P(cstring_to_text(poppler_page_get_text(page)));
}

PG_FUNCTION_INFO_V1(pdf_creation);

Datum
pdf_creation(PG_FUNCTION_ARGS)
{
    PopplerDocument *doc = PG_GETARG_POPPLER_DOCUMENT(0);
    GDateTime *dt = poppler_document_get_creation_date_time(doc);

    if (dt == NULL)
        PG_RETURN_NULL();

    gint year = g_date_time_get_year(dt);
    gint month = g_date_time_get_month(dt);
    gint day = g_date_time_get_day_of_month(dt);
    gint hour = g_date_time_get_hour(dt);
    gint minute = g_date_time_get_minute(dt);
    gint second = g_date_time_get_second(dt);

    g_date_time_unref(dt);

    TimestampTz ts = DatumGetTimestamp(DirectFunctionCall6(
        make_timestamp,
        Int32GetDatum(year),
        Int32GetDatum(month),
        Int32GetDatum(day),
        Int32GetDatum(hour),
        Int32GetDatum(minute),
        Float8GetDatum((double)second)
    ));

    PG_RETURN_TIMESTAMPTZ(ts);
}

PG_FUNCTION_INFO_V1(pdf_modification);

Datum
pdf_modification(PG_FUNCTION_ARGS)
{
    PopplerDocument *doc = PG_GETARG_POPPLER_DOCUMENT(0);
    GDateTime *dt = poppler_document_get_modification_date_time(doc);

    if (dt == NULL)
        PG_RETURN_NULL();

    gint year = g_date_time_get_year(dt);
    gint month = g_date_time_get_month(dt);
    gint day = g_date_time_get_day_of_month(dt);
    gint hour = g_date_time_get_hour(dt);
    gint minute = g_date_time_get_minute(dt);
    gint second = g_date_time_get_second(dt);

    g_date_time_unref(dt);

    TimestampTz ts = DatumGetTimestamp(DirectFunctionCall6(
        make_timestamp,
        Int32GetDatum(year),
        Int32GetDatum(month),
        Int32GetDatum(day),
        Int32GetDatum(hour),
        Int32GetDatum(minute),
        Float8GetDatum((double)second)
    ));

    PG_RETURN_TIMESTAMPTZ(ts);
}

PG_FUNCTION_INFO_V1(pdf_from_bytea);

Datum
pdf_from_bytea(PG_FUNCTION_ARGS)
{
    bytea* input_bytes = PG_GETARG_BYTEA_P(0); // Get the binary PDF data
    int32 bytes_len = VARSIZE_ANY_EXHDR(input_bytes);
    pdftype* result;

    result = (pdftype*)palloc(VARHDRSZ + bytes_len);
    SET_VARSIZE(result, VARHDRSZ + bytes_len);

    memcpy(VARDATA(result), VARDATA_ANY(input_bytes), bytes_len);

    PG_RETURN_POINTER(result);
}


PG_FUNCTION_INFO_V1(pdf_to_bytea);

Datum
pdf_to_bytea(PG_FUNCTION_ARGS)
{
    pdftype *pdf = PG_GETARG_PDF_P(0);
    bytea *result;

    int32 data_len = VARSIZE_ANY_EXHDR(pdf);

    result = (bytea *) palloc(VARHDRSZ + data_len);
    SET_VARSIZE(result, VARHDRSZ + data_len);

    memcpy(VARDATA(result), VARDATA(pdf), data_len);

    PG_RETURN_BYTEA_P(result);
}

PG_FUNCTION_INFO_V1(bytea_to_pdf);


Datum
bytea_to_pdf(PG_FUNCTION_ARGS)
{
    bytea *bytes = PG_GETARG_BYTEA_PP(0);
    pdftype *result;
    GError *error = NULL;

    GBytes *pdf_data = g_bytes_new(VARDATA_ANY(bytes), VARSIZE_ANY_EXHDR(bytes));

    PopplerDocument *doc = poppler_document_new_from_bytes(pdf_data, NULL, &error);
    if (!doc) {
        g_bytes_unref(pdf_data);
        elog(ERROR, "Error parsing PDF document: %s", error->message);
    }

    result = (pdftype*) palloc(VARHDRSZ + VARSIZE_ANY_EXHDR(bytes));
    SET_VARSIZE(result, VARHDRSZ + VARSIZE_ANY_EXHDR(bytes));

    memcpy(VARDATA(result), g_bytes_get_data(pdf_data, NULL), VARSIZE_ANY_EXHDR(bytes));

    g_object_unref(doc);
    g_bytes_unref(pdf_data);

    PG_RETURN_POINTER(result);
}
