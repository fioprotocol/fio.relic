#Troubleshoting
https://developers.eos.io/manuals/eos/v2.1/nodeos/troubleshooting/index

#Error: database dirty flag
Remedy: (remove existing history directories)
cd /var/lib/fio
sudo rm -fr data history history_index

Sometimes it seems to be caused by very bad internet. So restarting the FIO node with better internet might help.

#Error: Irreversible block in state history is lower than the one last seen
Remedy: purge /opt/src/chronicle-data 
