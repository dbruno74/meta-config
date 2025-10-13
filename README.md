# meta-config
meta-config is a Yocto Layer for the configuration of a core-minimal Yocto image, suitable for running [Pulsar](https://github.com/exein-io/pulsar) and the [Exein Runtime Agent](https://www.exein.io/platform/exein-runtime).

> **Note:** `meta-config` is tested on Yocto Scarthgap.

## Usage
Before start: review the Yocto system requirements at
https://docs.yoctoproject.org/dev/ref-manual/system-requirements.html

1. Install [Exein Layer for Yocto](https://github.com/exein-io/meta-exein?tab=readme-ov-file)
2. Download the `meta-config` layer
3. Add the `meta-config` layer to your `bblayers.conf` file
4. Add the following lines to `local.conf' file
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
    ```bash
    bitbake core-image-minimal
    ```

## Configurations
The meta-config layer implements the following configurations
### included packages
- openssl
- openssh server
- udhcpc
- avahi

### configurations
- securityfs mounting
- cgroup2 mounting
- lib64 symlink
