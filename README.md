# pgpdf: Read PDFs from Postgres

[![build](https://github.com/Florents-Tselai/pgpdf/actions/workflows/build.yml/badge.svg)](https://github.com/Florents-Tselai/pgpdf/actions/workflows/build.yml)

Have you ever wanted to apply a function to a `jsonb` object, both dynamically and recursively?

No? Well, here's a Postgres extension that allows you to do just that:

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

