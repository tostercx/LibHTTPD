#
# Dummy Makefile used with makegen.
#
# NOTE : The TOP variable must be set to reflect the top of the
# src tree (either relative or absolute)
#

TOP=..
TARGETS= all install clean

$(TARGETS) :: Makefile.full
	@$(MAKE) -f Makefile.full $@

Makefile.full :: $(TOP)/Site.mm Makefile.tmpl
	@$(TOP)/makegen/makegen $(TOP) ;\
	echo "Done."


