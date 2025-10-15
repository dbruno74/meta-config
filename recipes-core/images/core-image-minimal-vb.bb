# SPDX-License-Identifier: MIT
DESCRIPTION = "Core minimal image variant with no bootlogd"
LICENSE = "MIT"

IMAGE_NAME= "yocto-vbx86-64"
IMG_HOSTNAME="yocto-vb"
IMAGE_FSTYPES = "wic.vdi"
WKS_FILE = "mkefidisk.wks"

# Reuse core-image-minimal content and customizations
require recipes-core/images/core-image-minimal.bb
require core-image-minimal.inc

# Ensure sysvinit is in the image (if you depend on it)
IMAGE_INSTALL += " sysvinit grub-efi"

# create hostname config file
ROOTFS_POSTPROCESS_COMMAND:prepend = " do_create_host_cfg"

python do_create_host_cfg() {
    import os

    host_cfg_dir = d.getVar('HOST_CFG_DIR', True)
    if not os.path.exists(host_cfg_dir):
        os.makedirs(host_cfg_dir)

    cfg_file = os.path.join(host_cfg_dir, 'hostname.cfg')
    hostname = d.getVar('IMG_HOSTNAME', True) or 'yocto-net'

    with open(cfg_file, 'w') as f:
        f.write('IMG_HOSTNAME="%s"\n' % hostname)

    bb.note("Created hostname config file at %s" % cfg_file)
}

# Remove bootlogd from the final rootfs (postprocess)
ROOTFS_POSTPROCESS_COMMAND += " remove_bootlogd_files;"

remove_bootlogd_files() {
    bbnote "Removing bootlogd files from rootfs"
    # Remove binary
    rm -f ${IMAGE_ROOTFS}/sbin/bootlogd || true

    # Remove init scripts
    rm -f ${IMAGE_ROOTFS}/etc/init.d/bootlogd \
          ${IMAGE_ROOTFS}/etc/init.d/stop-bootlogd || true

    # Remove any rc symlinks that reference bootlogd
    if [ -d ${IMAGE_ROOTFS}/etc ]; then
        find ${IMAGE_ROOTFS}/etc -type l -name '*bootlogd*' -exec rm -f {} \; || true
        find ${IMAGE_ROOTFS}/etc -type f -name '*bootlogd*' -exec rm -f {} \; || true
    fi

    # Extra: remove any leftovers under /usr if present
    rm -f ${IMAGE_ROOTFS}/usr/sbin/bootlogd || true
}

