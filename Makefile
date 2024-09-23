EXTENSION = pgpdf
EXTVERSION = 0.1.0

PG_CONFIG ?= pg_config

MODULE_big = $(EXTENSION)

OBJS = pgpdf.o

TESTS = $(wildcard test/sql/*.sql)
REGRESS = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test --load-extension=$(EXTENSION)

/tmp/pgintro.pdf:
	cp test/pgintro.pdf $@
installcheck: /tmp/pgintro.pdf

DATA = $(wildcard sql/*--*.sql)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
HOMEBREW_PREFIX := /opt/homebrew/Cellar

POPPLER_VER := 24.04.0_1
GLIB_VER := 2.82.0
CAIRO_VER := 1.18.2

PG_CPPFLAGS := \
    -I$(HOMEBREW_PREFIX)/poppler/$(POPPLER_VER)/include \
    -I$(HOMEBREW_PREFIX)/glib/$(GLIB_VER)/include \
    -I$(HOMEBREW_PREFIX)/glib/$(GLIB_VER)/include/glib-2.0 \
    -I$(HOMEBREW_PREFIX)/glib/$(GLIB_VER)/lib/glib-2.0/include \
    -I$(HOMEBREW_PREFIX)/cairo/$(CAIRO_VER)/include/cairo

PG_LDFLAGS := \
    -L$(HOMEBREW_PREFIX)/glib/$(GLIB_VER)/lib \
    -L$(HOMEBREW_PREFIX)/poppler/$(POPPLER_VER)/lib
else
PG_CPPFLAGS := $(shell pkg-config --cflags glib-2.0 poppler-glib cairo)
PG_LDFLAGS := $(shell pkg-config --libs glib-2.0 poppler-glib cairo)
endif

SHLIB_LINK=-lglib-2.0 -lpoppler -lgobject-2.0 -lpoppler-glib


PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

dev: clean all install installcheck