
CREATE OR REPLACE FUNCTION updhandlessetencryptkey(
    blocknumber bigint,
    fiohandle varchar(64),
    encryptkey varchar(64)
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;

    BEGIN  
        SELECT pk_handle_id INTO pkid from handles 
            WHERE handle = fiohandle;
        
        UPDATE  handles SET 
            fk_block_number = blocknumber,
            encryption_key = encryptkey,
            is_encrypt_key_set = true
            WHERE pk_handle_id = pkid;
        RETURN pkid;
    END;
$BODY$;