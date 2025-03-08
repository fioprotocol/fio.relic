CREATE OR REPLACE FUNCTION getaccountpubkeyandid( 
    accountname  varchar(12)
) RETURNS TABLE (fkaccountid bigint,
                 publickey varchar(53))   
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  

   BEGIN
    RETURN QUERY 
       SELECT pk_account_id, public_key from accounts
         WHERE account_name = accountname;
   END;

$BODY$;