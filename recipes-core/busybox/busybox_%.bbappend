FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " file://defconfig file://default.script "

do_install:append() {
    install -Dm0755 ${WORKDIR}/sources-unpack/default.script ${D}${datadir}/udhcpc/default.script
    install -d ${D}${sysconfdir}/init.d
}

# Ensure our config merges correctly
do_configure:append() {
    echo "Merging custom BusyBox config"
}

