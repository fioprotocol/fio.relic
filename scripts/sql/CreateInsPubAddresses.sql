
CREATE OR REPLACE FUNCTION insupdpubaddresses(  
    fkblocknumber bigint,
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
    
    BEGIN  
        IF (fkblocknumber< 0) THEN 
            RETURN -1;
        END IF;
        SELECT pk_handle_id from handles 
            WHERE handle = fiohandle INTO handleid ;
          
        SELECT pk_pub_addresses_id INTO pkid FROM pubaddresses 
            WHERE (fk_handle_id = pk_handle_id AND
                  chain_code = chaincode AND
                  token_code = tokencode );

        IF NOT FOUND THEN
            INSERT INTO pubaddresses ( 
                pk_pub_addresses_id,
                fk_block_number,
                fk_handle_id,
                chain_code,
                token_code,
                pub_address
            ) VALUES ( 
                DEFAULT,
                fkblocknumber,
                handleid,
                chaincode,
                tokencode,
                pubaddress
            ) RETURNING pk_pub_addresses_id INTO pkid ;
        ELSE
            UPDATE pubaddresses SET (
                fk_block_number = fkblocknumber,
                pub_address = pubaddress
            ) WHERE pk_pub_addresses_id = pkid;
        RETURN pkid;
    END;
$BODY$;