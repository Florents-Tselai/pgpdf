PG_CONFIG = pg_config
PKG_CONFIG = pkg-config

EXTENSION = pgpdf

MODULE_big = $(EXTENSION)

OBJS = pgpdf.o

DATA = $(wildcard sql/*--*.sql)

PG_CFLAGS = -DUSE_ASSERT_CHECKING -Wall -Wextra -Werror -Wno-unused-parameter
PG_CPPFLAGS = $(shell $(PKG_CONFIG) --cflags poppler poppler-glib)
PG_LDFLAGS = $(shell $(PKG_CONFIG) --libs poppler poppler-glib)
SHLIB_LINK =-lpoppler -lpoppler-glib

TESTS = $(wildcard test/sql/*.sql)
REGRESS = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test --load-extension=$(EXTENSION)

TEST_FILES = /tmp/pgintro.pdf /tmp/bad.pdf /tmp/big.pdf
/tmp/pgintro.pdf:
	cp test/pgintro.pdf $@
/tmp/bad.pdf:
	echo 'not a pdf' >> $@
/tmp/big.pdf:
	wget https://www.postgresql.org/files/documentation/pdf/17/postgresql-17-A4.pdf -O $@

installcheck: $(TEST_FILES)

EXTRA_CLEAN = $(TEST_FILES)

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

dev: clean all install installcheck