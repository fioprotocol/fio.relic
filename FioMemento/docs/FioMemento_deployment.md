## Install Chronicle

wget https://github.com/EOSChronicleProject/eos-chronicle/releases/download/v2.4/eosio-chronicle-2.4-Clang-11.0.1-ubuntu20.04-x86_64.deb
apt install ./eosio-chronicle-2.4-Clang-11.0.1-ubuntu20.04-x86_64.deb
cp /usr/local/share/chronicle_receiver\@.service /etc/systemd/system/
systemctl daemon-reload


## Database engine

apt-get update && apt-get install -y mariadb-server mariadb-client


## Install MariaDB Connector/C and package dependencies:

sudo apt install libmariadb3 libmariadb-dev