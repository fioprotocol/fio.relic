# FIO.Relic Deployment Guide

The FIO.Relic Deployment Guide outlines the steps necessary to build, install and deploy a FIO Nodeos node, the FIO.Chronicle history middleware application, and PostgreSQL.

While the FIO.Chronicle historical data processor is still under development, this guide contains the latest information to stand up each component of the system.

## Relevant Information
The Foundation for Interwallet Operability (FIO) or, in short, the FIO Protocol, is an open-source project based on EOSIO 1.8+.

* For information about the FIO Protocol, visit [FIO](https://fio.net).
* For information on the FIO Chain, API, and SDKs, including detailed clone, build and deploy instructions, visit [FIO Protocol Developer Hub](https://dev.fio.net).

## LocalNet Dev/Test
For a local dev/test environment of FIO Nodeos, and Fio.Chronicle, see the [LocalNet Deployment Guide](https://github.com/fioprotocol/fio.relic/blob/develop/docs/localnet-standup.md).

## PostgreSQL, FIO.Chronicle and FIO Nodeos Deployment Overview
The FIO.Relic ecosystem is comprised of PostgreSQL, FIO.Chronicle and FIO Nodeos Blockchain. While this guide includes documentation on deploying a FIO Blockchain Node, the assumption is that a FIO Blockchain node is already stood up, configured and running. Regardless, there are three components to a FIO.Relic System;
1) PostgreSQL RDMS
2) FIO.Chronicle
3) FIO Nodeos Blockchain node

## PostgreSQL
The installation and configuration of PostgreSQL, the persistance layer of the FIO.Relic system, must occur in two parts due to the customization that should be done to provide connectivity as well as security for the target environment.

###  Installation
PostgreSQL provides packages for Ubuntu and may be installed manually, however, for an automated install, see the [PostgreSQL install script](https://github.com/fioprotocol/fio.relic/blob/develop/scripts/install-pgsql.sh).

For further support refer to the PostgreSQL Ubuntu documentation located [here](https://www.postgresql.org/download/linux/ubuntu).

### Configuration
TBD: Incorporate connection config, schema creation and db user access from ed's work

The configuration of PostgresSQ: including connection handling, authentication, database administration is outlined in the PostgresQL configuration [here](https://github.com/fioprotocol/fio.relic/blob/develop/docs/postgres-config.md).

For further insight into the PostgreSQL database see [Getting Started](https://www.postgresql.org/docs/16/tutorial-start.html).

## FIO.Chronicle
### Build and Install
The build and installation of FIO.Chronicle is straightforward and can be performed in just a few steps. As with PostgreSQL there are several configuration items to identify before doing so. The include;
* FIO Nodeos history port
* this
* that

Refer to the FIO.Chronicle [README](https://github.com/fioprotocol/fio.chronicle/blob/feature/bd-4660-buildinstall-updates/README.md#build-and-install-instructions) for build and installation instructions

### Start FIO.Chronicle
Start the fio-chronicle-receiver
```shell
/opt/fio-chronicle/chronicle-receiver --config-dir=/opt/fio-chronicle/config --data-dir=/opt/fio-chronicle/data --end-block=846511
```

## FIO Nodeos

The FIO Nodeos source code, and build instructions may be found [here](https://github.com/fioprotocol/fio).

A FIO Nodeos [node](https://dev.fio.net/docs/chain-node) that will used in this deployment is one that is installed with a full history archive and is configured to provide historical data via the history plugin, and the history api plugin. The FIO Nodeos installation is straightforward, however, multiple steps must be followed including;
* [FIO Package Installation](https://dev.fio.net/docs/install-using-packages)
* [FIO Nodeos Configuration](https://dev.fio.net/docs/configure-and-run-your-node)
* [Replaying blockchain blocks](https://dev.fio.net/docs/nodeos-replay)

To expedite the FIO Nodeos installation, including blockchain configuration and history playback, use the [install script](https://dev.fio.net/docs/install-script).
