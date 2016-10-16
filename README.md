# FADS
FADS (or "Fails At Drawing Smoothly") is a graphing calculator written for curses-compatible terminals.

### Installation
Right now the build process is pretty simple. After cloning, run `make` to build and `make install` to build and install to /usr/local.

In order to build, FADS requires that GCC can link against 'ncursesw.' If the build process fails for this reason, see [this stackoverflow post](http://stackoverflow.com/questions/26186824/how-to-install-the-ncursesw-development-libraries) to see how to install the ncursesw development library.
