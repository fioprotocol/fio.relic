# LocalNet Build, Install and Deployment Guide

## Overview
EOS-Chronicle is a middleware app that consumes history data available via the state history plugin of an Antelope (EOSIO) blockchain. The following instructions outline the necessary steps to setup EOS-Chronicle to consume history data from the FIO blockchain.

## Clone, Build, Install, Configure And Execute a local blockchain, chronicle and a chronicle test web socket server

### Clone fio, fio.contracts, fio.devtools

```shell
mkdir <working directory>
cd <working directory>
git clone https://github.com/fioprotocol/fio.git
git clone https://github.com/fioprotocol/fio.contracts.git
git clone https://github.com/fioprotocol/fio.devtools.git
```

### Build
Only the fio blockchain will be built as the fio contracts will be built during startup. No build is necessary for the fio devtools. As this is a localnet implementation, i.e. where a local 3-node deployment will be utilized, accept the default for the install directory. 


```shell
cd fio/scripts
./fio_build.sh -P
```

### Install
```shell
cd fio/scripts
./fio_install.sh
```

### Clone and build EOS-Chronicle
The following command is a one-stop shop for the clone and build of eos-chronicle. This will clone eos-chronicle to a tmp directory, then move the install to /opt, update directory and file permissions to run as the default ubuntu user and then build eos-chronicle including any dependencies (llvm, clang). This is slightly different from the default instructions outlined in the EOS-Chronicle Project, https://github.com/EOSChronicleProject/eos-chronicle, in that it installs to /opt/eos-chronicle instead of /opt/src.

```shell
cd $HOME/tmp && git clone --recursive https://github.com/EOSChronicleProject/eos-chronicle.git && sudo mv eos-chronicle /opt && sudo chown -R ubuntu:ubuntu /opt/eos-chronicle && cd /opt/eos-chronicle && sudo ./pinned_build/install_deps.sh && mkdir build && nice ./pinned_build/chronicle_pinned_build.sh /opt/eos-chronicle/chronicle-deps /opt/eos-chronicle/build $(nproc)
```

### Configure FIO and EOS-Chronicle

#### FIO
As this is a localnet deployment of FIO, the fio.devtools framework will be used and everything should be set up by default. However, for clarity it is important to understand the fio.devtools localnet configuration as well as the fio.devtools history node configuration.

The localnet 3-node default blockchain as well as the state history node is started using the fio.devtools start script, start.sh. The 3-node blockchain is configured to process blocks on ports 9876, 9877, and 9878 and have chain api plugin ports of 8879, 8889 and 8890.

The history node, which is run as a docker container, will connect to the 3-node blockchain described above, ingest blocks and store state history. Its state history api port will be 8080 and any connnections to pull state history will utilize this port. 

#### EOS-Chronicle
Create the config and the data directory and initialize the eos-chronicle config.ini. The config.ini connection options are as follows;
* host: the nodeos state history host (upstream connection to fio nodeos state history api endpoint)
* port: the nodeos state history api port (upstream connection to fio nodeos state history api endpoint port)
* exp-ws-host: the websocket server host (the downstream connnection to a web socket server host)
* exp-ws-port: the websocket server port (the downstream connnection to a web socket server port)

```shell
mkdir -p /opt/eos-chronicle/config /opt/eos-chronicle/data

cat >/opt/eos-chronicle/config/config.ini <<'EOT'
host = 127.0.0.1
port = 8080
mode = scan
plugin = exp_ws_plugin
exp-ws-host = 127.0.0.1
exp-ws-port = 8891
exp-ws-bin-header = false
EOT
```

### Run the fio-chronicle-webSocket server ecosystem
#### Start fio-nodeos
```shell
# Build contracts (optional)
./start 3.5, option 2

# Start local 3-node blockchain
./start 3.5, option 1, option 1
```

#### Start fio-nodoes state history nodeos
```shell
# Start state history node (as a docker container)
./start 3.5, option 1, option 6
```

### Start chronicle test web socket server
Note that this web socket server represents a downstream server showing json formatted state history data processed by EOS-Chronicle and is for test purposes only

```shell
perl /opt/eos-chronicle/testing/chronicle-ws-dumper.pl --port=8891
```

#### Start eos-chronicle
```shell
/opt/eos-chronicle/build/chronicle-receiver --config-dir=/opt/eos-chronicle/config --data-dir=/opt/eos-chronicle/data --end-block=846511
```