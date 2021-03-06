SUMMARY = "Plugins for the GStreamer multimedia framework 1.x"
HOMEPAGE = "http://gstreamer.freedesktop.org/"
BUGTRACKER = "https://bugzilla.gnome.org/enter_bug.cgi?product=Gstreamer"
SECTION = "multimedia"
DEPENDS = "gstreamer1.0"

inherit autotools pkgconfig

FILESPATH =. "${FILE_DIRNAME}/gst-plugins:"

GSTREAMER_1_0_DEBUG ?= "--disable-debug"
GSTREAMER_1_0_GIT_BRANCH ?= "master"
EXTRA_OECONF = "--disable-valgrind ${GSTREAMER_1_0_DEBUG} --disable-examples "

acpaths = "-I ${S}/common/m4 -I ${S}/m4"

require gstreamer1.0-plugins-package.inc

PACKAGECONFIG[orc] = "--enable-orc,--disable-orc,orc"

PACKAGES_DYNAMIC = "^${PN}-.*"

# apply gstreamer hack after Makefile.in.in in source is replaced by our version from
# ${STAGING_DATADIR_NATIVE}/gettext/po/Makefile.in.in, but before configure is executed 
# http://lists.linuxtogo.org/pipermail/openembedded-core/2012-November/032233.html
oe_runconf_prepend() {
	if [ -e ${S}/po/Makefile.in.in ]; then
		sed -i -e "1a\\" -e 'GETTEXT_PACKAGE = @GETTEXT_PACKAGE@' ${S}/po/Makefile.in.in
	fi
}

python __anonymous () {
    # check if expected prefix is present
    prefix = "gstreamer1.0-"
    bpn = d.getVar("BPN", True)
    if not bpn.startswith(prefix):
        bb.fatal('Invalid GStreamer 1.0 plugin package name "%s" : must start with "%s"' % (bpn, prefix))

    # replaced prefix with "gst-", which is what is used for the tarball and repository filenames
    gstpkg_basename = "gst-" + bpn[len(prefix):]
    pv = d.getVar("PV", True)
    branch = d.getVar("GSTREAMER_1_0_GIT_BRANCH", True)

    if pv == "git":
        d.setVar("SRC_URI", d.getVar("SRC_URI") + " git://anongit.freedesktop.org/gstreamer/%s;branch=%s;protocol=git" % (gstpkg_basename, branch))
    else:
        d.setVar("SRC_URI", d.getVar("SRC_URI") + " http://gstreamer.freedesktop.org/src/%s/%s-%s.tar.xz" % (gstpkg_basename, gstpkg_basename, pv))
}

delete_liblink_m4_file() {
	# This m4 file contains nastiness which conflicts with libtool 2.2.2
	rm "${S}/m4/lib-link.m4" || true
}

do_configure[prefuncs] += " delete_liblink_m4_file "

