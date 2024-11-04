# FIO.Relic Deployment Guide

The FIO.Relic Deployment Guide outlines the steps necessary to build, install and deploy a FIO Nodeos node, the FIO.Chronicle history middleware application, and PostgreSQL.

While the FIO.Chronicle historical data processor is still under development, this guide contains the latest information to stand up each component of the system.

## Relevant Information
The Foundation for Interwallet Operability (FIO) or, in short, the FIO Protocol, is an open-source project based on EOSIO 1.8+.

* For information about the FIO Protocol, visit [FIO](https://fio.net).
* For information on the FIO Chain, API, and SDKs, including detailed clone, build and deploy instructions, visit [FIO Protocol Developer Hub](https://dev.fio.net).

## FIO Nodeos

The FIO Nodeos source code, and build instructions may be found [here](https://github.com/fioprotocol/fio).

A FIO Nodeos [node](https://dev.fio.net/docs/chain-node) that will used in this deployment is one that is installed with a full history archive and is configured to provide historical data via the history plugin, and the history api plugin. The aforemention link contains the installation instructions using official FIO [packages](https://dev.fio.net/docs/install-using-packages), set chain state, and have full historical block data. To expedite the FIO Nodeos installation the [install script](https://dev.fio.net/docs/install-script) will used.

## FIO.Chronicle

See https://github.com/fioprotocol/fio.chronicle/blob/develop/docs/install-local.md for the deployment of the FIO.Chronicle history processor.

## PostgreSQL
For step by step instructions to install, configure and run PostgreSQL, see the PostgreSQL stand up document [here](https://github.com/fioprotocol/fio.relic/blob/develop/docs/postgres-standup.md). For an automated install, see the [PostgreSQL install script](https://github.com/fioprotocol/fio.relic/blob/develop/scripts/install-pgsql.sh).
