#!/usr/bin/env bash

# Debug
#set -x

# Load utilities
SCRIPT_DIR=$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)
. ${SCRIPT_DIR}/utils.sh

echo
if [[ "$EUID" -ne 0 ]]; then
  echo "ERROR: Script must be run as root! Use sudo command as follows; sudo ./<script name>"
  echo
  exit 1
fi

#echo "WARNING: This script runs commands as the 'postgres' user using sudo to update the database'
#pause

# tables: droprelictables.sql.
# stored procs: dropstoredprocedures.sql.

# 1) drop relic db stored procedures: droprelicstoredprocedures.sql
# 2) drop relic db tables: droprelictables.sql
# 3) drop the relicdb, db: relicdb
# 4) delete the user account, user: chronicle_user 

echo
echo "Dropping Relic DB schema..."
echo
sudo -u postgres psql -d relicdb -a -f ${SCRIPT_DIR}/sql/DropRelicStoredProcedures.sql
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
  echo "Un-configuring Relic DB User Access..."
  # Backup pg_hba.conf file b4 modifying
  if [[ -e /etc/postgresql/16/main/pg_hba.conf.relic ]]; then
    mv /etc/postgresql/16/main/pg_hba.conf.relic /etc/postgresql/16/main/pg_hba.conf
    chown postgres:postgres /etc/postgresql/16/main/pg_hba.conf
  else
    # Update pg_hba.conf file to chronicle_user
    if grep -q chronicle_user /etc/postgresql/16/main/pg_hba.conf; then
      sed -i '/local[[:space:]]\+all[[:space:]]\+chronicle_user/d' /etc/postgresql/16/main/pg_hba.conf
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
echo "Restarting PostgreSQL to load configuration changes..."
systemctl restart postgresql
