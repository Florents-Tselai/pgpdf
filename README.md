# pgPDF: Read PDFs from Postgres

[![build](https://github.com/Florents-Tselai/pgpdf/actions/workflows/build.yml/badge.svg)](https://github.com/Florents-Tselai/pgpdf/actions/workflows/build.yml)

## Usage

```tsql
select pdf_read_file('/abs/path/to/test/pgintro.pdf');
```

## Installation

**Prerequisites**

* [poppler](https://poppler.freedesktop.org)
* [glib](https://docs.gtk.org/glib/)
* [cairo](https://www.cairographics.org)

I've noticed that prefixes for these can be tricky,
especially on MacOS;
check the `Makefile` for ideas.

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

