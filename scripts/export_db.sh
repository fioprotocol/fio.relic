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
  echo "./scripts/export_db.sh <dump file>"
  echo
  exit -1
fi

if [[ -e "$1" ]]; then
  echo
  echo "WARNING: The FIO.Relic DB dump file, $1, already exists! Proceeding will overwrite file..." && echo
  pause
fi

# Verify fio.chronicle is not running
# As fio.chronicle chronicle captures state in-memory it must be shut down to capture consistent state
PID=$(pgrep chronicle)
if [[ -n $PID ]]; then
  echo && echo "ERROR: FIO.Chronicle appears to be running! To capture consistent state FIO.Chronicle should NOT be running..."
  echo
  exit 1
fi

# Check that the database exists
echo && echo "Checking Relic DB existence for DB export..."
if ! sudo -u postgres psql -lqt | cut -d \| -f 1 | grep -qw relicdb; then
  echo && echo "ERROR: It does not appear that the FIO.Relic DB, 'relicdb', exists! Exiting..."
  exit 1
fi

# Check that the database user exists
echo && echo "Checking Relic DB User for DB export..."
if ! sudo -u postgres psql -tXAc "SELECT 1 FROM pg_roles WHERE rolname='chronicle_user'" | grep -q 1; then
  echo && echo "ERROR: It does not appear that the FIO.Relic DB user, 'chronicle_user', exists! Exiting..."
  exit 1
fi

# Steps
# 1) export (dump) existing db including users, tables, functions (stored procs), data
echo && echo "INFO: A DB export will dump users, all existing tables, all functions and all data..."
echo
if ! yes_or_no "Proceed"; then
  echo && echo "Exiting export of FIO.Relic DB!";
  echo
  exit 1
fi

# Stop fio.chronicle with using its stop script
# Capture fio.chronicle state with
#   tar -czvf /opt/fio-chronicle/bkups/rcvr-state.bkup1.tar.gz /opt/fio-chronicle/data/receiver-state/
# Capture fio.relic db with
#   sudo ./scripts/export_db.sh /opt/fio-chronicle/bkups/relicdb.bkup1.tar.gz
dumpfile="${1}"
echo && echo "Exporting Relic DB to ${dumpfile}..."
sudo rm -f ${dumpfile}
sudo -u postgres pg_dump relicdb > ${dumpfile}

if [[ ! -e "${dumpfile}" ]]; then
  echo && echo "ERROR: ${dumpfile} is NOT valid; DB export failed!" && echo
  exit -1
fi
chown $(id -un):$(id -gn) $dumpfile
echo && echo "INFO: Export completed, see ${dumpfile}." && echo

# And done
echo && echo "Done!"
