## Introduction of KETI-OpenCSD KETI-LBA2PBA-Manager
-------------

KETI-DB-Connector-Instance analyzes queries and generates snippets to pushdown queries to CSD.



## Contents
-------------
[1. Requirement](#1.-requirement)

[2. How To Install](#2.-how-to-build)

[3. Modules](#3.-modules)

[4. Governance](#governance)

## 1. Requirement
-------------
>   gcc-11

>   g++-11

>   gRPC

>   cpprestSDK

>   RapidJSON

>   ODBC


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

3. Install cpprestSDK
```bash
apt-get install libcpprest-dev
```

4. Install RapidJSON
```bash
apt-get install -y rapidjson-dev
```

5. Install ODBC
```bash
wget http://www.unixodbc.org/unixODBC-2.3.7.tar.gz
tar xvzf unixODBC-2.3.7.tar.gz
cd unixODBC-2.3.7/
./configure --prefix=/
make -j
make install
```

6. Clone KETI-DB-Connector-Instance
```bash
git clone https://github.com/opencsd/KETI-DB-Connector-Instance.git
cd KETI-DB-Connector-Instance/cmake/build/
```

7. Build
```bash
cmake ../..
make -j
```

8. Run DB-Connector-Instance
```bash
./db_connector_instance
```


## 3. Modules
-------------
### Query Planner
-------------
- Parses and Analyzes Query
- 

## Governance
-------------
This work was supported by Institute of Information & communications Technology Planning & Evaluation (IITP) grant funded by the Korea government(MSIT) (No.2021-0-00862, Development of DBMS storage engine technology to minimize massive data movement)
