cat <<EOF | mysql
CREATE USER 'Relic_rw'@'localhost' IDENTIFIED BY 'lkdsnvkfdjnflskdm';
CREATE USER 'Relic_ro'@'%' IDENTIFIED BY 'Relic_ro';
EOF
