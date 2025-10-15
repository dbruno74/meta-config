# SPDX-License-Identifier: MIT
DESCRIPTION = "Core minimal image variant with no bootlogd"
LICENSE = "MIT"

IMAGE_FSTYPES = "tar"
IMAGE_NAME = "yocto-ocix86-64"
IMG_HOSTNAME = "yocto-oci"

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

