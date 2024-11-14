# pgPDF: `pdf` type for Postgres

[![build](https://github.com/Florents-Tselai/pgpdf/actions/workflows/build.yml/badge.svg)](https://github.com/Florents-Tselai/pgpdf/actions/workflows/build.yml)

This extension for PostgreSQL provides a `pdf` data type and assorted functions.

You can create a `pdf` type, by casting either a `text` filepath or `bytea` column.

```tsql
SELECT '/tmp/pgintro.pdf'::pdf;
```

```tsql
                                       pdf                                        
----------------------------------------------------------------------------------
 PostgreSQL Introduction                                                         +
 Digoal.Zhou                                                                     +
 7/20/2011Catalog                                                                +
  PostgreSQL Origin 
```

If you don’t have the PDF file in your filesystem,
but have already stored its content in a `bytea` column,
you can just cast it to `pdf`.

```tsql
SELECT pg_read_binary_file('/tmp/pgintro.pdf')::bytea::pdf;
```


The actual PDF parsing is done by [poppler](https://poppler.freedesktop.org).

Also check blog: 
- [Full Text Search on PDFs With Postgres](https://tselai.com/full-text-search-pdf-postgres)
- [pgpdf: pdf type for Postgres](https://tselai.com/pgpdf-pdf-type-postgres)

**Why?**: 
This allows you to work with PDFs in an ACID-compliant way.
The usual alternative relies on external scripts or services which can easily 
make your data ingestion pipeline brittle and leave your raw data out-of-sync.

## Usage

Download some PDFs. 

```sh
wget https://wiki.postgresql.org/images/e/ea/PostgreSQL_Introduction.pdf -O /tmp/pgintro.pdf
wget https://pdfobject.com/pdf/sample.pdf -O /tmp/sample.pdf
```

Create a table with a `pdf` column:

```tsql
CREATE TABLE pdfs(name text primary key, doc pdf);

INSERT INTO pdfs VALUES ('pgintro', '/tmp/pgintro.pdf');
INSERT INTO pdfs VALUES ('pgintro', '/tmp/sample.pdf');
```

Parsing and validation should happen automatically.
The files will be read from the disk only once!

> [!NOTE]
> The filepath should be accessible by the `postgres` process / user!
> That's different than the user running psql.
> If you don't understand what this means, as your DBA!

### String Functions and Operators

Standard Postgres [String Functions and Operators](https://www.postgresql.org/docs/17/functions-string.html)
should work as usual:

```tsql
SELECT 'Below is the PDF we received ' || '/tmp/pgintro.pdf'::pdf;
SELECT upper('/tmp/pgintro.pdf'::pdf::text);
```

``` tsql
SELECT name
FROM pdfs
WHERE doc::text LIKE '%Postgres%';
```

### Full-Text Search (FTS)

You can also perform full-text search (FTS), since you can work on a `pdf` file like normal text.

```tsql
SELECT '/tmp/pgintro.pdf'::pdf::text @@ to_tsquery('postgres');
```

```tsql
 ?column? 
----------
 t
(1 row)
```

```tsql
SELECT '/tmp/pgintro.pdf'::pdf::text @@ to_tsquery('oracle');
```

```tsql
 ?column? 
----------
 f
(1 row)
```

### Document similarity with `pg_trgm`

You can use [pg_trgm](https://postgresql.org/docs/17/interactive/pgtrgm.html)
to get the similarity between to documents:

```tsql
CREATE EXTENSION pg_trgm;

SELECT similarity('/tmp/pgintro.pdf'::pdf::text, '/tmp/sample.pdf'::pdf::text);
```

### Content

```tsql
SELECT '/tmp/pgintro.pdf'::pdf;
```

```tsql
                                       pdf                                        
----------------------------------------------------------------------------------
 PostgreSQL Introduction                                                         +
 Digoal.Zhou                                                                     +
 7/20/2011Catalog                                                                +
  PostgreSQL Origin 
```

```tsql
SELECT pdf_title('/tmp/pgintro.pdf');
```

```tsql
        pdf_title        
-------------------------
 PostgreSQL Introduction
(1 row)
```

Getting a subset of pages

```tsql
SELECT pdf_num_pages('/tmp/pgintro.pdf');
```

```tsql
 pdf_num_pages 
---------------
            24
(1 row)
```

```tsql
SELECT pdf_page('/tmp/pgintro.pdf', 1);
```

```tsql
           pdf_page           
------------------------------
 Catalog                     +
  PostgreSQL Origin         +
  Layout                    +
  Features                  +
  Enterprise Class Attribute+
  Case
(1 row)
```

```tsql
SELECT pdf_subject('/tmp/pgintro.pdf');
```

```tsql
 pdf_subject 
-------------
 
(1 row)
```

### Metadata

The following functions are available:

- `pdf_title(pdf) → text`
- `pdf_author(pdf) → text`
- `pdf_num_pages(pdf) → integer`

  Total number of pages in the document
- `pdf_page(pdf, integer) → text`

  Get the i-th page as text
- `pdf_creator(pdf) → text`
- `pdf_keywords(pdf) → text`
- `pdf_metadata(pdf) → text`
- `pdf_version(pdf) → text`
- `pdf_subject(pdf) → text`
- `pdf_creation(pdf) → timestamp`
- `pdf_modification(pdf) → timestamp`

```tsql
SELECT pdf_author('/tmp/pgintro.pdf');
```

```tsql
 pdf_author 
------------
 周正中
(1 row)
```

```tsql
SELECT pdf_creation('/tmp/pgintro.pdf');
```

```tsql
       pdf_creation       
--------------------------
 Wed Jul 20 11:13:37 2011
(1 row)
```

```tsql
SELECT pdf_modification('/tmp/pgintro.pdf');
```

```tsql
     pdf_modification     
--------------------------
 Wed Jul 20 11:13:37 2011
(1 row)
```

```tsql
SELECT pdf_creator('/tmp/pgintro.pdf');
```

```tsql
            pdf_creator             
------------------------------------
 Microsoft® Office PowerPoint® 2007
(1 row)
```

```tsql
SELECT pdf_metadata('/tmp/pgintro.pdf');
```

```tsql
 pdf_metadata 
--------------
 
(1 row)
```

```tsql
SELECT pdf_version('/tmp/pgintro.pdf');
```

```tsql
 pdf_version 
-------------
 PDF-1.5
(1 row)
```

## Installation

```
sudo apt install -y libpoppler-glib-dev pkg-config
```
```
cd /tmp
git clone https://github.com/Florents-Tselai/pgpdf.git
cd pgpdf
make
make install
```

```tsql
CREATE EXTENSION pgpdf;
```

> [!WARNING]
> Reading arbitrary binary data (PDF) into your database can pose security risks.
> Only use this for files you trust.

