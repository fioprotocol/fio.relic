CREATE OR REPLACE FUNCTION upddomainexp( 
    domainname    varchar(62),
    expirationtimestamp timestamp 
) RETURNS int 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    BEGIN  
    SELECT pk_domain_id INTO pkid FROM domains
         WHERE domain_name = domainname; 
    
    UPDATE  domains SET 
            expiration_timestamp = expirationtimestamp
        WHERE pk_domain_id = pkid;

        RETURN pkid;
    END;
$BODY$;