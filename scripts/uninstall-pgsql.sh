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

apt-get --purge remove postgresql
apt-get --purge remove postgresql-17
apt-get --purge remove postgresql-16

echo
echo "Checking for any remaining PostgreSQL packages..."
dpkg -l | grep postgres

echo
echo "Run the following command to remove each package listed above; sudo apt-get --purge remove <package>"
pause

echo
echo "Removing PostgreSQL related directories..."
rm -rf /var/lib/postgresql/
rm -rf /var/log/postgresql/
rm -rf /etc/postgresql/

echo
echo "Removing postgres user..."
deluser postgres

echo
echo "Verifying PostgreSQL has been removed; the following command should fail..."
echo
psql --version
