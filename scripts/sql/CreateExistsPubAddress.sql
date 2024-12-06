CREATE OR REPLACE FUNCTION existspubaddress( 
    fiohandle varchar(64),
    chaincode varchar(12),
    tokencode varchar(12),
    pubaddress varchar(128) 
) RETURNS int 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare handleid bigint;
    declare domexpire timestamp;
    declare domispublic boolean;
    BEGIN  

    SELECT pk_handle_id INTO handleid from handles 
            WHERE handle = fiohandle;

    SELECT pk_pub_addresses_id_id  FROM pubaddresses
         WHERE fk_handle_id = handleid AND 
               chain_code = chaincode AND 
               token_code = tokencode AND
               pub_address = pubaddress;

     
    IF NOT FOUND THEN
        return 0;
    ELSE
        return 1;
    END IF;
       
    END;
$BODY$;