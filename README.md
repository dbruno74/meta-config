# meta-config
meta-config is a Yocto Layer for the configuration of a core-minimal Yocto image, suitable for running [Pulsar](https://github.com/exein-io/pulsar) and the [Exein Runtime Agent](https://www.exein.io/platform/exein-runtime).

> **Note:** `meta-config` is tested on Yocto Scarthgap.

## Usage
Before start: review the Yocto system requirements at
https://docs.yoctoproject.org/dev/ref-manual/system-requirements.html

1. Install [Exein Layer for Yocto](https://github.com/exein-io/meta-exein?tab=readme-ov-file)
2. Download the `meta-config` layer
3. Add the `meta-config` layer to your `bblayers.conf` file
4. Add the following lines to `local.conf` file
```
IMAGE_ROOTFS_SIZE = "524288"
IMAGE_OVERHEAD_FACTOR = "1.2"
TCLIBC = "glibc"

# Prefer Yocto mirrors first
SOURCE_MIRROR_URL ?= "http://downloads.yoctoproject.org/mirror/sources/"
INHERIT += "own-mirrors"

# Disable slow ftp.gnu.org mirrors
PREMIRRORS:prepend = "\
  ftp://.*/.* http://downloads.yoctoproject.org/mirror/sources/ \n \
  http://.*/.* http://downloads.yoctoproject.org/mirror/sources/ \n \
"
IMAGE_INSTALL:append = " openssh openssh-sshd openssh-scp openssh-keygen file binutils glibc-utils glibc glibc-utils glibc-dev libgcc openssl sqlite3 busybox"
```
5. Build your image:
    For a qemu image:
    ```bash
    bitbake core-image-minimal
    ```

    For a Virtual Box compatible image:
    ```bash
    bitbake core-image-minimal-vb
    ```

## Boot the image with qemu
To boot your image with qemu, run the following command
```
runqemu nographic slirp qemuparams="-m 2048"
```
Login with root (no password), or ssh to the running qemu image from your host with this command:
```
ssh -o StrictHostKeyChecking=no root@127.0.0.1 -p 2222
```

## Boot the image with Virtual Box
To boot your image with Virtual Box
- Launch Virtual Box
- Create a New Machine
   - VM Name: yocto-vb
   - Specify virtual hard disk - Use an Existing Virtual Hard Disk File - select <YOCTO_BUILD_DIR>/tmp/deploy/images/qemux86-64/yocto-vbx86-64.wic.vdi
   - Press OK
- Go to Settings - System - tick UEFI
- Go to Network - Attached to Bridge Adapter (select you adapter Name)
- Go to Network - Promiscuous Mode: Allow All
- Press OK
- Run the yocto-vb Virtual Machine

Login with root (no password), or ssh to the running image from your host with this command:
```
ssh -o StrictHostKeyChecking=no root@yocto-vb.local
```

## Run Pulsar
Launch `pulsard`

## Prebuilt Yocto images
Prebuilt Yocto images for qemu and Virtual Box are available in [yocto-pulsar-images](https://github.com/dbruno74/yocto-pulsar-images) repo

## List of configurations
The meta-config layer implements the following configurations:
### included packages
- openssl
- openssh server
- udhcpc
- avahi

### configurations
- securityfs mounting
- cgroup2 mounting
- lib64 symlink
