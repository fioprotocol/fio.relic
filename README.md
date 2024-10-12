# FIO.Relic

## Overview
fio-relic is intended to be the middleware between a fio-nodeos node providing state history via the state-history plugin and a relational database for utilization by consumers such as the fio dashboard or a block explorer. It is based on the opensource product EOS-Chronicle, a middleware app that consumes history data available via the state history plugin of an Antelope (EOSIO) blockchain and provides downstream consumers json formatted data via plugins. Fio-relic, however, is an integrated application incorporating the ingest of state history data, as well as the parsing and storage of that data into a relational database for asynchronous consumption by 3rd party applications.

# FIO Protocol
The Foundation for Interwallet Operability (FIO) or, in short, the FIO Protocol, is an open-source project based on EOSIO 1.8+.

* For information on FIO Protocol, visit [FIO](https://fio.net).
* For information on the FIO Chain, API, and SDKs, including detailed clone, build and deploy instructions, visit [FIO Protocol Developer Hub](https://dev.fio.net).
* To get updates on the development roadmap, visit [FIO Improvement Proposals](https://github.com/fioprotocol/fips). Anyone is welcome and encouraged to contribute.
* To contribute, please review [Contributing to FIO](CONTRIBUTING.md)
* To join the community, visit [Discord](https://discord.com/invite/pHBmJCc)

## Licenses
[FIO License](https://github.com/fioprotocol/fio/blob/master/LICENSE)

[FIO.Relic License](https://github.com/fioprotocol/fio.chronicle/blob/develop/LICENSE.txt)

### Local FIO Blockchain
See [FIO](https://github.com/fioprotocol/fio/blob/master/README.md) for instructions on how to setup a chain

### FIO.Relic
See the [local standup doc](https://github.com/fioprotocol/fio.relic/blob/develop/docs/localnet-standup.md)
