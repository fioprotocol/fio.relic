##CHRONICLE

$ sudo apt install ./antelope-chronicle-3.3-Clang-11.0.1-ubuntu20.04-x86_64.deb


##FIO-NODE

Building: (commit 2896a72 (2896a72f6ba94b97fc04eff6fc8c4775f157f590) in branch feature/bd-4617-ubuntu-upgrade, the latest on 2024-03-30)
git clone -b feature/bd-4617-ubuntu-upgrade --single-branch --recursive https://github.com/fioprotocol/fio 

$ cd ~/fioprotocol/fio/scripts

(takes 2+ hours)
(!!! when asked where to install, enter: /etc/fio/nodeos)
$ ./fio_build.sh -P

OR might be needed:
$ sudo apt --fix-broken install
$ apt-get update
$ ./fio_build.sh -P

$ cd ~/fioprotocol/fio/scripts

(The first run took ~2 hours because it repeated building, why?! The second run did not build anything and took 1 sec.)
$ ./fio_install.sh

$ cd /etc/fio/nodeos
Copy here the right config:
$ /etc/fio/nodeos/config.ini


##RUNNING:

$ /etc/fio/nodeos/bin/nodeos --data-dir /var/lib/fio/data --config-dir /etc/fio/nodeos/ --genesis-json=/etc/fio/nodeos/genesis.json --disable-replay-opts &>nodeos_output.txt

Start test reader (!make sure it has $binary_hdr=1):
$ perl /opt/src/eos-chronicle/testing/chronicle-ws-dumper.pl &>chronicle_output.txt

$ /usr/local/sbin/chronicle-receiver --config-dir=/opt/src/eos-chronicle/ --data-dir=/opt/src/chronicle-data --start-block=<in the FIO node log: "chain_state_history.log has blocks:..." get the first number of the 2> &>chronicle_log.txt



