#!/usr/bin/env bash

echo
#if [[ "$EUID" -ne 0 ]]; then
#  echo "ERROR: Script must be run as root! Use sudo command as follows; sudo ./<script name>"
#  echo
#  exit 1
#fi

# tables: createrelictables.sql
# stored procs: createrelicstoredprocedures.sql

# 1) create the user account, user: chronicle_user 
# 2) create the relicdb, db: relicdb
# 3) grant access to the account
# 4) create relic db tables: createrelictables.sql
# 5) create relic db stored procedures: createrelicstoredprocedures.sql

SCRIPT_DIR=$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)

. ${SCRIPT_DIR}/utils.sh

# Configure PostgreSQL DB
# Refer to https://computingforgeeks.com/install-and-configure-postgresql-on-ubuntu/
# Remote connections
# - sudo vim /etc/postgresql/16/main/postgresql.conf
# - edit listen_addresses as follows (default: localhost); listen_addresses = '*'
# Authentication 
# - Peer (password-based): sudo sed -i '/^host/s/ident/md5/' /etc/postgresql/16/main/pg_hba.conf
# - Trust (role -based): sudo sed -i '/^local/s/peer/trust/' /etc/postgresql/16/main/pg_hba.conf
# Remote Access
# sudo vim /etc/postgresql/16/main/pg_hba.conf
# - See IPV4 and IPV4 local connection section in url above for options
# Connect to dB
# - sudo -u postgres psql
# - sudo -i -u postgres && psql
# - psql 'postgres://<username>:<password>@<host>:<port>/<db>?sslmode=disable'
# Default Postgres User password: ALTER USER postgres PASSWORD 'Str0ngP@ssw0rd';
# Admin User: CREATE ROLE admin WITH LOGIN SUPERUSER CREATEDB CREATEROLE PASSWORD 'Passw0rd';
# Create database: CREATE DATABASE sampleDB;
# Create DB User:
# - CREATE USER demo_user with encrypted password 'PassW0rd';
# - GRANT ALL PRIVILEGES ON DATABASE sampleDB to demo_user;
#
# wrappers: createuser (createuser --help), dropuser (dropuser --help)
# example: createuser -h localhost -p 5432 -U postgres -w
echo "Creating Relic Database..."
echo
sudo -u postgres psql -c "CREATE DATABASE relicdb;"

echo
echo "Creating Relic DB schema..."
echo
sudo -u postgres psql -U postgres -d relicdb -a -f ${SCRIPT_DIR}/sql/CreateRelicTables.sql
sudo -u postgres psql -U postgres -d relicdb -a -f ${SCRIPT_DIR}/sql/CreateRelicStoredProcedures.sql

echo
echo "Creating Relic DB User..."
echo
sudo -u postgres psql -c "CREATE USER chronicle_user;"
sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE relicdb to chronicle_user;"

echo
echo "Configuring Relic DB User Access"
# Backup pg_hba.conf file b4 modifying
if [[ ! -e /etc/postgresql/16/main/pg_hba.conf.orig ]]; then
  sudo cp /etc/postgresql/16/main/pg_hba.conf /etc/postgresql/16/main/pg_hba.conf.orig
fi
# Update pg_hba.conf file to trust chronicle_user
if ! sudo grep -q chronicle_user /etc/postgresql/16/main/pg_hba.conf; then
  sudo sed -i '/# "local" is for Unix domain socket connections only/a local   all             chronicle_user                          trust' /etc/postgresql/16/main/pg_hba.conf
fi

# Restart PostgreSQL to enable changes
sudo systemctl restart postgresql
