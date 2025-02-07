#!/usr/bin/env bash

# Debug
#set -x

echo && echo "PostgreSQL v${POSTGRES_VER} Install"

# Set up script environment
SCRIPT_DIR=$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)
. ${SCRIPT_DIR}/utils.sh

if [[ "$EUID" -ne 0 ]]; then
  echo
  echo "ERROR: Script must be run as root! Use sudo command as follows; sudo ./<script name>"
  echo
  exit 1
fi

if [[ "$(uname)" == "Linux" ]]; then
   if [[ -e /etc/os-release ]]; then
      # obtain NAME and other information
      . /etc/os-release
      if [[ ${NAME} != "Ubuntu" ]]; then
         echo "Currently only supporting Ubuntu based insteall. Proceed at your own risk."
         pause
      fi
   else
       echo "Currently only supporting Ubuntu based install. /etc/os-release not found. Your Linux distribution is not supported. Proceed at your own risk."
       pause
   fi
else
    echo "Currently only supporting Ubuntu based install. Your architecture is not supported. Proceed at your own risk."
    pause
fi

POSTGRES_VER=16

# Begin install

echo
echo "Continuing will install PostgreSQL packages and all related artifacts..."
pause

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
echo "Installing PostgreSQL v${POSTGRES_VER}.x..."
apt install -y postgresql-${POSTGRES_VER} postgresql-server-dev-${POSTGRES_VER} postgresql-contrib-${POSTGRES_VER} libpq-dev

if [[ $? -eq 0 ]]; then
  echo
  echo "PostgreSQL v${POSTGRES_VER} has been installed successfully"

  echo
  echo "In another window, verify the install using the following commands;"
  echo "psql --version OR sudo -u postgres psql -c \"SELECT version();\""
  echo
  echo "Both commands should result in the display of the installed PostgreSQL version. In"
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