

CREATE OR REPLACE FUNCTION delpubaddress(  
    fiohandle varchar(64),
    chaincode varchar(12),
    tokencode varchar(12),
    pubaddress varchar(128)
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
declare handleid bigint;
    
    
    BEGIN  
        SELECT pk_handle_id INTO handleid FROM handles
           WHERE handle = fiohandle;
        DELETE FROM pubaddresses WHERE
            fk_handle_id = handleid AND
            chain_code = chaincode AND
            token_code = tokencode AND
            pub_address = pubaddress;
        RETURN 1;
    END;
$BODY$;