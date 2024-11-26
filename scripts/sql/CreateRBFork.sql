/* this procedure will remove all records in the DB with block number >= the specified block number */
CREATE OR REPLACE FUNCTION rbfork( 
    bnumber bigint
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE    
AS 
$BODY$  
    BEGIN  
        IF (bnumber< 0) THEN 
            RETURN 0;
        END IF;
        DELETE FROM domainactivities WHERE  domainactivities.fk_block_number IN (
            SELECT domainactivities.fk_block_number FROM domainactivities where 
                fk_block_number >= bnumber);
        DELETE FROM domains WHERE  domains.fk_block_number IN (
            SELECT domains.fk_block_number FROM domains where 
                fk_block_number >= bnumber);
        DELETE FROM traces WHERE  traces.fk_block_number IN (
            SELECT traces.fk_block_number FROM traces where 
                fk_block_number >= bnumber);
        DELETE FROM tokenstakings where fk_transaction_id in (
            SELECT transactions.pk_transaction_id FROM transactions where 
                fk_block_number >= bnumber);
        DELETE FROM accountactivities where fk_transaction_id in (
                    SELECT transactions.pk_transaction_id FROM transactions where 
                fk_block_number >= bnumber);
        DELETE FROM accounts WHERE  accounts.fk_block_number IN (
            SELECT accounts.fk_block_number FROM accounts where 
                fk_block_number >= bnumber);
        DELETE FROM transactions WHERE  transactions.fk_block_number IN (
            SELECT transactions.fk_block_number FROM transactions where 
                fk_block_number >= bnumber);
        DELETE FROM blocks where pk_block_number >= bnumber; 
        RETURN 1;
    END;
$BODY$;