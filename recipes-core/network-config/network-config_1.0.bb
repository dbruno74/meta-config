SUMMARY = "Network configuration and DHCP autostart"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${WORKDIR}/sources-unpack/LICENSE;md5=a0b5614acd31d1f66c2b9fe2c035f5dd"

SRC_URI = "file://S50network \
           file://LICENSE"

inherit update-rc.d

# Install files
do_install() {
    # Init script
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/sources-unpack/S50network ${D}${sysconfdir}/init.d/S50network
}

# Enable init script at boot
INITSCRIPT_NAME = "S50network"
INITSCRIPT_PARAMS = "defaults 95"

# Ensure BusyBox initscripts are available
RDEPENDS_${PN} = "busybox-initscripts udhcpc"

