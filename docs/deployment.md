# FIO.Relic Deployment Guide

The FIO.Relic Deployment Guide outlines the steps necessary to build, install and deploy the FIO.Chronicle history middleware application, the FIO.Chronicle persistence layer, PostgreSQL, as well as configuration for connecting to a FIO Nodeos node publishing history data via its state history API.

While the FIO.Chronicle historical data processor is still under development, this guide contains the latest information to stand up each component of the system.

## PostgreSQL, FIO.Chronicle and FIO Nodeos Deployment Overview
The FIO.Relic ecosystem is comprised of PostgreSQL, FIO.Chronicle and FIO Nodeos Blockchain. While this guide includes documentation on deploying a FIO Blockchain Node, the assumption is that a FIO Blockchain node is already stood up, configured and running. Regardless, there are three components to a FIO.Relic System;
1) FIO Nodeos Blockchain node
2) FIO.Chronicle
3) PostgreSQL RDMS

Each component of the FIO.Relic ecosystem has configuration parameters for history data processing. For instance, a FIO Nodeos history node will process and provide history via the state history plugin, FIO.Chronicle will query state history (via the state history api), process it, then persist transformed data into its PostgreSQL database.

The following table describes the attributes that must be set to successfully stand up the FIO.Relic ecosystem. Use this table as a template substituting specific target environment values. These will be needed when configuring [FIO.Chronicle](#build-install-and-configure)

| Application | Associated Parameter | Default Value | Target Env Value | Purpose |
|-|-|-|-|-|
| FIO Nodeos | state-history-endpoint | 0.0.0.0:8080 | | FIO.Nodeos Listen IP Mask and Port |
| FIO Nodeos | host | 127.0.0.1 | | FIO.Nodeos State History API Host |
| FIO Nodeos | port | 8080 | | FIO.Nodeos State History API Port |
| PostgreSQL | exp-relic-host | 127.0.0.1 | | RDMS Host |
| PostgreSQL | exp-relic-port | 5432 | | RDMS Port |
| PostgreSQL | exp-relic-db | relicdb | | RDMS Database |
| PostgreSQL | exp-relic-username | chronicle_user | | RDMS User |
| PostgreSQL | exp-relic-password | password123! | | RDMS Password |

## FIO Nodeos
The following information describes standup of a TestNet or MainNet history node. To connect to an existing TestNet/MainNet state history node, all that is required is to update the fio.chronicle connection configuration. See the FIO.Chronicle [documentation](#fiochronicle) below.

### Installation
The FIO Nodeos source code, and build instructions are found at the [FIO GitHub Repository](https://github.com/fioprotocol/fio). For installation using official packages see the [FIO Package Install](https://dev.fio.net/docs/install-using-packages) page.

### Configuration
A FIO Nodeos [node](https://dev.fio.net/docs/chain-node) must be configured to process and expose state history, including state and trace history data, via the FIO Nodeos state history plugin. The FIO Nodeos installation is straightforward, however, multiple steps must be followed including;
* [FIO Nodeos Configuration](https://dev.fio.net/docs/configure-and-run-your-node)
* [Replaying blockchain blocks](https://dev.fio.net/docs/nodeos-replay)

Note that the important configuration parameters include;
* plugin = eosio::state_history_plugin
* state-history-endpoint = 0.0.0.0:8080
* chain-state-history = true
* trace-history = true

To expedite the FIO Nodeos installation, including blockchain configuration and history playback, use the [install script](https://dev.fio.net/docs/install-script).

#### LocalNet FIO History Node
For the purposes of confirming end-to-end connectivity please refer to the [LocalNet Deployment Guide - Start FIO Nodeos](https://github.com/fioprotocol/fio.relic/blob/develop/docs/localnet-standup.md#start-fio-nodeos) and [LocalNet Deployment Guide - Start FIO Nodeos History Node](https://github.com/fioprotocol/fio.relic/blob/develop/docs/localnet-standup.md#start-fio-nodoes-state-history-nodeos)

## PostgreSQL
The installation and configuration of PostgreSQL, the persistance layer of the FIO.Relic system, will occur in two parts due to the customization that must be done to configure connectivity including security for the target environment but also installation of the FIO.Relic schema.

###  Installation
PostgreSQL provides packages for Ubuntu and may be installed manually, however, for an automated install, the [PostgreSQL install script](https://github.com/fioprotocol/fio.relic/blob/develop/scripts/install-pgsql.sh) will be used. Follow these steps to install PostgreSQL as well as the FIO.Relic schema;
1. Clone the FIO.Relic repo
2. Change directory into the FIO.Relic repo
3. Execute the script, ./scripts/install-pgsql.sh, as the root user and follow the prompts

```shell
git clone https://github.com/fioprotocol/fio.relic.git
cd fio.relic
sudo ./scripts/install-pgsql.sh
```

The PostgreSQL install script will update the OS, install PostgreSQL and any required PostgreSQL package dependencies. Note that the PostgreSQL installation includes configuration and startup of the PostgreSQL database.

For further support refer to the PostgreSQL Ubuntu documentation located [here](https://www.postgresql.org/download/linux/ubuntu).

### FIO.Relic Database
To create the FIO.Relic database, schema and user execute the script _create_schema.sh_. This will include creating the necessary tables, stored procedures as well as the user allowing FIO.Chronicle to connect, and persist data.
```shell
sudo ./scripts/create_schema.sh
```

Verification of the relicdb may be done by executing the 
The configuration of PostgresSQL including connection handling, authentication, database administration is outlined in the PostgresQL configuration document [here](https://github.com/fioprotocol/fio.relic/blob/develop/docs/postgres-config.md).

For further insight into the PostgreSQL database see [Getting Started](https://www.postgresql.org/docs/16/tutorial-start.html).

## FIO.Chronicle
The FIO.Chronicle state history processor ingests state history data from a FIO State History Server, via the state history plugin, processes it and persists into the FIO.Relic database. To get started with the FIO.Chronicle application do the following;
1. Clone the FIO.Chronicle repo
2. Change directory into the FIO.Chronicle repo

```shell
git clone --recursive https://github.com/fioprotocol/fio.chronicle.git
cd fio.chronicle
```

### Build, Install and Configure
The build and installation of FIO.Chronicle is straightforward and can be performed in just a few steps, using the default configuration. As noted above, there are several configuration items that may be customized, including;
* FIO Nodeos State History Host and Port
* PostgreSQL Host and Port

To build FIO.Chronicle, execute the following;
```shell
./scripts/build.sh /opt
```

To install FIO.Chronicle, along with its default configuration, execute the following;
```shell
./scripts/install.sh
```

Note that the default configuration will access a local state history node and output data to a local PostgreSQL server. For example, the installed configuration, as described in `/opt/fio-chronicle/config/config.ini`, is as follows;
```shell
host = 127.0.0.1
port = 8080
mode = scan
plugin = exp_relic_plugin
exp-relic-host = 127.0.0.1
exp-relic-port = 5432
exp-relic-username = chronicle_user
exp-relic-password = password123!
```

Update the above attributes based on your environment. For more information, i.e. build options, advanced configuration options, etc. refer to the FIO.Chronicle [README](https://github.com/fioprotocol/fio.chronicle/blob/develop/README.md#build-and-install-instructions) as well as the [advanced configuration](https://github.com/fioprotocol/fio.chronicle/blob/develop/docs/advanced-config.md).

### Start FIO.Chronicle
The following command has only an end block specified and, therefore, FIO.Chronicle will process blocks from block 1 up to and including the end block. To limit processing, specify both a start block, `--start-block` and and end block, `--end-block`. 

Start the fio-chronicle-receiver
```shell
/opt/fio-chronicle/chronicle-receiver --config-dir=/opt/fio-chronicle/config --data-dir=/opt/fio-chronicle/data --end-block=400000000
```

Refer to the [advanced configuration](https://github.com/fioprotocol/fio.chronicle/blob/develop/docs/advanced-config.md) for more options.

### Verify processing
Verification that FIO.Chronicle is processing blocks may be done in two ways; reviewing the fio.chronicle log, currently as standard out from execution of the above command, as well as through inspection of the database.

Each method is described below;
FIO.Chronicle log: output should resemble log below, where block number is increasing over time.

```
info  2025-02-06T23:34:01.094 chronicle receiver_plugin.cpp:640       request_blocks       ] Start block: 1
...
info  2025-02-06T23:34:02.178 chronicle exp_relic_plugin.cpp:1985     push_msg             ] exp_relic_plugin queue_size=1
...
info  2025-02-05T03:32:49.345 chronicle receiver_plugin.cpp:863       receive_result       ] block=10000; irreversible=306839321; dbmem_free=99; received_blocks=10000
```

**FIO.Relic database**
Using the exp-relic-username and exp-relic-password from the config above, execute the following commands
```shell
psql -d relicdb -U chronicle_user
Password for user chronicle_user:
```

To get a listing of all tables, execute the command;
```shell
relicdb=> \dt
                  List of relations
 Schema |        Name        | Type  |     Owner
--------+--------------------+-------+----------------
 public | accountactivities  | table | chronicle_user
 public | accounts           | table | chronicle_user
 public | accountsaudit      | table | chronicle_user
 public | blocks             | table | chronicle_user
 public | domainactivities   | table | chronicle_user
 public | domains            | table | chronicle_user
...
 public | transactions       | table | chronicle_user
(22 rows)
```

Other commands, to verify the Relic DB schema, are;
* To get a listing of all stored procedures, execute the command; `\df` (Total: 50)
* To get a listing of all stored procedures, execute the command; `\ds` (Total: 21)

To get a count of the number of transactions processed, execute the following command (and repeat as necessary);
```shell
relicdb=> select count(*) from transactions;
 count
-------
 13051
(1 row)
```
