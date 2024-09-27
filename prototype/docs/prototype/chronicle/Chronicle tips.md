##Deployment

#Repo:
https://github.com/EOSChronicleProject/eos-chronicle
#Local source directory (!!!cannot change)
/opt/src/eos-chronicle/

#Build(optional)
$ mkdir -p /opt/src/
$ cd /opt/src/
$ git clone --recursive https://github.com/EOSChronicleProject/eos-chronicle.git
$ cd eos-chronicle
$ ./pinned_build/install_deps.sh && mkdir build && nice ./pinned_build/chronicle_pinned_build.sh /opt/src/chronicle-deps /opt/src/eos-chronicle/build $(nproc)

#Install
$ sudo apt install ./antelope-chronicle-3.3-Clang-11.0.1-ubuntu20.04-x86_64.deb

Register as service (optional):
$ cp /usr/local/share/chronicle_receiver@.service /etc/systemd/system/
$ systemctl daemon-reload

#Configure
/opt/src/eos-chronicle/config.ini


##Run

First, start FIO node and Relic or Chronicle or dumper.

#Run dumper:
$ perl /opt/src/eos-chronicle/testing/chronicle-ws-dumper.pl &>chronicle_output.txt

#Initialize (!mandatory; only once per FIO-node start):
$ /usr/local/sbin/chronicle-receiver --config-dir=/opt/src/eos-chronicle/ --data-dir=/opt/src/chronicle-data --start-block=<in the FIO node log: "chain_state_history.log has blocks:..." get the first number of the 2>
 
#Start:
$ /usr/local/sbin/chronicle-receiver --config-dir=/opt/src/eos-chronicle/ --data-dir=/opt/src/chronicle-data &>chronicle_log.txt
OR
$ systemctl start chronicle_receiver@memento_wax1


##Troubleshoting
https://developers.eos.io/manuals/eos/v2.1/nodeos/troubleshooting/index

#Error: database dirty flag
Remedy: (remove existing history directories)
cd /var/lib/fio
sudo rm -fr data history history_index

Sometimes it seems to be caused by very bad internet. So restarting the FIO node with better internet might help.

#Error: Irreversible block in state history is lower than the one last seen
Remedy: purge /opt/src/chronicle-data 
