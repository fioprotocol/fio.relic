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

echo && echo "Resetting the FIO.Relic Database (full drop and create of all tables, and functions). All data will be deleted during this process!"
pause
#echo "WARNING: This script runs commands as the 'postgres' user using sudo to update the database'
#pause

# tables: droprelictables.sql.
# stored procs: dropstoredprocedures.sql.

# 1) drop relic db stored procedures: droprelicstoredprocedures.sql
# 2) drop relic db tables: droprelictables.sql

# wrappers: createuser (createuser --help), dropuser (dropuser --help)
# example: createuser -h localhost -p 5432 -U postgres -w
echo && echo "Updating Relic DB User Access for DB reset..."
if ! sudo grep -q chronicle_user /etc/postgresql/16/main/pg_hba.conf; then
  echo && echo "ERROR: It does not appear that DB auth has been configured to authenticate the 'chronicle_user'! Exiting..."
  exit 1
fi

# Update chronicle_user to have 'trust' authentication
sed -i '/local[[:space:]]\+all[[:space:]]\+chronicle_user/d' /etc/postgresql/16/main/pg_hba.conf
sed -i '/# "local" is for Unix domain socket connections only/a local   all             chronicle_user                          trust' /etc/postgresql/16/main/pg_hba.conf

# Now restart postgresql for new configuration to be active
echo && echo "Restarting PostgreSQL to load configuration changes..."
systemctl restart postgresql

echo
echo && echo "Dropping Relic DB schema..."
sudo -u postgres psql -d relicdb -a -f ${SCRIPT_DIR}/sql/DropRelicStoredProcedures.sql
sudo -u postgres psql -d relicdb -a -f ${SCRIPT_DIR}/sql/DropRelicTables.sql

echo && echo "Creating FIO.Relic DB schema, including tables and stored procedures..."
echo
sudo -u postgres psql -U chronicle_user -d relicdb -a -f ${SCRIPT_DIR}/sql/CreateRelicTables.sql
sudo -u postgres psql -U chronicle_user -d relicdb -a -f ${SCRIPT_DIR}/sql/CreateRelicStoredProcedures.sql

# Update chronicle_user to have 'trust' authentication
sed -i 's/local[[:space:]]\+all[[:space:]]\+chronicle_user[[:space:]]\+trust/local   all             chronicle_user                          md5/' /etc/postgresql/16/main/pg_hba.conf

# Restart PostgreSQL to enable changes
echo && echo "Restarting PostgreSQL to load configuration changes..."
systemctl restart postgresql
echo