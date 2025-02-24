CREATE OR REPLACE FUNCTION upddomainburnt( 
    domainname    varchar(62)
) RETURNS int 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  
    BEGIN  
   

        UPDATE  domains SET 
                domain_status = 'burnt'
            WHERE domain_name = domainname;
        RETURN 1;

    END;
$BODY$;