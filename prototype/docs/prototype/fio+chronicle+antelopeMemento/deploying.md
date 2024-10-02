##FIO-NODE:

#Installation: 
https://dev.fio.net/docs/installation-using-packages

#Check the package:
certutil -hashfile filepath MD5

#Installation on WSL:
- make sure Ubuntu is on WSL2;
- to have systemctl on, create /etc/wsl.conf and write there: 
[boot]
systemd=true
- $ wsl -t <Ubuntu>;
- start Ubuntu;

WSL HELP: see WSL_tips.md

#Configure:
/etc/fio/nodeos/config.ini

(Usually can be omitted:)Refresh there p2p-peer-addresses if needed from https://monitor.testnet.fioprotocol.io/#p2p

#Run:
$ sudo systemctl start fio-nodeos

#Stop:
$ sudo systemctl stop fio-nodeos

#Log:
/var/log/fio/nodeos.log

#Binary:
/usr/local/bin

#Service:
/lib/systemd/system/

#History:
/var/lib/fio/data/blocks

#Validation of the node:
https://dev.fio.net/docs/validate-your-api-node
$ curl -s -X GET 'http://localhost:8888/v1/chain/get_info' | jq '.chain_id'

#Snapshots:
https://snap.blockpane.com/chains/fio/
https://seed01.eosusa.news/snaps/fio/

#Mainnet monitor:
https://monitor.fioprotocol.io/

#Testnet monitor:
https://monitor.testnet.fioprotocol.io/


#Troubleshooting:

(!)If FIO was stopped not by sudo systemctl stop fio-nodeos OR Ctrl+X, it will not start with this error: ~database bit 1 <...> Replay is required.
Remedy: (remove existing history directories)
cd /var/lib/fio
sudo rm -fr data history history_index



##CHRONICLE:

https://github.com/EOSChronicleProject/eos-chronicle

/opt/src/eos-chronicle/

#Building (optional):
$ mkdir -p /opt/src/
$ cd /opt/src/
$ git clone --recursive https://github.com/EOSChronicleProject/eos-chronicle.git
$ cd eos-chronicle
$ ./pinned_build/install_deps.sh && mkdir build && nice ./pinned_build/chronicle_pinned_build.sh /opt/src/chronicle-deps /opt/src/eos-chronicle/build $(nproc)

#Installing
$ sudo apt install ./antelope-chronicle-3.3-Clang-11.0.1-ubuntu20.04-x86_64.deb

Register as service (optional):
$ cp /usr/local/share/chronicle_receiver@.service /etc/systemd/system/
$ systemctl daemon-reload

#Configure:
/opt/src/eos-chronicle/config.ini

#Initialize (!mandatory; only once per FIO-node start):
$ /usr/local/sbin/chronicle-receiver --config-dir=/opt/src/eos-chronicle/ --data-dir=/opt/src/chronicle-data --start-block=<in the FIO node log: "chain_state_history.log has blocks:..." get the first number of the 2>

#Start test reader (!make sure it has $binary_hdr=1):
$ perl /opt/src/eos-chronicle/testing/chronicle-ws-dumper.pl &>chronicle_output.txt
 
#Start:
$ /usr/local/sbin/chronicle-receiver --config-dir=/opt/src/eos-chronicle/ --data-dir=/opt/src/chronicle-data &>/opt/src/eos-chronicle/testing/chronicle_log.txt
OR
$ systemctl start chronicle_receiver@memento_wax1

Troubleshooting:

Error: chronicle receiver_plugin.cpp:726       receive_result       ] Irreversible block in state history (1025748) is lower than the one last seen
Remedy: purge /opt/src/chronicle-data 


MEMENTO:

https://github.com/Antelope-Memento/antelope_memento/?tab=readme-ov-file#installation

Set parameters in the memento scripts.


RUNNING everything together:

sudo systemctl start fio-nodeos

systemctl start memento_dbwriter@wax1

/usr/local/sbin/chronicle-receiver --config-dir=/srv/memento_wax1/chronicle-config --data-dir=/srv/memento_wax1/chronicle-data
OR
systemctl start chronicle_receiver@memento_wax1

Check received history
mysql memento_wax --execute="SELECT * from TRANSACTIONS"




GENERAL NOTES:

Antelope-smart-contract-developers-handbook
https://cc32d9.gitbook.io/antelope-smart-contract-developers-handbook/


WSL vhdx images:
c:\Users\serge\AppData\Local\Packages\CanonicalGroupLimited.Ubuntu<...>


Exporting WSL image:
(from PowerShell)
wsl -l -v
wsl --terminate <distroname>
wsl --export Ubuntu-20.04 "C:\temp\Ubuntu-20.04.tar"

Importing WSL image:
# This will erase your existing distro's file contents.
wsl --unregister <Distroname>
# Import your VHD backup
wsl --import-in-place <Distroname> <Install Location with filename>