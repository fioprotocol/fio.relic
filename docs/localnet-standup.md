# LocalNet Build, Install and Deployment Guide

## Overview
fio-relic is intended to be the middleware between a fio-nodeos node providing state history via the state-history plugin and a relational database for utilization by consumers such as the fio dashboard or a block explorer. It is based on the opensource product EOS-Chronicle, a middleware app that consumes history data available via the state history plugin of an Antelope (EOSIO) blockchain and provides downstream consumers json formatted data via plugins. Fio-relic, however, is an integrated application incorporating the ingest of state history data, as well as the parsing and storage of that data into a relational database for asynchronous consumption by 3rd party applications.

The following instructions outline the necessary steps to setup a local FIO blockchain, EOS-Chronicle to consume history data from that blockchain and a test consumer to display data to the user. It utilizes a 3 node block producing blockchain, an additional fio-nodeos node attached to that blockchain providing state history data as well as a test server for output of processed data as json to standard out.

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
The following command is a one-stop shop for the clone and build of eos-chronicle and will
* clone eos-chronicle to a tmp directory
* move the install to /opt/eos-chronicle
* update directory and file permissions to run as the default ubuntu user
* build eos-chronicle including any dependencies (llvm, clang).

This is slightly different from the default instructions outlined in the EOS-Chronicle Project, https://github.com/EOSChronicleProject/eos-chronicle, in that it installs to /opt/eos-chronicle instead of /opt/src.

```shell
cd $HOME/tmp && git clone --recursive https://github.com/EOSChronicleProject/eos-chronicle.git && sudo mv eos-chronicle /opt && sudo chown -R ubuntu:ubuntu /opt/eos-chronicle && cd /opt/eos-chronicle && sudo ./pinned_build/install_deps.sh && mkdir build && nice ./pinned_build/chronicle_pinned_build.sh /opt/eos-chronicle/chronicle-deps /opt/eos-chronicle/build $(nproc)
```

### Configure FIO and EOS-Chronicle

#### FIO
As this is a localnet deployment of FIO, the fio.devtools framework will be used and everything should be set up by default. However, for clarity it is important to understand the fio.devtools localnet configuration as well as the fio.devtools history node configuration.

The localnet 3-node default blockchain as well as the state history node is started using the fio.devtools start script, start.sh. The 3-node blockchain is configured to process blocks on ports 9876, 9877, and 9878 and have chain api plugin ports of 8879, 8889 and 8890.

The history node, which is run as a docker container, will connect to the 3-node blockchain described above, ingest blocks and store state history. Its state history api port will be 8080 and any connnections to pull state history will utilize this port.

Refering to this in the EOS Chronicle doc (https://github.com/EOSChronicleProject/eos-chronicle?tab=readme-ov-file#state-history-plugin-in-nodeos) the state history node configuration will have the following attributes/values;

```shell
contracts-console = true
validation-mode = light
plugin = eosio::state_history_plugin
trace-history = true
chain-state-history = true
trace-history-debug-mode = true
state-history-endpoint = 0.0.0.0:8080
```

To expedite this update, the fio.devtools start script has been updated to copy the appropriate history node configuration. For example when starting a state history docker node, the script performs the following;
```shell
cp scripts/launch/history/container/etc/config.ini-statehistory scripts/launch/history/container/etc/config.ini
```

This will be used below when starting the history node 
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

# Select the default P2P Nodeos IP address and P2P Nodeos Port
P2P Nodeos IP address [172.31.20.163]:<Enter>
P2P Nodeos Port [8889]:<Enter>

# Select option 2 to start a state history node
1. V1 History Node 2. State History Node
Choose(#):2<Enter>
```

### Start chronicle test web socket server
Note that this web socket server represents a downstream server showing json formatted state history data processed by EOS-Chronicle and is for test purposes only

```shell
perl /opt/eos-chronicle/testing/chronicle-ws-dumper.pl --port=8891
```

Note that if any missing module errors occur running the script above, then you may be missing one or more module dependencies. These dependencies may be found in the comment section at the top of the script.

```shell
sudo apt install cpanminus libjson-xs-perl libjson-perl
sudo cpanm Net::WebSocket::Server
```

#### Start eos-chronicle
```shell
/opt/eos-chronicle/build/chronicle-receiver --config-dir=/opt/eos-chronicle/config --data-dir=/opt/eos-chronicle/data --end-block=846511
```
