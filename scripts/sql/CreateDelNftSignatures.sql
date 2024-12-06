

CREATE OR REPLACE FUNCTION delnftsignatures(  
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

        DELETE FROM nftsignatures WHERE
            fk_handle_id = handleid;
        RETURN 1;
    END;

$BODY$;