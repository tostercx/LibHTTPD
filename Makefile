#
# Dummy Makefile used with makegen.
#
# NOTE : The TOP variable must be set to reflect the top of the
# src tree (either relative or absolute)
#

TOP=.
TARGETS= all install clean

$(TARGETS) :: Makefile.full
	@$(MAKE) -f Makefile.full $@

Makefile.full :: $(TOP)/Site.mm Makefile.tmpl
	@$(TOP)/makegen/makegen $(TOP) ;\
	echo "Done."

very-clean: clean
	rm src/config.h
	rm -f config.cache config.log config.status
	rm -f Site.mm

backup:
	@cd .. ;\
	TODAY=`date +%y-%m-%d` ;\
	tar -cf libhttpd-$${TODAY}.tar libhttpd; \
	gzip -9 libhttpd-$${TODAY}.tar
