ARG PG_MAJOR=17
FROM postgres:$PG_MAJOR
ARG PG_MAJOR

COPY . /tmp/pgpdf

RUN apt-get update && \
		apt-mark hold locales && \
		apt-get install -y --no-install-recommends libpoppler-glib-dev pkg-config wget build-essential postgresql-server-dev-$PG_MAJOR && \
		cd /tmp/pgpdf && \
		make clean && \
		make install && \
		mkdir /usr/share/doc/pgpdf && \
		cp LICENSE README.md /usr/share/doc/pgpdf && \
		rm -r /tmp/pgpdf && \
		apt-get remove -y pkg-config wget build-essential postgresql-server-dev-$PG_MAJOR && \
		apt-get autoremove -y && \
		apt-mark unhold locales && \
		rm -rf /var/lib/apt/lists/*