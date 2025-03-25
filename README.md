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

### Tech Stack and Architecture Diagram
See the [FIO.Relic Tech Stack](https://github.com/fioprotocol/fio.relic/blob/develop/docs/tech-stack.md) document for the requisite hardware and software needed to install and run the FIO.Relic ecosystem

### Comprehensive Deployment Guide: How to deploy FIO Nodeos, and FIO.Relic
See the [FIO.Relic Deployment Guide](https://github.com/fioprotocol/fio.relic/blob/develop/docs/deployment.md) document for the deployment outline of a FIO History Node, FIO.Relic including a RDMS as well as the state history processor.

## Clone the repository
To clone the FIO.Relic repository, execute the command;
```shell
git clone https://github.com/fioprotocol/fio.relic.git
```

see [Cloning a repository](https://docs.github.com/en/repositories/creating-and-managing-repositories/cloning-a-repository) for more information.

## FIO.Relic Database Server
The installation and configuration of the FIO.Relic PostgreSQL database will occur in two parts;
1. Installation and configuration of the database server as a system application
2. Creation and configuration of the database schema including tables, and functions, as well as connectivity and user access

### Installation and System Configuration
PostgreSQL provides packages for Ubuntu and may be installed manually, however, for an automated install, the [PostgreSQL install script](https://github.com/fioprotocol/fio.relic/blob/develop/scripts/install_pgsql.sh) will be used. To install PostgreSQL, execute the following command;
```shell
sudo ./scripts/install_pgsql.sh
```

The PostgreSQL install script will update the OS, install PostgreSQL and any required PostgreSQL package dependencies. Note that the PostgreSQL installation includes configuration and startup of the PostgreSQL database.

For further support refer to the PostgreSQL Ubuntu documentation located [here](https://www.postgresql.org/download/linux/ubuntu).

### Database Creation and Configuration
To create the FIO.Relic database, including tables, functions as well as configure the necessary user access and connectivity, execute the following command;
```shell
sudo ./scripts/create_schema.sh
```

Verification of the relicdb may be done by executing the following commands;
```shell
psql -d relicdb -U chronicle_user
Password for user chronicle_user:
```
Note that the default password is **password123!**

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

The configuration of PostgresSQL including connection handling, authentication, database administration is outlined in the PostgresQL configuration document [here](https://github.com/fioprotocol/fio.relic/blob/develop/docs/postgres-config.md).

For further insight into the PostgreSQL database see [Getting Started](https://www.postgresql.org/docs/16/tutorial-start.html).

### Data Capture: Export and Import
Two scripts are provided to handle the export and import of data from/to the FIO.Relic PostgreSQL database, _export_db.sh_ and _import_db.sh_.

To export the FIO.Relic database schema including tables, functions, users and data execute the script, _export_db.sh_, passing in a file name, including path. For example, to export the data to the file _relicdb_snapshot.tar.gz_, execute the command; 
```shell
sudo ./scripts/export_db.sh relicdb_snapshot.tar.gz
```

Note: Capture of the FIO.Relic database must coincide with capture of the FIO.Relic state history processor state, therefore, it is recommended to shut down the state history processor, export as instructed above and capture the FIO.Relic state history processor state. See [FIO.Chronicle Data Capture](https://github.com/fioprotocol/fio.chronicle#data-capture:-export-and-import)

To import the FIO.Relic database schema including tables, functions, users and data execute the script, _import_db.sh_, passing in a file name, including path. For example, to import the data previously exported to the file _relicdb_snapshot.tar.gz_, execute the command; 
```shell
sudo ./scripts/import_db.sh relicdb_snapshot.tar.gz
```

WARNING: ***The import script will clear any and all data as well as tables, functions and users!***


## FIO.Relic State History Processor
The FIO.Relic state history processor, FIO.Chronicle, utilizes the FIO.Relic database server and schema as the peristence layer for historical data for the FIO Nodeos blockchain. See [FIO.Chronicle](https://github.com/fioprotocol/fio.chronicle) to stand up the FIO.Relic State History Processor.
