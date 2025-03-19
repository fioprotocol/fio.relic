
CREATE OR REPLACE FUNCTION insupdhandles(
    fkblocknumber bigint,
    domainname varchar(64),
    owneraccountname varchar(12),
    fiohandle varchar(64),
    encryptkey varchar(64), 
    isencryptkeyset boolean, 
    expirationtimestamp timestamp,
    handlestatus varchar(20)
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare domainid bigint;
    declare owneraccountid bigint;

    BEGIN  
        IF (fkblocknumber< 0) THEN 
            RETURN -1;
        END IF;
        SELECT pk_account_id INTO owneraccountid from accounts 
            WHERE account_name = owneraccountname;
       
        SELECT pk_domain_id INTO domainid from domains 
            WHERE domain_name = domainname;

        SELECT pk_handle_id INTO pkid from handles 
            WHERE handle = fiohandle;

        IF NOT FOUND THEN
            INSERT INTO handles (
                pk_handle_id,
                fk_block_number,
                fk_domain_id,
                handle,
                fk_owner_account_id,
                encryption_key,
                is_encrypt_key_set,
                expiration_stamp,
                handle_status
            ) VALUES (
                DEFAULT,
                fkblocknumber,
                domainid,
                fiohandle,
                owneraccountid,
                encryptkey,
                isencryptkeyset,
                expirationtimestamp,
                handlestatus
            ) RETURNING pk_handle_id INTO pkid ;
        ELSE
            UPDATE  handles SET 
                fk_block_number = fkblocknumber,
                fk_domain_id = domainid,
                handle = fiohandle,
                fk_owner_account_id = owneraccountid,
                encryption_key = encryptkey,
                is_encrypt_key_set = isencryptkeyset,
                expiration_stamp = expirationtimestamp,
                handle_status = handlestatus
            WHERE pk_handle_id = pkid;
        END IF;
        RETURN pkid;
    END;
$BODY$;