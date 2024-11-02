# pgPDF: Read PDFs from Postgres

[![build](https://github.com/Florents-Tselai/pgpdf/actions/workflows/build.yml/badge.svg)](https://github.com/Florents-Tselai/pgpdf/actions/workflows/build.yml)

## Usage

```sh
wget https://wiki.postgresql.org/images/e/ea/PostgreSQL_Introduction.pdf -O /tmp/pgintro.pdf
```

You can use an absolute path to file as a `text` argument

```tsql
select pdf_read_file('/tmp/pgintro.pdf');
```
```tsql
                                  pdf_read_file                                   
----------------------------------------------------------------------------------
 PostgreSQL Introduction                                                         +
 Digoal.Zhou                                                                     +
 7/20/2011Catalog                                                                +
  PostgreSQL Origin                                                             +
```

If you don't have the PDF file in your filesystem but have already stored its content in a `bytea` column:

```tsql
select pdf_read_bytes(pg_read_binary_file('/tmp/pgintro.pdf'));
```
```tsql

                                  pdf_read_bytes                                  
----------------------------------------------------------------------------------
 PostgreSQL Introduction                                                         +
 Digoal.Zhou                                                                     +
 7/20/2011Catalog                                                                +
  PostgreSQL Origin                                                             +
```

You can now do whatever you want,
for example full-text search is easy:

```tsql
select pdf_read_file('/tmp/pgintro.pdf') @@ to_tsquery('postgres');
```

```tsql
 ?column? 
----------
 t
(1 row)
```

```tsql
select pdf_read_file('/tmp/pgintro.pdf') @@ to_tsquery('oracle');
```

```tsql
 ?column? 
----------
 f
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
create extension pgpdf;
```

> [!WARNING]
> Reading arbitrary binary data (PDF) into your database can pose security risks.
> Only use this for files you trust.

