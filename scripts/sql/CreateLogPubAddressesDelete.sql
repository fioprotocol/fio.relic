CREATE OR REPLACE FUNCTION logpubaddressesdelete( ) 
RETURNS TRIGGER 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  

  declare blocktimestamp timestamp;
    
    BEGIN  
        SELECT stamp INTO blocktimestamp FROM blocks
        WHERE pk_block_number = OLD.fk_block_number;
    
        INSERT INTO pubaddressesaudit (
            pk_pub_addresses_audit_id,
            fk_pub_addresses_id,
            fk_block_number,
            fk_handle_id,
            chain_code,
            token_code,
            pub_address,
            table_operation,
            table_timestamp
            ) VALUES (
                DEFAULT,
                OLD.pk_pub_addresses_id,
                OLD.fk_block_number,
                OLD.fk_handle_id,
                OLD.chain_code,
                OLD.token_code,
                OLD.pub_address,
                'delete',
                blocktimestamp
            );
        RETURN NEW;
    END;
$BODY$;