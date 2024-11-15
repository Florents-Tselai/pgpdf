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

TEST_FILES = /tmp/pgintro.pdf /tmp/bad.pdf /tmp/big.pdf
/tmp/pgintro.pdf:
	cp test/pgintro.pdf $@
/tmp/bad.pdf:
	echo 'not a pdf' >> $@
/tmp/big.pdf:
	wget https://www.postgresql.org/files/documentation/pdf/10/postgresql-10-A4.pdf -O $@

installcheck: $(TEST_FILES)

EXTRA_CLEAN = $(TEST_FILES)

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

dev: clean all install installcheck

.PHONY: dist

dist:
	mkdir -p dist
	git archive --format zip --prefix=$(EXTENSION)-$(EXTVERSION)/ --output dist/$(EXTENSION)-$(EXTVERSION).zip main

# for Docker
PG_MAJOR ?= 17

.PHONY: docker

docker:
	docker build --pull --no-cache --build-arg PG_MAJOR=$(PG_MAJOR) -t florents/pgpdf:pg$(PG_MAJOR) -t florents/pgpdf:$(EXTVERSION)-pg$(PG_MAJOR) .

.PHONY: docker-release

docker-release:
	docker buildx build --push --pull --no-cache --platform linux/amd64,linux/arm64 --build-arg PG_MAJOR=$(PG_MAJOR) -t florents/pgpdf:pg$(PG_MAJOR) -t florents/pgpdf:$(EXTVERSION)-pg$(PG_MAJOR) .