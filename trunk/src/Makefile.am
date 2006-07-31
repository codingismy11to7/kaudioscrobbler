# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = kaudioscrobbler.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kaudioscrobbler.pot

KDE_ICON = kaudioscrobbler

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kaudioscrobbler

# the application source, library search path, and link libraries
kaudioscrobbler_SOURCES = main.cpp kaudioscrobbler.cpp
kaudioscrobbler_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kaudioscrobbler_LDADD = $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = kaudioscrobbler.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/kaudioscrobbler
shellrc_DATA = kaudioscrobblerui.rc

