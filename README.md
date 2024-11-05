# pgPDF: `pdf` type for Postgres

[![build](https://github.com/Florents-Tselai/pgpdf/actions/workflows/build.yml/badge.svg)](https://github.com/Florents-Tselai/pgpdf/actions/workflows/build.yml)

This is an extension for PostgreSQL that provides a `pdf` data type.

The actual PDF parsing is done by [poppler](https://poppler.freedesktop.org).

## Usage

```sh
wget https://wiki.postgresql.org/images/e/ea/PostgreSQL_Introduction.pdf -O /tmp/pgintro.pdf
```

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

If you don't have the PDF file in your filesystem but have already stored its content in a `bytea` column,
you can cast a `bytea` to `pdf`, like so:

```tsql
SELECT pg_read_binary_file('/tmp/pgintro.pdf')::pdf
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

