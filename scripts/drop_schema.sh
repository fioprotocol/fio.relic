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

# tables: droprelictables.sql.
# stored procs: dropstoredprocedures.sql.

# 1) drop relic db stored procedures: droprelicstoredprocedures.sql
# 2) drop relic db tables: droprelictables.sql
# 3) drop the relicdb, db: relicdb
# 4) delete the user account, user: chronicle_user 

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
echo "Dropping Relic DB schema..."
echo
#sudo -u postgres psql -U chronicle_user -d relicdb -a -f ${SCRIPT_DIR}/sql/DropRelicStoredProcedures.sql
sudo -u postgres psql -d relicdb -a -f ${SCRIPT_DIR}/sql/DropRelicStoredProcedures.sql
#sudo -u postgres psql -U chronicle_user -d relicdb -a -f ${SCRIPT_DIR}/sql/DropRelicTables.sql
sudo -u postgres psql -d relicdb -a -f ${SCRIPT_DIR}/sql/DropRelicTables.sql

echo
if yes_or_no "Drop Relic DB User, 'chronicle_user'"; then
  echo
  echo "Dropping Relic DB User..."
  echo
  sudo -u postgres psql -d relicdb -c "REVOKE ALL PRIVILEGES ON DATABASE relicdb from chronicle_user;"
  sudo -u postgres psql -d relicdb -c "REVOKE ALL ON SCHEMA public FROM chronicle_user;"
  sudo -u postgres psql -c "DROP USER IF EXISTS chronicle_user;"

  echo
  echo "Un-configuring Relic DB User Access"
  # Backup pg_hba.conf file b4 modifying
  if [[ -e /etc/postgresql/16/main/pg_hba.conf.orig ]]; then
    sudo mv /etc/postgresql/16/main/pg_hba.conf.orig /etc/postgresql/16/main/pg_hba.conf
    sudo chown postgres:postgres /etc/postgresql/16/main/pg_hba.conf
  else
    # Update pg_hba.conf file to chronicle_user
    if sudo grep -q chronicle_user /etc/postgresql/16/main/pg_hba.conf; then
      sudo sed -i '/local   all             chronicle_user/d' /etc/postgresql/16/main/pg_hba.conf
    fi
  fi
fi

echo
if yes_or_no "Drop Relic DB"; then
  echo
  echo "Dropping Relic Database..."
  echo
  sudo -u postgres psql -c "DROP DATABASE IF EXISTS relicdb;"
fi

# Restart PostgreSQL to enable changes
echo
sudo systemctl restart postgresql
