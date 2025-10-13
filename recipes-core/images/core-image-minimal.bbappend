# Add Avahi + DHCP + Hostname setup
IMAGE_INSTALL:append = " avahi-daemon avahi-utils network-config"

ROOTFS_POSTPROCESS_COMMAND:append = " exein_setup_rcd; setup_network_and_avahi_and_hostname; "

exein_setup_rcd() {
    cat <<'EOF' > ${IMAGE_ROOTFS}/etc/init.d/exein-setup.sh
#!/bin/sh
### BEGIN INIT INFO
# Provides:          exein-setup
# Required-Start:    $local_fs
# Default-Start:     S
# Short-Description: Setup mounts and symlinks for Exein
### END INIT INFO

case "$1" in
  start)
    # Wait briefly to ensure kernel LSMs are initialized
    sleep 1

    # Mount securityfs if not already mounted
    if ! mount | grep -q '/sys/kernel/security'; then
        mkdir -p /sys/kernel/security
        mount -t securityfs none /sys/kernel/security
    fi

    # Mount cgroup2 if not already mounted
    if ! mount | grep -q '/sys/fs/cgroup'; then
        mkdir -p /sys/fs/cgroup
        mount -t cgroup2 none /sys/fs/cgroup
    fi

    # Create /lib64 and symlink all /lib/* there
    if [ ! -d /lib64 ]; then
        mkdir -p /lib64
        for f in /lib/*; do
            ln -sf "$f" /lib64/
        done
    fi
    ;;
  stop)
    # nothing to do
    ;;
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
EOF

    install -d ${IMAGE_ROOTFS}/etc/rcS.d
    ln -sf ../init.d/exein-setup.sh ${IMAGE_ROOTFS}/etc/rcS.d/S05exein-setup.sh
}

setup_network_and_avahi_and_hostname() {
    echo "[meta-network] Configuring DHCP, Avahi and hostname..."

    mkdir -p ${IMAGE_ROOTFS}/etc/init.d

    # --- Hostname ---
    echo "yocto-net" > ${IMAGE_ROOTFS}/etc/hostname
    echo "127.0.1.1 yocto-net" >> ${IMAGE_ROOTFS}/etc/hosts

    # --- DHCP startup script ---
    cat <<'EOF' > ${IMAGE_ROOTFS}/etc/init.d/dhcp.sh
#!/bin/sh
### BEGIN INIT INFO
# Provides:          dhcp
# Required-Start:    $network
# Default-Start:     S
### END INIT INFO
echo "Starting DHCP client on eth0..."
/sbin/udhcpc -i eth0 -q -b &
EOF
    chmod 0755 ${IMAGE_ROOTFS}/etc/init.d/dhcp.sh

    # --- Avahi autostart ---
    if [ -d ${IMAGE_ROOTFS}/etc/systemd/system ]; then
        mkdir -p ${IMAGE_ROOTFS}/etc/systemd/system/multi-user.target.wants
        ln -sf /lib/systemd/system/avahi-daemon.service \
            ${IMAGE_ROOTFS}/etc/systemd/system/multi-user.target.wants/avahi-daemon.service
    else
        echo "/etc/init.d/avahi-daemon start" >> ${IMAGE_ROOTFS}/etc/init.d/rcS
    fi

}

