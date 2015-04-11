lib_LTLIBRARIES = sylicons.la

sylicons_la_SOURCES = sylicons.c sylicons.h
sylicons_la_LDFLAGS = -module -no-undefined -avoid-version

AM_CPPFLAGS = \
	$(GTK_CFLAGS) \
	$(SYLPF_CFLAGS) \
	-I$(includedir) \
	$(SYLPHEED_CFLAGS)

sylicons_la_LIBADD = \
	$(GTK_LIBS) \
	$(SYLPF_LIBS) \
	$(SYLPHEED_LIBS)

install:
install-plugin: install-am
