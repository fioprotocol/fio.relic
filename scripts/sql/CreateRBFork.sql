/* this procedure will remove all records in the DB with block number >= the specified block number */
CREATE OR REPLACE FUNCTION rbfork( 
    bnumber bigint
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE    
AS 
$BODY$  
declare auditid bigint;
    BEGIN  
        IF (bnumber< 0) THEN 
            RETURN 0;
        END IF;


       PERFORM rbhandles(bnumber);
       DELETE FROM nftsignatures WHERE  nftsignatures.fk_block_number >= bnumber;
       DELETE FROM fiodatas WHERE  fiodatas.fk_block_number >= bnumber;
       DELETE FROM fiorequests WHERE  fiorequests.fk_block_number >= bnumber;
       DELETE FROM pubaddresses WHERE  pubaddresses.fk_block_number >= bnumber;
       DELETE FROM handleactivities where handleactivities.fk_block_number >= bnumber;
       DELETE FROM handlesaudit WHERE  handlesaudit.fk_block_number  >= bnumber;
        DELETE FROM handles WHERE  handles.fk_block_number  >= bnumber;
        DELETE FROM domainactivities WHERE  domainactivities.fk_block_number >= bnumber;
        DELETE FROM domains WHERE  domains.fk_block_number >= bnumber;
        DELETE FROM traces WHERE  traces.fk_block_number >= bnumber;
        DELETE FROM tokenstakings where fk_transaction_id in (
            SELECT transactions.pk_transaction_id FROM transactions where 
                fk_block_number >= bnumber);
        DELETE FROM accountactivities where fk_transaction_id in (
                    SELECT transactions.pk_transaction_id FROM transactions where 
                fk_block_number >= bnumber);
       -- DELETE FROM accounts WHERE  accounts.fk_block_number  >= bnumber;
       -- DELETE FROM transactions WHERE  transactions.fk_block_number >= bnumber;
       -- DELETE FROM blocks where pk_block_number >= bnumber; 
        RETURN 1;
    END;
$BODY$;