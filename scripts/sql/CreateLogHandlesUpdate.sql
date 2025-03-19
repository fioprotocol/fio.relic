CREATE OR REPLACE FUNCTION loghandlesupdate( ) 
RETURNS TRIGGER 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  

  declare blocktimestamp timestamp;
    
    BEGIN  
        SELECT stamp INTO blocktimestamp FROM blocks
        WHERE pk_block_number = OLD.fk_block_number;
    
        INSERT INTO handlesaudit (
                pk_handles_audit_id,
                fk_handle_id,
                fk_block_number,
                fk_domain_id,
                handle,
                fk_owner_account_id,
                encryption_key,
                is_encrypt_key_set,
                expiration_stamp,
                handle_status,
                table_operation,
                table_timestamp 
            ) VALUES (
                DEFAULT,
                OLD.pk_handle_id,
                OLD.fk_block_number,
                OLD.fk_domain_id,
                OLD.handle,
                OLD.fk_owner_account_id,
                OLD.encryption_key,
                OLD.is_encrypt_key_set,
                OLD.expiration_stamp,
                OLD.handle_status,
                'update',
                blocktimestamp
            );
        RETURN NEW;
    END;
$BODY$;