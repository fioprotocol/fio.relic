##Troubleshoting
https://developers.eos.io/manuals/eos/v2.1/nodeos/troubleshooting/index

#Error: database dirty flag
Remedy: (remove existing history directories)
$ cd /var/lib/fio
$ sudo rm -fr data history history_index

Sometimes it seems to be caused by very bad internet. So restarting the FIO node with better internet might help.

#Error: database bit 1 <...> Replay is required.
(!)If FIO was stopped not by 
$ sudo systemctl stop fio-nodeos 
OR 
$ Ctrl+C
it will not start with this error: ~database bit 1 <...> Replay is required.
Remedy: (remove existing history directories)
$ cd /var/lib/fio
$ sudo rm -fr data history history_index



##Installation
https://dev.fio.net/docs/installation-using-packages

#Installation on WSL:
- make sure Ubuntu is on WSL2;
- to have systemctl on, create /etc/wsl.conf and write there: 
[boot]
systemd=true
- $ wsl -t <Ubuntu>;
- start Ubuntu;

WSL HELP: see WSL_tips.md

#Configure:
Edit /etc/fio/nodeos/config.ini

(Usually can be omitted:)Refresh there p2p-peer-addresses if needed from https://monitor.testnet.fioprotocol.io/#p2p

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


##Run
Make sure that the FIO node with enabled state_history_plugin is running. 

#Run as a non-service-process:
$ /etc/fio/nodeos/bin/nodeos --data-dir /var/lib/fio/data --config-dir /etc/fio/nodeos/ --genesis-json=/etc/fio/nodeos/genesis.json --disable-replay-opts &>nodeos_output.txt

Stop:
$ Ctrl+C

#Run as a service:
$ sudo systemctl start fio-nodeos

Stop:
$ sudo systemctl stop fio-nodeos
