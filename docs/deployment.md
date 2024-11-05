# FIO.Relic Deployment Guide

The FIO.Relic Deployment Guide outlines the steps necessary to build, install and deploy a FIO Nodeos node, the FIO.Chronicle history middleware application, and PostgreSQL.

While the FIO.Chronicle historical data processor is still under development, this guide contains the latest information to stand up each component of the system.

## Relevant Information
The Foundation for Interwallet Operability (FIO) or, in short, the FIO Protocol, is an open-source project based on EOSIO 1.8+.

* For information about the FIO Protocol, visit [FIO](https://fio.net).
* For information on the FIO Chain, API, and SDKs, including detailed clone, build and deploy instructions, visit [FIO Protocol Developer Hub](https://dev.fio.net).

## FIO Nodeos

The FIO Nodeos source code, and build instructions may be found [here](https://github.com/fioprotocol/fio).

A FIO Nodeos [node](https://dev.fio.net/docs/chain-node) that will used in this deployment is one that is installed with a full history archive and is configured to provide historical data via the history plugin, and the history api plugin. The FIO Nodeos installation is straightforward, however, multiple steps must be followed including;
* [FIO Package Installation](https://dev.fio.net/docs/install-using-packages)
* [FIO Nodeos Configuration](https://dev.fio.net/docs/configure-and-run-your-node)
* [Replaying blockchain blocks](https://dev.fio.net/docs/nodeos-replay)

To expedite the FIO Nodeos installation, including blockchain configuration and history playback, use the [install script](https://dev.fio.net/docs/install-script).

## FIO.Chronicle

### Local Dev/Test
For a local standalone dev/test environment see the [install doc](https://github.com/fioprotocol/fio.chronicle/blob/develop/docs/install-local.md)

### Production
In-work

Clone and build Fio.Chronicle
```shell
git clone --recursive https://github.com/fioprotocol/fio.chronicle.git && cd ./fio.chronicle && git checkout develop && git pull && mkdir -p build && sudo ./pinned_build/install_deps.sh && nice ./pinned_build/chronicle_pinned_build.sh /opt build $(nproc)
```

Install fio.chronicle to /opt
```shell
sudo mkdir -p /opt/fio-chronicle && sudo cp -r build/* /opt/fio-chronicle
```

Configure the fio-chronicle server
```shell
mkdir -p /opt/fio-chronicle/config /opt/fio-chronicle/data

cat >/opt/fio-chronicle/config/config.ini <<'EOT'
host = 127.0.0.1
port = 8080
mode = scan
plugin = exp_ws_plugin
exp-ws-host = 127.0.0.1
exp-ws-port = 8891
exp-ws-bin-header = false
EOT
```

Start the fio-chronicle-receiver
```shell
/opt/fio-chronicle/chronicle-receiver --config-dir=/opt/fio-chronicle/config --data-dir=/opt/fio-chronicle/data --end-block=846511
```

## PostgreSQL
The installation and configuration of PostgreSQL, the persistance layer of the FIO.Relic system, must occur in two parts due to the customization that should be done to provide connectivity as well as security for the target environment.

###  Installation
The PostgreSQL installation portion of a FIO.Relic deployment is described [here](https://www.postgresql.org/download/linux/ubuntu). For an automated install, see the [PostgreSQL install script](https://github.com/fioprotocol/fio.relic/blob/develop/scripts/install-pgsql.sh).

### Configuration

#### Allow remote connections

`sudo vim /etc/postgresql/16/main/postgresql.conf`

Change the listen_addresses attribute to * to allow any remote connection. Note: This is not a production level update, meaning that only specific clients should connect to postgres for data. If a specific remote connection is desired, one can use an actual IP address or a range, use an IP mask, i.e. '127.0.*'

The section of the configuration will look something like this;

```shell
#------------------------------------------------------------------------------
# CONNECTIONS AND AUTHENTICATION
#------------------------------------------------------------------------------

# - Connection Settings -

listen_addresses = '*'          # what IP address(es) to listen on;
                                # comma-separated list of addresses;
                                # defaults to 'localhost'; use '*' for all
```

#### Allow password authentication.

The default is ident authentication and will be tied to the ubuntu user, which is not desired right now. This is a one liner and will perform the necessary modification.

`sudo sed -i '/^host/s/ident/md5/' /etc/postgresql/16/main/pg_hba.conf`

Also change the identification method from peer to trust with the command;

`sudo sed -i '/^local/s/peer/trust/' /etc/postgresql/16/main/pg_hba.conf`

#### Allow PostgreSQL to be accessed remotely

Either manually edit the pg_hba.conf, located in /etc/postgresql/16/main/, or execute the following command;

`echo "host all all 0.0.0.0/0 md5" | sudo tee -a /etc/postgresql/16/main/pg_hba.conf`

The section of the configuration will look something like this;
```shell
# IPv4 local connections:
host    all             all             127.0.0.1/32        scram-sha-256
host    all             all             0.0.0.0/0           scram-sha-256
# IPv6 local connections:
host    all             all             ::1/128             scram-sha-256
host    all             all             0.0.0.0/0           md5
```

#### Restart the PostgreSQL service

`sudo systemctl restart postgresql`

#### Firewall Considerations

If there is a firewall or security measures that would prevent connection to port 5432, update it to allow it, i.e. the ubuntu's netfilter firewall, ufw;

`sudo ufw allow 5432/tcp`

#### Connect to the PostgreSQL Server

```shell
sudo -u postgres psql
#-OR-
sudo -i -u postgres
psql
```

Remotely using the psql with format as follows;
```shell
psql 'postgres://<username>:<password>@<host>:<port>/<db>?sslmode=disable'
```
For example;
```shell
psql 'postgres://postgres@192.168.200.51:5432/postgres?sslmode=disable'
```

#### Secure the Admin Account

By default the admin user account is not password protected. Let's do that;

`ALTER USER postgres PASSWORD 'Str0ngP@ssw0rd';`

#### Create a superuser

Create an admin role using the command;

`CREATE ROLE admin WITH LOGIN SUPERUSER CREATEDB CREATEROLE PASSWORD 'Passw0rd';`

List roles by executing the command (from the postgres prompt); `\du`

#### Create a database

Create a database using the command:

`CREATE DATABASE sampleDB;`

#### Create a user with permissions to manage the database;

```shell
CREATE USER demo_user with encrypted password 'PassW0rd';
GRANT ALL PRIVILEGES ON DATABASE sampleDB to demo_user;
```
