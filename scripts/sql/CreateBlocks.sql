
/*
This table holds Relic block information, 
producer account name is varchar here to avoid circular logic in table with account table.
*/
CREATE TABLE blocks ( 
      pk_block_number bigint PRIMARY KEY,  
      stamp timestamp, 
      block_id varchar(80) UNIQUE,
      producer_account_name varchar(12), 
      schedule_version varchar(64));