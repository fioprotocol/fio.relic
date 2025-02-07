#!/usr/bin/env bash

# Debug
#set -x

# Set up script environment
SCRIPT_DIR=$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)
. ${SCRIPT_DIR}/utils.sh

echo
if [[ "$EUID" -ne 0 ]]; then
  echo "ERROR: Script must be run as root! Use sudo command as follows; sudo ./<script name>"
  echo
  exit 1
fi

POSTGRES_VER=16

echo "PostgreSQL v${POSTGRES_VER} Uninstall"
echo
echo "Continuing will completely remove PostgreSQL packages and all related artifacts..."
pause

echo "Stopping, and disabling PostgreSQL service..."
systemctl stop postgresql &> /dev/null
systemctl disable postgresql &> /dev/null

echo
echo "Removing PostgreSQL packages..."
pause
apt-get --purge remove -y postgresql-${POSTGRES_VER}
apt-get --purge remove -y postgresql-server-dev-${POSTGRES_VER}
apt-get --purge remove -y postgresql-contrib-${POSTGRES_VER}
apt-get --purge remove -y postgresql-common
apt-get --purge remove -y libpq-dev
apt autoremove -y

echo
echo "Checking for any remaining PostgreSQL packages..."
dpkg -l | grep postgres
for pkg in `dpkg -l | grep postgres | awk '{print $2}'`; do
  echo
  echo Removing $pkg...
  pause
  apt-get --purge remove ${pkg}
done

echo
echo "Removing PostgreSQL related directories..."
pause
rm -rf /var/lib/postgresql/
rm -rf /var/log/postgresql/
rm -rf /etc/postgresql/

echo "Removing postgres user..."
pause
deluser postgres &> /dev/null

echo "Verifying PostgreSQL has been removed; the following command should fail..."
echo
psql --version &>/dev/null
if [[ $? -eq 0 ]]; then
  echo "WARNING: PostgreSQL has NOT been fully removed from the system; manual intervention is required to do so! Exiting..."
  exit 1
fi
echo "PostgreSQL has been successfully removed from the system"
