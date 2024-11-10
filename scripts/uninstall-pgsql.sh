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
echo "PostgreSQL Uninstall Script"
echo
echo "Continuing will completely remove PostgreSQL packages and all related artifacts..."
pause

echo
echo "Stopping, and disabling PostgreSQL service..."
systemctl stop postgresql &> /dev/null
systemctl disable postgresql &> /dev/null
#rm /usr/lib/systemd/system/*postgresql
#rm /etc/init.d/*postgresql

echo
echo "Removing PostgreSQL packages..."
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
  apt-get --purge remove ${pkg}
  echo
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
deluser postgres &> /dev/null

echo
echo "Verifying PostgreSQL has been removed; the following command should fail..."
echo
psql --version &>/dev/null
if [[ $? -eq 0 ]]; then
  echo "WARNING: PostgreSQL has NOT been fully removed from the system; manual intervention is required to do so! Exiting..."
  exit 1
fi
echo "PostgreSQL has been successfully removed from the system"
