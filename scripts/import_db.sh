#!/usr/bin/env bash

# Debug
#set -x

# Load utilities
SCRIPT_DIR=$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)
. ${SCRIPT_DIR}/utils.sh

if [[ "$EUID" -ne 0 ]]; then
  echo && echo "ERROR: Script must be run as root! Use sudo command as follows; sudo ./<script name>"
  echo
  exit 1
fi

# A database backup may be created by executing the following command;
#   `sudo -u postgres pg_dump relicdb > relicdb-bkup.sql`
# To restore a backup, the following are needed;
#   A database to restore the dump file into, i.e. relicdb (note while the db must exist it needn't be the same name)

# Check that a db dump file was provided
if [[ $# -eq 0 || -z "$1" ]]; then
  echo
  echo "ERROR: No argument provided for FIO.Relic DB dump file (from pg_dump)!" && echo
  echo "Usage:"
  echo "./scripts/import_db.sh <dump file>"
  echo
  exit -1
fi

if [[ ! -r "$1" ]]; then
  echo
  echo "ERROR: $1 is NOT valid; Check permissions and retry!" && echo
  echo "Usage:"
  echo "./scripts/import_db.sh <dump file>"
  echo
  exit -1
fi
dumpfile="${1}"

# Check that the database exists
echo && echo "Checking Relic DB User for DB restore..."
if ! sudo -u postgres psql -lqt | cut -d \| -f 1 | grep -qw relicdb; then
  echo && echo "ERROR: It does not appear that the FIO.Relic DB, 'relicdb', exists! Exiting..."
  exit 1
fi

# Check that the database user exists
echo && echo "Checking Relic DB User for DB restore..."
if ! sudo -u postgres psql -tXAc "SELECT 1 FROM pg_roles WHERE rolname='chronicle_user'" | grep -q 1; then
  echo && echo "ERROR: It does not appear that the FIO.Relic DB user, 'chronicle_user', exists! Exiting..."
  exit 1
fi

# Steps
# 1) drop relic db stored procedures: droprelicstoredprocedures.sql
# 2) drop relic db tables: droprelictables.sql
# 3) import pg_dump'ed file (Note update to export to compress file, -F c, and then use pg_restore
echo && echo "WARNING: A DB Restore will clear all existing tables, functions as well as any data!"
echo
if ! yes_or_no "Do you want to proceed"; then
  echo && echo "Exiting restore of FIO.Relic DB!";
  echo
  exit 1
fi

echo && echo "Dropping Relic DB schema..."
sudo -u postgres psql -d relicdb -a -f ${SCRIPT_DIR}/sql/DropRelicStoredProcedures.sql
sudo -u postgres psql -d relicdb -a -f ${SCRIPT_DIR}/sql/DropRelicTables.sql

echo && echo "Importing FIO.Relic DB schema, including tables, stored procedures and data..."
echo
sudo -u postgres psql -d relicdb -f ${dumpfile}

# Restart PostgreSQL to enable changes
echo && echo "Restarting PostgreSQL to load configuration changes..."
systemctl restart postgresql

# And done
echo && echo "Done!"