#!/usr/bin/env bash

echo
if [[ "$EUID" -ne 0 ]]; then
  echo "ERROR: Script must be run as root! Use sudo command as follows; sudo ./<script name>"
  echo
  exit 1
fi

# Utility functions
function pause(){
  echo
  read -s -n 1 -p "Press any key to continue (CTRL-c to exit)..."
  echo
  echo
}

echo
echo "Confirm complete removal of the PostgreSQL packages and all related artifacts..."
pause

apt-get --purge remove postgresql
apt-get --purge remove postgresql-17
apt-get --purge remove postgresql-16

echo
echo "Checking for any remaining PostgreSQL packages..."
dpkg -l | grep postgres
echo
for pkg in `dpkg -l | grep postgres | awk '{print $2}'`; do
  echo Removing $pkg...
  pause
  echo
  apt-get --purge remove ${pkg}
done

echo
echo "Removing PostgreSQL related directories..."
pause
rm -rf /var/lib/postgresql/
rm -rf /var/log/postgresql/
rm -rf /etc/postgresql/

echo
echo "Removing postgres user..."
pause
deluser postgres

echo
echo "Verifying PostgreSQL has been removed; the following command should fail..."
echo
psql --version &>/dev/null
if [[ $? -eq 0 ]]; then
  echo "WARNING: PostgreSQL has NOT been fully removed from the system; manual intervention is required to do so! Exiting..."
  exit 1
fi
echo "PostgreSQL has been successfully removed from the system"
