

CREATE OR REPLACE FUNCTION updfiorequestsstatus(  
    fkchainrequestid bigint,
    requeststatus varchar(20)
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;

     
    BEGIN  

    SELECT pk_fio_requests_id  INTO pkid FROM fiorequests
       where fio_chain_request_id = fkchainrequestid;
   
    IF FOUND THEN
        UPDATE fiorequests SET 
            request_status = requeststatus
            WHERE pk_fio_requests_id = pkid;
        RETURN pkid;
    END IF;
    RETURN 1;
    END;
$BODY$;