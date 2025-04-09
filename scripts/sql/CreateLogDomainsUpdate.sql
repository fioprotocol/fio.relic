CREATE OR REPLACE FUNCTION logdomainsupdate( ) 
RETURNS TRIGGER 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  

  declare blocktimestamp timestamp;
  declare currentblock bigint;
    
    BEGIN  
        SELECT stamp INTO blocktimestamp FROM blocks
        WHERE pk_block_number = OLD.fk_block_number;

        SELECT MAX(pk_block_number) INTO currentblock from blocks;
    
        INSERT INTO domainsaudit (
            pk_domains_audit_id,
            fk_domain_id,
            fk_block_number,
            fk_table_operation_block_number,
            domain_name,
            fk_owner_account_id,
            is_public,
            expiration_timestamp,
            domain_status,
            table_operation,
            table_timestamp
            ) VALUES (
                DEFAULT,
                OLD.pk_domain_id,
                OLD.fk_block_number,
                currentblock,
                OLD.domain_name,
                OLD.fk_owner_account_id,
                OLD.is_public,
                OLD.expiration_timestamp,
                OLD.domain_status,
                'update',
                blocktimestamp
            );
        RETURN NEW;
    END;
$BODY$;