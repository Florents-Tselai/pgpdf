PG_CONFIG = pg_config
PKG_CONFIG = pkg-config

EXTENSION = pgpdf
EXTVERSION = 0.1.0

MODULE_big = $(EXTENSION)

OBJS = pgpdf.o

DATA = $(wildcard sql/*--*.sql)

PG_CPPFLAGS = $(shell $(PKG_CONFIG) --cflags poppler poppler-glib)
PG_LDFLAGS = $(shell $(PKG_CONFIG) --libs poppler poppler-glib)
SHLIB_LINK =-lpoppler -lpoppler-glib

TESTS = $(wildcard test/sql/*.sql)
REGRESS = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test --load-extension=$(EXTENSION)

/tmp/pgintro.pdf:
	cp test/pgintro.pdf $@

/tmp/bad.pdf:
	echo 'not a pdf' >> $@

installcheck: /tmp/pgintro.pdf /tmp/bad.pdf

EXTRA_CLEAN = /tmp/pgintro.pdf /tmp/bad.pdf

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

dev: clean all install installcheck