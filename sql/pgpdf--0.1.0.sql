CREATE FUNCTION pdf_read_file(text) returns text AS 'MODULE_PATHNAME',
'pdf_read_file'
    LANGUAGE C;
