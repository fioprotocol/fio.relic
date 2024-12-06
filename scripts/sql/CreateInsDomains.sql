
CREATE OR REPLACE FUNCTION insdomains(  
    fkblocknumber bigint,
    domainname varchar(62),
    owneraccount varchar(12),
    ispublic boolean,
    expirationtimestamp timestamp,
    domainstatus varchar(20)
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare owneraccountid bigint;

    BEGIN  
        IF (fkblocknumber< 0) THEN 
            RETURN -1;
        END IF;
        SELECT pk_account_id from accounts 
            WHERE account_name = owneraccount INTO owneraccountid ;
       
        INSERT INTO domains ( 
            pk_domain_id, 
            fk_block_number, 
            domain_name,  
            fk_owner_account_id, 
            is_public,
            expiration_timestamp,
            domain_status
        ) VALUES ( 
            DEFAULT,
            fkblocknumber,
            domainname,
            owneraccountid,
            ispublic,
            expirationtimestamp,
            domainstatus
        ) RETURNING pk_domain_id INTO pkid ;
        RETURN pkid;
    END;
$BODY$;