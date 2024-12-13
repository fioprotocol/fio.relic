
CREATE OR REPLACE FUNCTION updhandlesstatus(
    blocknumber bigint,
    fiohandle varchar(64),
    handlestatus varchar(20)
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
            handle_status = handlestatus
            WHERE pk_handle_id = pkid;

        RETURN pkid;
    END;
$BODY$;