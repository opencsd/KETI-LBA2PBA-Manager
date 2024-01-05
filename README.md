## Introduction of KETI-OpenCSD KETI-LBA2PBA-Manager
--------------

KETI-LBA2PBA-Manager returns the physical block address and length of the file to be scanned, as the Host CPU and CSD's File System differ.


## Contents
--------------
[1. Requirement](#1.-requirement)

[2. How To Install](#2.-how-to-build)

[3. Governance](#governance)


## 1. Requirement
--------------
>   gcc-11

>   g++-11

>   gRPC


## 2. How To Build
1. Install gcc-11 & g++-11
```bash
add-apt-repository ppa:ubuntu-toolchain-r/test
apt-get update
apt-get install gcc-11 g++-11
ln /usr/bin/gcc-11 /usr/bin/gcc
ln /usr/bin/g++-11 /usr/bin/g++
```

2. Install gRPC
```bash
apt install -y cmake
apt install -y build-essential autoconf libtool pkg-config
git clone --recurse-submodules -b v1.46.3 --depth 1 --shallow-submodules https://github.com/grpc/grpc
cd grpc
mkdir -p cmake/build
cd cmake/build
cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
      ../..
make –j
make install
cd ../..
```

3. Clone KETI-LBA2PBA-Manager
```bash
git clone https://github.com/opencsd/KETI-LBA2PBA-Manager.git
cd KETI-LBA2PBA-Manager/cmake/build/
```

4. Build
```bash
cmake ../..
make -j
```

## Governance
-------------
This work was supported by Institute of Information & communications Technology Planning & Evaluation (IITP) grant funded by the Korea government(MSIT) (No.2021-0-00862, Development of DBMS storage engine technology to minimize massive data movement)
