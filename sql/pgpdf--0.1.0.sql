CREATE TYPE pdf;

CREATE FUNCTION pdf_in(cstring) RETURNS pdf
    IMMUTABLE
    STRICT
    LANGUAGE C
    AS 'MODULE_PATHNAME';

CREATE FUNCTION pdf_out(pdf) RETURNS cstring
    IMMUTABLE
    STRICT
    LANGUAGE C
    AS 'MODULE_PATHNAME';

CREATE TYPE pdf (
    INTERNALLENGTH = -1,
    INPUT = pdf_in,
    OUTPUT = pdf_out
);

CREATE CAST (pdf AS text) WITH INOUT AS ASSIGNMENT;
CREATE CAST (text AS pdf) WITH INOUT AS ASSIGNMENT;

CREATE FUNCTION bytea_to_pdf(bytea) RETURNS pdf
    LANGUAGE C
    IMMUTABLE
    STRICT
AS 'MODULE_PATHNAME';


CREATE FUNCTION pdf_to_bytea(pdf) RETURNS bytea
    LANGUAGE C
    IMMUTABLE
    STRICT
AS 'MODULE_PATHNAME';

CREATE CAST (bytea AS pdf) WITH FUNCTION bytea_to_pdf(bytea) AS ASSIGNMENT;
CREATE CAST (pdf AS bytea) WITH FUNCTION pdf_to_bytea(pdf) AS ASSIGNMENT;

CREATE FUNCTION pdf_read_file(text) returns text AS 'MODULE_PATHNAME',
'pdf_read_file'
    LANGUAGE C;

CREATE FUNCTION pdf_read_bytes(bytea) returns text AS 'MODULE_PATHNAME',
'pdf_read_bytes'
    LANGUAGE C;
