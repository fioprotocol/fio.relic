CREATE OR REPLACE FUNCTION upddomainburnt( 
    domainname    varchar(62),
    expirationtimestamp timestamp,
    ispublic boolean 
) RETURNS int 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare domexpire timestamp;
    declare domispublic boolean;
    BEGIN  
    SELECT pk_domain_id, expiration_timestamp, is_public INTO 
    pkid, domexpire, domispublic FROM domains
         WHERE domain_name = domainname; 
    
    IF(domexpire = expirationtimestamp AND domexpire = ispublic) THEN
        UPDATE  domains SET 
                domain_status = 'burnt'
            WHERE pk_domain_id = pkid;
        RETURN 1;
    ELSE 
        RETURN 0;
    END IF;

    END;
$BODY$;