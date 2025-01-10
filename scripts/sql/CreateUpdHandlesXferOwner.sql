
CREATE OR REPLACE FUNCTION updhandlesxferowner(
    blocknumber bigint,
    fiohandle varchar(64),
    owneraccount varchar(12),
    encryptkey varchar(64),
    encryptkeyisset boolean
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare owneracctid integer;

    BEGIN  
        SELECT pk_handle_id INTO pkid from handles 
            WHERE handle = fiohandle;
        SELECT pk_account_id INTO owneracctid from accounts 
            WHERE account_name = owneraccount;

        UPDATE  handles SET 
            fk_block_number = blocknumber,
            fk_owner_account_id = owneracctid,
            encryption_key = encryptkey,
            is_encrypt_key_set = encryptkeyisset
            WHERE pk_handle_id = pkid;
        RETURN pkid;
    END;
$BODY$;