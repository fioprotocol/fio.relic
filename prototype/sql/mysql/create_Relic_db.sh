DB=$1
if [ x${DB} = x ]; then echo "database name required" 1>&2; exit 1; fi

cat <<EOF | mysql
CREATE DATABASE ${DB};
GRANT ALL ON ${DB}.* TO 'Relic_rw'@'localhost';
GRANT SELECT on ${DB}.* to 'Relic_ro'@'%';
EOF

mysql ${DB} <Relic_mysql.sql

