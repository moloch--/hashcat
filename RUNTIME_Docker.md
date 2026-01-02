# Building and running container with Docker


## Prequesists ##

If you want to run hashcat with your gpus inside docker you first need to install the appropriate container runtime to allow for gpu-passthrough and install usually the latest driver on the host.

### NVidia ###

To enable your docker deamon to support gpu passthrough go here https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/latest/install-guide.html

(The runtime stage in the dockerfile will use an offical nvidia image, make sure your host has at least the same cuda version or a newer version installed. If you wanna change the runtime version of cuda just have a search here https://hub.docker.com/r/nvidia/cuda/tags?name=12.9.1 and exchange it in the dockfile)

Also make sure to install the latest cuda on your host system.

### AMD ###

For AMD go here https://rocm.docs.amd.com/projects/install-on-linux/en/latest/how-to/docker.html


### Intel ###

TBD


## Building and Runtime ##

There will be different dockerfiles for different platforms in the syntax "docker/runtime.PLATFORM.OS.TYPE", each of which only works on that specific platform and requires the host to have the prequesists installed.

Currently there are only dockerfiles for cuda available (docker/runtime.cuda.OS.TYPE), more will follow.

Also there are two TYPE options available to build the image:

## 1. With the official binaries (TYPE=release) (docker/runtime.PLATFORM.OS.release)

This will download the version specified in the dockerfile from the official website and use it.

Here is an example for nvidia on ubuntu:

```bash
docker build -f docker/runtime.cuda.ubuntu24.release -t hashcat .
docker run --rm --gpus=all -it hashcat bash
root@docker:~/hashcat# ./hashcat.bin --help
```

Here is an example for amd on ubuntu:

```bash
docker build -f docker/runtime.amd.ubuntu24.release -t hashcat .
docker run --rm --device /dev/kfd --device /dev/dri/renderD128 --device /dev/dri/renderD129 -it hashcat bash
rocm-user@ae67788b1d87:~/hashcat$ ./hashcat.bin --help
```

## 2. Build the binaries yourself (TYPE=beta) (docker/runtime.PLATFORM.OS.beta)

This will require the official build container to already be built (with the tag hashcat-binaries) successfully and will pull hashcat from it.

Here is an example for nvidia on ubuntu:

```bash
docker build -f docker/BinaryPackage.ubuntu20 -t hashcat-binaries .
docker build -f docker/runtime.cuda.ubuntu24.beta -t hashcat .
docker run --rm --gpus=all -it hashcat bash
root@docker:~/hashcat# ./hashcat.bin --help
```

Here is an example for amd on ubuntu:

```bash
docker build -f docker/BinaryPackage.ubuntu20 -t hashcat-binaries .
docker build -f docker/runtime.amd.ubuntu24.beta -t hashcat .
docker run --rm --device /dev/kfd --device /dev/dri/renderD128 --device /dev/dri/renderD129 -it hashcat bash
rocm-user@ae67788b1d87:~/hashcat$ ./hashcat.bin --help
```

