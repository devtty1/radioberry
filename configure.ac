AC_INIT([radioberry], [0.0.0], [devtty0@gmail.com], [radioberry])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE(no-dist-gzip dist-bzip2)

AC_SUBST(PACKAGE_AUTHOR, ["Luotao Fu"])
AC_SUBST(PACKAGE_DESCRIPTION, ["Web radio with analog control"])
AC_SUBST(PACKAGE_HOMEPAGE, ["https://github.com/devtty1/radioberry"])
AC_SUBST(PACKAGE_MONTH, [$(LANG=C date '+%B')])
AC_SUBST(PACKAGE_YEAR, [$(LANG=C date '+%Y')])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

AC_MSG_CHECKING(for sysroot definition)
AC_ARG_WITH([sysroot],
            AS_HELP_STRING([path to sysroot of target]),
            [sysroot="$withval"])

if test -z "${sysroot}"; then
	AC_MSG_RESULT(none set -- Warning: might fail to cross compile)
else
	SYSROOT_FLAG="--sysroot=$sysroot"
	LDFLAGS="$LDFLAGS $SYSROOT_FLAG"
	AC_MSG_RESULT($sysroot)
fi
AC_SUBST([SYSROOT_FLAG])

# Checks for libraries.
AC_CHECK_LIB([mpd], [mpd_new], MPD_LIB="-lmpd", AC_MSG_ERROR([libmpd not found.]))
AC_SUBST([MPD_LIB])

AC_CHECK_LIB([confuse], [cfg_init], CONFUSE_LIB="-lconfuse", AC_MSG_ERROR([libconfuse not found.]))
AC_SUBST([CONFUSE_LIB])

AC_CHECK_LIB([asound], [snd_mixer_open], ASOUND_LIB="-lasound", AC_MSG_ERROR([asound not found.]))
AC_SUBST([ASOUND_LIB])

# Checks for header files.
AC_STDC_HEADERS

# Checks for typedefs, structures, and compiler characteristics.

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
