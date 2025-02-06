#!/usr/bin/env bash

echo
#if [[ "$EUID" -ne 0 ]]; then
#  echo "ERROR: Script must be run as root! Use sudo command as follows; sudo ./<script name>"
#  echo
#  exit 1
#fi

#echo "WARNING: This script runs commands as the 'postgres' user using sudo to update the database'
#pause

groups $(id -un) | grep sudo >/dev/null
if [[ $? -ne 0 ]]; then
  echo "ERROR: User $(id -un) does NOT have sudo privilege! sudo privilege is required to run this script. Exiting..."
  echo
  exit 1
fi

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

echo
echo "Verifying that the PostgreSQL database server is installed and running..."
dpkg -l | grep postgres >/dev/null
if [[ $? -ne 0 ]]; then
  echo
  echo "The PostgreSQL database server does not appear to be installed!"
  echo
  echo "PostgreSQL 16 may be installed using the script, ./scripts/install-pgsql.sh. Once,"
  echo "complete re-execute this script."
  echo
  exit 1
fi

systemctl | grep running | grep -q postgresql
if [[ $? -ne 0 ]]; then
  echo
  echo "The PostgreSQL database server does not appear to be running!"
  echo
  echo "Start the server using the command, 'systemctl start postgresql', and re-execute this script."
  echo
  exit 1
fi

echo
echo "Creating the FIO.Relic Database..."
echo
sudo -u postgres psql -c "CREATE DATABASE relicdb;"

echo
echo "Creating FIO.Relic DB User..."
echo
sudo -u postgres psql -c "CREATE USER chronicle_user;"

echo
echo "Granting privileges to FIO.Relic DB User..."
echo
sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE relicdb to chronicle_user;"
sudo -u postgres psql -d relicdb -c "GRANT ALL ON SCHEMA public TO chronicle_user;"

echo
echo "Configuring initial FIO.Relic DB User Access"
# Backup pg_hba.conf file b4 modifying
if [[ ! -e /etc/postgresql/16/main/pg_hba.conf.orig ]]; then
  sudo cp /etc/postgresql/16/main/pg_hba.conf /etc/postgresql/16/main/pg_hba.conf.orig
fi
# Update pg_hba.conf file to trust chronicle_user
if ! sudo grep -q chronicle_user /etc/postgresql/16/main/pg_hba.conf; then
  sudo sed -i '/# "local" is for Unix domain socket connections only/a local   all             chronicle_user                          trust' /etc/postgresql/16/main/pg_hba.conf
fi

# Restart PostgreSQL to enable changes
echo
echo "Restarting PostgreSQL to reload any configuration changes..."
sudo systemctl restart postgresql
echo

echo
echo "Creating FIO.Relic DB schema, including tables and stored procedures..."
echo
sudo -u postgres psql -U chronicle_user -d relicdb -a -f ${SCRIPT_DIR}/sql/CreateRelicTables.sql
sudo -u postgres psql -U chronicle_user -d relicdb -a -f ${SCRIPT_DIR}/sql/CreateRelicStoredProcedures.sql

echo
echo "Setting FIO.Relic DB User password..."
echo
sudo -u postgres psql -c "ALTER USER chronicle_user WITH PASSWORD 'password123!';"

echo
echo "Configuring final FIO.Relic DB User Access"
sudo sed -i '/local   all             chronicle_user                          trust/c\local   all             chronicle_user                          md5' /etc/postgresql/16/main/pg_hba.conf

# Restart PostgreSQL to enable changes
echo
echo "Restarting PostgreSQL to reload any configuration changes..."
sudo systemctl restart postgresql
echo