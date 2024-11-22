CREATE OR REPLACE FUNCTION getaccountid( 
    accountname  varchar(12)
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    BEGIN  
        SELECT pk_account_id from accounts
         WHERE account_name = accountname INTO pkid ;
        RETURN pkid;
    END;
$BODY$;