# FIO.Relic Deployment Guide

The FIO.Relic Deployment Guide outlines the steps necessary to build, install and deploy the FIO.Chronicle history middleware application, the FIO.Chronicle persistence layer, PostgreSQL, as well as configuration for connecting to a FIO Nodeos node publishing history data via its state history API.

While the FIO.Chronicle historical data processor is still under development, this guide contains the latest information to stand up each component of the system.

## Relevant Information
The Foundation for Interwallet Operability (FIO) or, in short, the FIO Protocol, is an open-source project based on EOSIO 1.8+.

* For information about the FIO Protocol, visit [FIO](https://fio.net).
* For information on the FIO Chain, API, and SDKs, including detailed clone, build and deploy instructions, visit [FIO Protocol Developer Hub](https://dev.fio.net).

## LocalNet Dev/Test
For a local dev/test environment of FIO Nodeos, and Fio.Chronicle, see the [LocalNet Deployment Guide](https://github.com/fioprotocol/fio.relic/blob/develop/docs/localnet-standup.md).

## PostgreSQL, FIO.Chronicle and FIO Nodeos Deployment Overview
The FIO.Relic ecosystem is comprised of PostgreSQL, FIO.Chronicle and FIO Nodeos Blockchain. While this guide includes documentation on deploying a FIO Blockchain Node, the assumption is that a FIO Blockchain node is already stood up, configured and running. Regardless, there are three components to a FIO.Relic System;
1) FIO Nodeos Blockchain node
2) FIO.Chronicle
3) PostgreSQL RDMS

Each component of the FIO.Relic ecosystem has configuration parameters for history data processing. For instance, a FIO Nodeos history node will process and provide history via the state history plugins, FIO.Chronicle will pull data from the API plugin, process it and, via its connection to PostgreSQL, persist it.

The following table specifically outlines the default connection parameters, however, for a production configuration please use this table as a template for the actual target environment.

| Application | Parameter | Default Value | Target Env Value | Purpose |
|-|-|-|-|-|
| FIO Nodeos | state-history-endpoint | 0.0.0.0:8080 | | Listen IP Mask and Port |
| FIO.Chronicle | host | 127.0.0.1 | | Nodeos State History API Host |
| FIO.Chronicle | port | 8080 | | Nodeos State History API Port |
| PostgreSQL | PGHOST | 127.0.0.1 | | RDMS Host |
| PostgreSQL | PGPORT | 5432 | | RDMS Port |

## FIO Nodeos

The FIO Nodeos source code, and build instructions may be found [here](https://github.com/fioprotocol/fio).

A FIO Nodeos [node](https://dev.fio.net/docs/chain-node) that will used in this deployment is one that is installed with a full history archive and is configured to provide historical data via the state history plugin, and the history api plugin. The FIO Nodeos installation is straightforward, however, multiple steps must be followed including;
* [FIO Package Installation](https://dev.fio.net/docs/install-using-packages)
* [FIO Nodeos Configuration](https://dev.fio.net/docs/configure-and-run-your-node)
* [Replaying blockchain blocks](https://dev.fio.net/docs/nodeos-replay)

To expedite the FIO Nodeos installation, including blockchain configuration and history playback, use the [install script](https://dev.fio.net/docs/install-script).

For the purposes of confirming end-to-end connectivity please refer to the [LocalNet Deployment Guide - Start FIO Nodeos](https://github.com/fioprotocol/fio.relic/blob/develop/docs/localnet-standup.md#start-fio-nodeos) and [LocalNet Deployment Guide - Start FIO Nodeos History Node](https://github.com/fioprotocol/fio.relic/blob/develop/docs/localnet-standup.md#start-fio-nodoes-state-history-nodeos)

## PostgreSQL
The installation and configuration of PostgreSQL, the persistance layer of the FIO.Relic system, will occur in two parts due to the customization that should be done to provide not only the schema but any remote connectivity as well as security for the target environment.

###  Installation
PostgreSQL provides packages for Ubuntu and may be installed manually, however, for an automated install, see the [PostgreSQL install script](https://github.com/fioprotocol/fio.relic/blob/develop/scripts/install-pgsql.sh).

For further support refer to the PostgreSQL Ubuntu documentation located [here](https://www.postgresql.org/download/linux/ubuntu).

### Configuration
TBD: schema creation and db user access from ed's work

The configuration of PostgresSQL including connection handling, authentication, database administration is outlined in the PostgresQL configuration document [here](https://github.com/fioprotocol/fio.relic/blob/develop/docs/postgres-config.md).

For further insight into the PostgreSQL database see [Getting Started](https://www.postgresql.org/docs/16/tutorial-start.html).

## FIO.Chronicle
### Build and Install
The build and installation of FIO.Chronicle is straightforward and can be performed in just a few steps, using the default configuration. As noted above, there are several configuration items that may be customized, including;
* FIO Nodeos State History Host and Port
* PostgreSQL Host and Port

For the default build and install of FIO.Chronicle accessing history locally with output locally, proceed with the following;
```shell
cd <FIO.Chronicle Repo>

./scripts/build.sh /opt
./scripts/install.sh
```

Refer to the FIO.Chronicle [README](https://github.com/fioprotocol/fio.chronicle/blob/feature/bd-4660-buildinstall-updates/README.md#build-and-install-instructions) for build and installation instructions as well as the advanced configuration [README](https://github.com/fioprotocol/fio.chronicle/blob/feature/bd-4660-buildinstall-updates/docs/advanced-config.md).

### Start the FIO.Chronicle Web Socket Server (Test Only)
Refer to [LocalNet Deployment Guide - Start FIO.Chronicle Web Socket Server](https://github.com/fioprotocol/fio.relic/blob/develop/docs/localnet-standup.md#start-fio-chronicle-test-web-socket-server)

### Start FIO.Chronicle
In the following command both start block and an end block number will be specified to limit block processing. Note that a local blockchain will process block from block 1 in which case specifying only an end block would be appropriate.

Start the fio-chronicle-receiver
```shell
/opt/fio-chronicle/chronicle-receiver --config-dir=/opt/fio-chronicle/config --data-dir=/opt/fio-chronicle/data --start-block=292100000 --end-block=292111588
```

