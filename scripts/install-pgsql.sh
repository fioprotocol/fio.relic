#!/usr/bin/env bash

# Utility functions
function pause(){
  echo
  read -s -n 1 -p "Press any key to continue (CTRL-c to exit)..."
  echo
  echo
}

echo
if [[ "$EUID" -ne 0 ]]; then
  echo "ERROR: Script must be run as root! Use sudo command as follows; sudo ./<script name>"
  echo
  exit 1
fi

echo "Updating OS..."
echo
apt update;
apt upgrade;


echo "Installing required packages..."
echo
apt install gnupg2 wget vim -y

echo
echo "Adding the postgres repository..."
sh -c 'echo "deb https://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" > /etc/apt/sources.list.d/pgdg.list'

echo
echo "Set the signing key for the postgres repository..."
curl -fsSL https://www.postgresql.org/media/keys/ACCC4CF8.asc | gpg --dearmor -o /etc/apt/trusted.gpg.d/postgresql.gpg

echo
echo "Updating package list (again)..."
apt update -y

echo
echo "Installing PostgreSQL v16.x (Note install any dependencies it requires)..."
apt install postgresql-16 postgresql-contrib-16

#echo
#echo "Installing LTS version of PostgreSQL (PostgreSQL 17)..."
#apt-get -y install postgresql postgresql-contrib

if [[ $? -eq 0 ]]; then
  echo
  echo "PostgreSQL has been installed successfully"

  echo
  echo "In another window, verify install was successful using the following commands before proceeding;"
  echo "psql --version OR sudo -u postgres psql -c \"SELECT version();\""
  pause

  echo "Start and enable the service..."
  pause
  systemctl enable postgresql
  systemctl start postgresql

  echo
  echo "Checking service status..."
  sleep 5
  systemctl status postgresql
fi
