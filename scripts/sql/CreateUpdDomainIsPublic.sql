CREATE OR REPLACE FUNCTION upddomainispublic( 
    domainname    varchar(62),
    ispublic boolean 
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
            is_public = ispublic
        WHERE pk_domain_id = pkid;

        RETURN pkid;
    END;
$BODY$;