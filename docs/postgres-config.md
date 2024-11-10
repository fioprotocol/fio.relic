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

