## Install Chronicle
wget https://github.com/EOSChronicleProject/eos-chronicle/releases/download/v2.4/eosio-chronicle-2.4-Clang-11.0.1-ubuntu20.04-x86_64.deb
apt install ./eosio-chronicle-2.4-Clang-11.0.1-ubuntu20.04-x86_64.deb
cp /usr/local/share/chronicle_receiver\@.service /etc/systemd/system/
systemctl daemon-reload


## Database engine
sudo apt-get update && apt-get install -y mariadb-server mariadb-client


## Deploy database
sh ./create_db_users.sh
sh ./create_memento_db.sh FioMemento


## Install MariaDB Connector/C and package dependencies:
(https://mariadb.com/docs/server/connect/programming-languages/c/install/)
sudo apt install libmariadb3 libmariadb-dev


## Install MariaDB Connector/C++ and package dependencies:
(https://mariadb.com/docs/server/connect/programming-languages/cpp/install/)
$ wget https://dlm.mariadb.com/3752139/Connectors/cpp/connector-cpp-1.1.3/mariadb-connector-cpp_1.1.3-1+maria~focal_amd64.deb
$ sudo dpkg -i mariadb-connector-cpp_1.1.3-1+maria~focal_amd64.deb

MANUAL INSTALLATION (if deb is not complete):
$ wget https://dlm.mariadb.com/3752103/Connectors/cpp/connector-cpp-1.1.3/mariadb-connector-cpp-1.1.3-ubuntu-focal-amd64.tar.gz
$ tar -xvzf mariadb-connector-cpp-1.1.3-ubuntu-focal-amd64.tar.gz
$ cd mariadb-connector-cpp-1.1.3-ubuntu-focal-amd64
Install the directories for the header files:
$ sudo install -d /usr/include/mariadb/conncpp
$ sudo install -d /usr/include/mariadb/conncpp/compat
Install the header files:
$ sudo install include/mariadb/* /usr/include/mariadb/
$ sudo install include/mariadb/conncpp/* /usr/include/mariadb/conncpp
$ sudo install include/mariadb/conncpp/compat/* /usr/include/mariadb/conncpp/compat
Install the directories for the shared libraries:
$ sudo install -d /usr/lib/mariadb
$ sudo install -d /usr/lib/mariadb/plugin
Install the shared libraries:
$ sudo install lib/mariadb/libmariadbcpp.so /usr/lib
$ sudo install lib/mariadb/plugin/* /usr/lib/mariadb/plugin


## Install boost packages required by appbase (to build the app):
sudo apt-get install libboost-system-dev
sudo apt-get install libboost-filesystem-dev
sudo apt-get install libboost-date-time-dev
sudo apt-get install libboost-chrono-dev
sudo apt-get install libboost-program-options-dev
sudo apt-get install libboost-test-dev