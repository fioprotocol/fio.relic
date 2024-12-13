
CREATE OR REPLACE FUNCTION updhandlesencryptkey(
    blocknumber bigint,
    fiohandle varchar(64),
    encryptkey varchar(64)
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;

    declare encryptkeyisset boolean;

    BEGIN  
        SELECT pk_handle_id, is_encrypt_key_set INTO pkid,encryptkeyisset from handles 
            WHERE handle = fiohandle;
        
        IF NOT encryptkeyisset THEN
          UPDATE  handles SET 
            fk_block_number = blocknumber,
            encryption_key = encryptkey
            WHERE pk_handle_id = pkid;
        END IF;
        RETURN pkid;
    END;
$BODY$;