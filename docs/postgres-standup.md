Install and configuration of PostgreSQL

This procedure described the installation and configuration of PostgreSQL 16 on Ubuntu 22.04|20.04|18.04

Installation
First, update your OS to get the latest apts for your version

`sudo apt update; sudo apt upgrade;`

Install required packages (substitute nano for vim below if desired);

`sudo apt install gnupg2 wget vim -y`

Add the postgres repository

`sudo sh -c 'echo "deb https://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" > /etc/apt/sources.list.d/pgdg.list'`

Set the signing key for the postgres repository

`curl -fsSL https://www.postgresql.org/media/keys/ACCC4CF8.asc|sudo gpg --dearmor -o /etc/apt/trusted.gpg.d/postgresql.gpg`

Update package list (again)

`sudo apt update -y`

To install PostgreSQL v16.x (Note install any dependencies it requires)

`sudo apt install postgresql-16 postgresql-contrib-16`

To install the latest version of PostgreSQL (PostgreSQL 16 is the latest LTS version), execute the following

`sudo apt-get -y install postgresql postgresql-contrib`

Verify install was successful

`psql --version` OR `sudo -u postgres psql -c "SELECT version();"`

Start and enable the service

`sudo systemctl start postgresql`
`sudo systemctl enable postgresql`

Check service status

`systemctl status postgresql`

Configuration

Allow remote connections

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

Allow password authentication. The default is ident authentication and will be tied to the ubuntu user, which is not desired right now. This is a one liner and will perform the necessary modification.

`sudo sed -i '/^host/s/ident/md5/' /etc/postgresql/16/main/pg_hba.conf`

Also change the identification method from peer to trust with the command;

`sudo sed -i '/^local/s/peer/trust/' /etc/postgresql/16/main/pg_hba.conf`

Allow PostgreSQL to be accessed remotely, either edit manually or execute the following command;
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

Restart the PostgreSQL service; `sudo systemctl restart postgresql`

If there is a firewall or security measures that would prevent connection to port 5432, update it to allow it, i.e. the ubuntu's netfilter firewall, ufw;

`sudo ufw allow 5432/tcp`

Connecting to PostgreSQL

```shell
sudo -u postgres psql
#-OR-
sudo -i -u postgres
psql
```

Remotely using the psql with format as follows; psql 'postgres://<username>:<password>@<host>:<port>/<db>?sslmode=disable'

`psql 'postgres://postgres@192.168.200.51:5432/postgres?sslmode=disable'`

By default the admin user account is not password protected. Let's do that;

`ALTER USER postgres PASSWORD 'Str0ngP@ssw0rd';`

Create superusers by creating an admin role using the command;

`CREATE ROLE admin WITH LOGIN SUPERUSER CREATEDB CREATEROLE PASSWORD 'Passw0rd';`

List roles by executing the command (from the postgres prompt); `\du`

To create a database, use the command:

`CREATE DATABASE sampleDB;`

To create a user with permissions to manage the database;

```shell
CREATE USER demo_user with encrypted password 'PassW0rd';
GRANT ALL PRIVILEGES ON DATABASE sampleDB to demo_user;
```