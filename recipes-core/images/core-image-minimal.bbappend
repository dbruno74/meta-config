require core-image-minimal.inc

IMAGE_NAME="yocto-qemux86-64"
IMG_HOSTNAME="yocto-qemu"

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

ROOTFS_POSTPROCESS_COMMAND:prepend = " do_create_host_cfg"

