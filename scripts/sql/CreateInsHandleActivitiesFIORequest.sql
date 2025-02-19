CREATE OR REPLACE FUNCTION inshandleactivitiesfiorequest(  
    fktransactionid bigint,
    fkblocknumber bigint,
    fiochainrequestid bigint,
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
        SELECT fk_payee_handle_id from fiorequests 
            WHERE fio_chain_request_id = fiochainrequestid  INTO handleid;

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