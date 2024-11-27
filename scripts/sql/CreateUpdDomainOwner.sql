CREATE OR REPLACE FUNCTION upddomainowner( 
    domainname    varchar(62),
    owneraccount varchar(12) 
) RETURNS int 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare owneracctid bigint;
     
    BEGIN  
    SELECT pk_domain_id INTO pkid FROM domains
         WHERE domain_name = domainname; 
    SELECT pk_account_id INTO owneracctid FROM accounts
         WHERE account_name = owneraccount; 
    
    UPDATE  domains SET 
            fk_owner_account_id = owneracctid
        WHERE pk_domain_id = pkid;

        RETURN pkid;
    END;
$BODY$;