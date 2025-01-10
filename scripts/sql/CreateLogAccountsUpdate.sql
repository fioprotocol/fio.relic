CREATE OR REPLACE FUNCTION logaccountsupdate( ) 
RETURNS TRIGGER 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  

  declare blocktimestamp timestamp;
    
    BEGIN  
        SELECT stamp INTO blocktimestamp FROM blocks
           WHERE pk_block_number = OLD.fk_block_number;
        INSERT INTO accountsaudit (
            pk_accounts_audit_id,
            fk_account_id,
            fk_block_number,
            account_name,
            public_key,
            block_timestamp,
            table_operation,
            table_timestamp
            ) VALUES (
                DEFAULT,
                OLD.pk_account_id,
                OLD.fk_block_number,
                OLD.account_name,
                OLD.public_key,
                OLD.block_timestamp,
                'update',
                blocktimestamp
            );
        RETURN NEW;
    END;
$BODY$;