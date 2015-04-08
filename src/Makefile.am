lib_LTLIBRARIES = sylicons.la

sylicons_la_SOURCES = sylicons.c sylicons.h
sylicons_la_LDFLAGS = -module -no-undefined -avoid-version

AM_CPPFLAGS = \
	$(GTK_CFLAGS) \
	$(SYLPF_CFLAGS) \
	-I$(includedir) \
	-I$(sylpheed_build_dir) \
	-I$(sylpheed_build_dir)/libsylph \
	-I$(sylpheed_build_dir)/src \
	-I$(sylplugin_factory_source_dir)/src

sylicons_la_LIBADD = \
	$(GTK_LIBS) \
	$(SYLPF_LIBS) \
	$(sylpheed_build_dir)/src/libsylpheed-plugin-0.la \
	$(sylpheed_build_dir)/libsylph/libsylph-0.la

install:
install-plugin: install-am
