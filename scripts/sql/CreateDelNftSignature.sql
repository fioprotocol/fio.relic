

CREATE OR REPLACE FUNCTION delnftsignature(  
    fiohandle varchar(64),
    chaincode varchar(12),
    contractaddress varchar(128),
    tokenid varchar(64)
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
declare handleid bigint;
    BEGIN  
    SELECT pk_handle_id INTO handleid FROM handles
         WHERE handle = fiohandle;

        DELETE FROM nftsignatures WHERE
            fk_handle_id = handleid AND 
            chain_code = chaincode AND 
            contract_address = contractaddress AND 
            token_id = tokenid;
        RETURN 1;
    END;

$BODY$;