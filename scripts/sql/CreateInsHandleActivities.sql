CREATE OR REPLACE FUNCTION inshandleactivities(  
    fktransactionid bigint,
    fkblocknumber bigint,
    fiohandle varchar(64),
    handleactivitytype varchar(20),
    blocktimestamp timestamp
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
            WHERE handle = fiohandle INTO handleid;

        INSERT INTO handleactivities ( 
            pk_handle_activity_id,
             fk_handle_id,
             fk_block_number,
            fk_transaction_id,
            handle_activity_type,
            block_tImestamp
        ) VALUES ( 
            DEFAULT,
            handleid,
            fkblocknumber,
            fktransactionid,
            handleactivitytype,
            blocktimestamp
        ) RETURNING pk_handle_activity_id INTO pkid ;
        RETURN pkid;
    END;
$BODY$;