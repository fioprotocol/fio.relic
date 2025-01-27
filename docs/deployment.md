# FIO.Relic Deployment Guide

The FIO.Relic Deployment Guide outlines the steps necessary to build, install and deploy the FIO.Chronicle history middleware application, the FIO.Chronicle persistence layer, PostgreSQL, as well as configuration for connecting to a FIO Nodeos node publishing history data via its state history API.

While the FIO.Chronicle historical data processor is still under development, this guide contains the latest information to stand up each component of the system.

## PostgreSQL, FIO.Chronicle and FIO Nodeos Deployment Overview
The FIO.Relic ecosystem is comprised of PostgreSQL, FIO.Chronicle and FIO Nodeos Blockchain. While this guide includes documentation on deploying a FIO Blockchain Node, the assumption is that a FIO Blockchain node is already stood up, configured and running. Regardless, there are three components to a FIO.Relic System;
1) FIO Nodeos Blockchain node
2) FIO.Chronicle
3) PostgreSQL RDMS

Each component of the FIO.Relic ecosystem has configuration parameters for history data processing. For instance, a FIO Nodeos history node will process and provide history via the state history plugin, FIO.Chronicle will query state history (via the state history api), process it, then persist transformed data into its PostgreSQL database.

The following table describes the default connection parameters, however, for a production configuration please use this table as a template substituting the target environment values.

| Application | Parameter | Default Value | Target Env Value | Purpose |
|-|-|-|-|-|
| FIO Nodeos | state-history-endpoint | 0.0.0.0:8080 | | Listen IP Mask and Port |
| FIO.Chronicle | host | 127.0.0.1 | | Nodeos State History API Host |
| FIO.Chronicle | port | 8080 | | Nodeos State History API Port |
| PostgreSQL | PGHOST | 127.0.0.1 | | RDMS Host |
| PostgreSQL | PGPORT | 5432 | | RDMS Port |

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
./scripts/create_schema.sh
```

The configuration of PostgresSQL including connection handling, authentication, database administration is outlined in the PostgresQL configuration document [here](https://github.com/fioprotocol/fio.relic/blob/develop/docs/postgres-config.md).

For further insight into the PostgreSQL database see [Getting Started](https://www.postgresql.org/docs/16/tutorial-start.html).

## FIO.Chronicle
The FIO.Chronicle state history processor ingests state history data from a FIO State History Server, via the state history plugin, processes it and persists into the FIO.Relic database. To get started with the FIO.Chronicle application do the following;
1. Clone the FIO.Chronicle repo
2. Change directory into the FIO.Chronicle repo

```shell
git clone --recursive https://github.com/fioprotocol/fio.chronicle.git
cd fio.relic
```

### Build and Install
The build and installation of FIO.Chronicle is straightforward and can be performed in just a few steps, using the default configuration. As noted above, there are several configuration items that may be customized, including;
* FIO Nodeos State History Host and Port
* PostgreSQL Host and Port

For the default build and install of FIO.Chronicle accessing history locally with output locally, proceed with the following;
```shell
./scripts/build.sh /opt
./scripts/install.sh
```

Refer to the FIO.Chronicle [README](https://github.com/fioprotocol/fio.chronicle/blob/develop/README.md#build-and-install-instructions) for build and installation instructions as well as the advanced configuration [README](https://github.com/fioprotocol/fio.chronicle/blob/develop/docs/advanced-config.md).

### Start the FIO.Chronicle Web Socket Server (Test Only)
Refer to [LocalNet Deployment Guide - Start FIO.Chronicle Web Socket Server](https://github.com/fioprotocol/fio.relic/blob/develop/docs/localnet-standup.md#start-fio-chronicle-test-web-socket-server)

### Start FIO.Chronicle
In the following command both start block and an end block number will be specified to limit block processing. Note that a local blockchain will process block from block 1 in which case specifying only an end block would be appropriate.

Start the fio-chronicle-receiver
```shell
/opt/fio-chronicle/chronicle-receiver --config-dir=/opt/fio-chronicle/config --data-dir=/opt/fio-chronicle/data --start-block=292100000 --end-block=292111588
```

