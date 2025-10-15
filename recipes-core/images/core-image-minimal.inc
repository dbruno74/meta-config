# core-image-minimal.inc

# Install required packages
IMAGE_INSTALL:append = " avahi-daemon avahi-utils network-config"

HOST_CFG_DIR = "${WORKDIR}/hostcfg"

# --- Set hostname ---
python set_hostname_from_cfg() {
    import os, bb

    rootfs = d.getVar('IMAGE_ROOTFS', True)
    workdir = d.getVar('HOST_CFG_DIR', True)
    cfg_file = os.path.join(workdir, 'hostname.cfg')

    hostname = 'yocto-net'  # default
    if os.path.exists(cfg_file):
        with open(cfg_file, 'r') as f:
            for line in f:
                line = line.strip()
                if line.startswith("IMG_HOSTNAME="):
                    hostname = line.split("=", 1)[1].strip().strip('"')
                    bb.note("Hostname read from cfg: %s" % hostname)

    etc_dir = os.path.join(rootfs, 'etc')
    if not os.path.exists(etc_dir):
        os.makedirs(etc_dir)

    # write /etc/hostname
    with open(os.path.join(etc_dir, 'hostname'), 'w') as f:
        f.write(hostname + '\n')

    # append to /etc/hosts
    hosts_path = os.path.join(etc_dir, 'hosts')
    if not os.path.exists(hosts_path):
        with open(hosts_path, 'w') as f:
            f.write('127.0.0.1 localhost\n')
    with open(hosts_path, 'a') as f:
        f.write('127.0.1.1 {}\n'.format(hostname))
}

# --- Install exein-setup init script ---
python setup_exein_init() {
    import os, stat, bb

    rootfs = d.getVar('IMAGE_ROOTFS', True)
    initd_dir = os.path.join(rootfs, 'etc', 'init.d')
    rcS_dir = os.path.join(rootfs, 'etc', 'rcS.d')

    for dname in [initd_dir, rcS_dir]:
        if not os.path.exists(dname):
            os.makedirs(dname)

    exein_path = os.path.join(initd_dir, 'exein-setup.sh')
    exein_content = """#!/bin/sh
### BEGIN INIT INFO
# Provides:          exein-setup
# Required-Start:    $local_fs
# Default-Start:     S
# Short-Description: Setup mounts and symlinks for Exein
### END INIT INFO

case "$1" in
  start)
    sleep 1
    [ ! -d /sys/kernel/security ] && mkdir -p /sys/kernel/security
    mountpoint -q /sys/kernel/security || mount -t securityfs none /sys/kernel/security

    [ ! -d /sys/fs/cgroup ] && mkdir -p /sys/fs/cgroup
    mountpoint -q /sys/fs/cgroup || mount -t cgroup2 none /sys/fs/cgroup

    [ ! -d /sys/kernel/tracing ] && mkdir -p /sys/kernel/tracing
    mountpoint -q /sys/kernel/tracing || mount -t tracefs tracefs /sys/kernel/tracing

    [ ! -d /lib64 ] && mkdir -p /lib64
    for f in /lib/*; do ln -sf "$f" /lib64/; done
    ;;
  stop) ;;
  restart|reload)
    $0 stop
    $0 start
    ;;
  *)
    echo "Usage: $0 {start|stop|restart}"
    exit 1
    ;;
esac
exit 0
"""
    with open(exein_path, 'w') as f:
        f.write(exein_content)
    os.chmod(exein_path, stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)

    # Symlink to rcS.d
    rcS_link = os.path.join(rcS_dir, 'S05exein-setup.sh')
    if not os.path.exists(rcS_link):
        os.symlink(os.path.join('../init.d', 'exein-setup.sh'), rcS_link)
}

# --- Install DHCP script ---
python setup_dhcp_script() {
    import os, stat, bb

    rootfs = d.getVar('IMAGE_ROOTFS', True)
    initd_dir = os.path.join(rootfs, 'etc', 'init.d')
    if not os.path.exists(initd_dir):
        os.makedirs(initd_dir)

    dhcp_path = os.path.join(initd_dir, 'dhcp.sh')
    dhcp_content = """#!/bin/sh
### BEGIN INIT INFO
# Provides:          dhcp
# Required-Start:    $network
# Default-Start:     S
### END INIT INFO
echo "Starting DHCP client on eth0..."
/sbin/udhcpc -i eth0 -q -b &
"""
    with open(dhcp_path, 'w') as f:
        f.write(dhcp_content)
    os.chmod(dhcp_path, stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)
}

# --- Enable Avahi autostart ---
python setup_avahi_autostart() {
    import os, bb

    rootfs = d.getVar('IMAGE_ROOTFS', True)
    systemd_dir = os.path.join(rootfs, 'etc/systemd/system/multi-user.target.wants')
    initd_dir = os.path.join(rootfs, 'etc/init.d')

    if os.path.exists(os.path.join(rootfs, 'etc/systemd/system')):
        if not os.path.exists(systemd_dir):
            os.makedirs(systemd_dir)
        link_target = '/lib/systemd/system/avahi-daemon.service'
        link_path = os.path.join(systemd_dir, 'avahi-daemon.service')
        if not os.path.exists(link_path):
            os.symlink(link_target, link_path)
    else:
        rcS_file = os.path.join(initd_dir, 'rcS')
        with open(rcS_file, 'a') as f:
            f.write('/etc/init.d/avahi-daemon start\n')
}

# --- 5. Append all functions to ROOTFS_POSTPROCESS_COMMAND ---
ROOTFS_POSTPROCESS_COMMAND:append = " set_hostname_from_cfg; setup_exein_init; setup_dhcp_script; setup_avahi_autostart; "

