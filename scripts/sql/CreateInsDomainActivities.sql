
CREATE OR REPLACE FUNCTION insdomainactivities(  
    fktransactionid bigint,
    fkblocknumber bigint,
    domainname varchar(62),
    expirationtimestamp timestamp,
    domainactivitytype varchar(20),
    blocktimestamp timestamp
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare domainid bigint;

    BEGIN  
        IF (fkblocknumber< 0) THEN 
            RETURN -1;
        END IF;
        SELECT pk_domain_id from domains 
            WHERE domain_name = domainname INTO domainid ;
      
        INSERT INTO domainactivities ( 
            pk_domain_activity_id, 
            fk_domain_id,
            fk_block_number, 
            fk_transaction_id,  
            expiration_stamp, 
            domain_activity_type,
            block_tImestamp
        ) VALUES ( 
            DEFAULT,
            domainid,
            fkblocknumber,
            fktransactionid,
            expirationtimestamp,
            domainactivitytype,
            blocktimestamp
        ) RETURNING pk_domain_activity_id INTO pkid ;
        RETURN pkid;
    END;
$BODY$;