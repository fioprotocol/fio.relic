
CREATE OR REPLACE FUNCTION clearpubaddresses(  
    fiohandle varchar(64)
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
            fk_handle_id = handleid AND NOT(chain_code = 'FIO' AND token_code = 'FIO');
        RETURN 1;
    END;
$BODY$;