CREATE OR REPLACE FUNCTION updhandleburnt( 
    blocknumber bigint,
    fiohandle    varchar(64)
) RETURNS int 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    
    BEGIN  
    SELECT pk_handle_id
        INTO pkid FROM handles 
        WHERE handle = fiohandle; 

    IF FOUND THEN
        UPDATE handles SET
            fk_block_number = blocknumber,
            fk_owner_account_id = null,
            encryption_key = '',
            is_encrypt_key_set = false,
            handle_status = 'burnt'
        WHERE pk_handle_id = pkid;
        RETURN 1;
    ELSE
        RETURN 0;
    END IF;
    END;
$BODY$;