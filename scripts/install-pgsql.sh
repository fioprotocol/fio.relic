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

if [[ "$(uname)" == "Linux" ]]; then
   if [[ -e /etc/os-release ]]; then
      # obtain NAME and other information
      . /etc/os-release
      if [[ ${NAME} != "Ubuntu" ]]; then
         echo && echo "Currently only supporting Ubuntu based insteall. Proceed at your own risk."
      fi
   else
       echo && echo "Currently only supporting Ubuntu based install. /etc/os-release not found. Your Linux distribution is not supported. Proceed at your own risk."
   fi
else
    echo && echo "Currently only supporting Ubuntu based install. Your architecture is not supported. Proceed at your own risk."
fi

# Set up script environment
SCRIPT_DIR=$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)

. ${SCRIPT_DIR}/utils.sh

# Begin install
echo "Updating OS..."
pause
echo
apt update;
apt upgrade -y;

echo
echo "Installing required packages..."
echo
apt install -y gnupg2 wget vim

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
echo "Installing PostgreSQL v16.x..."
apt install -y postgresql-16 postgresql-server-dev-16 postgresql-contrib-16 libpq-dev

#echo
#echo "Installing LTS version of PostgreSQL (PostgreSQL 17)..."
#apt-get -y install postgresql postgresql-contrib

if [[ $? -eq 0 ]]; then
  echo
  echo "PostgreSQL has been installed successfully"

  echo
  echo "In another window, verify the install using the following commands;"
  echo "psql --version OR sudo -u postgres psql -c \"SELECT version();\""
  echo
  echo "Note; both commands should result in the display of the installed PostgreSQL version. In"
  echo "case of an error, exit this script, determine the failure reason(s) and fix the installation"
  echo

  echo "Start and enable the PostgreSQL service..."
  pause
  systemctl enable postgresql
  systemctl start postgresql

  echo
  echo "Checking service status..."
  sleep 5
  systemctl status postgresql
else
  echo
  echo "An error occured installing PostgreSQL. Unable to proceed!"
  echo "Exiting..."
fi
echo