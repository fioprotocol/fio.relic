# FIO.Relic

## Overview
FIO.Relic is the state history solution for the FIO Blockchain incorporating the ingest of state history data, as well as the parsing and storage of that data into a relational database for asynchronous consumption by 3rd party applications.

The FIO.Relic ecosystem is comprised of;
* FIO Nodeos node providing state history via the state-history plugin
* The FIO.Relic state history plugin and FIO.Chronicle backend state history processor which ingests and processes state historical data from FIO.Nodeos
* A PostgreSQL database as the state history data store

FIO.Relic is based on the opensource project [EOS-Chronicle](https://github.com/EOSChronicleProject/eos-chronicle), a middleware app that consumes history data available via the state history plugin of an Antelope (EOSIO) blockchain and provides downstream consumers json formatted data.

# FIO Protocol
The Foundation for Interwallet Operability (FIO) or, in short, the FIO Protocol, is an open-source project based on EOSIO 1.8+.

* For information on FIO Protocol, visit [FIO](https://fio.net).
* For information on the FIO Chain, API, and SDKs, including detailed clone, build and deploy instructions, visit [FIO Protocol Developer Hub](https://dev.fio.net).
* To get updates on the development roadmap, visit [FIO Improvement Proposals](https://github.com/fioprotocol/fips). Anyone is welcome and encouraged to contribute.
* To contribute, please review [Contributing to FIO](https://dev.fio.net/docs/contributing-to-fio)
* To join the community, visit [Discord](https://discord.com/invite/pHBmJCc)

## Licenses
[FIO License](https://github.com/fioprotocol/fio/blob/master/LICENSE)

[FIO.Relic License](https://github.com/fioprotocol/fio.chronicle/blob/develop/LICENSE.txt)

### FIO Blockchain
Refer to the [FIO Protocol Developer Hub](https://dev.fio.net/docs/chain-node) documentation to install or attach to an enterprise fio.nodeos blockchain. See the [FIO Readme](https://github.com/fioprotocol/fio/blob/master/README.md) for instructions on how to build and install the FIO Protocol fio.nodeos block chain locally. 

### FIO.Relic

#### Tech Stack and Architecture Diagram
See the [FIO.Relic Tech Stack](https://github.com/fioprotocol/fio.relic/blob/develop/docs/tech-stack.md) document for the requisite hardware and software needed to install and run the FIO.Relic ecosystem

#### Deployment Guide
See the [FIO.Relic Deployment Guide](https://github.com/fioprotocol/fio.relic/blob/develop/docs/deployment.md) document for the deployment outline of a FIO History Node, and FIO.Chronicle which includes the RDMS, PostgreSQL.
