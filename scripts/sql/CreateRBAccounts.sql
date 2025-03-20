CREATE OR REPLACE FUNCTION rbaccounts( 
    bnumber bigint
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE    
AS 
$BODY$  


DECLARE 
    tcurse CURSOR FOR 
        SELECT fk_account_id, MAX(pk_accounts_audit_id) FROM accountsaudit 
          WHERE fk_account_id IN (
              SELECT pk_account_id FROM accounts 
                WHERE fk_block_number >= bnumber AND EXISTS (
                  SELECT fk_account_id FROM accountsaudit 
                      WHERE accounts.pk_account_id = accountsaudit.fk_account_id
                )
          ) AND fk_block_number < bnumber  GROUP BY  fk_account_id;
    retrow RECORD;
    rowsaffected bigint;

    BEGIN  
        IF (bnumber< 0) THEN 
            RETURN 0;
        END IF;

 OPEN tcurse;
 rowsaffected = 0;
 FETCH FROM tcurse INTO retrow;
  WHILE FOUND LOOP
     IF NOT (retrow.table_operation = 'update') THEN
         RAISE 'rollback of operations other than updates not supported on audit table';
     END IF;

     UPDATE accounts
          SET fk_block_number = subquery.fk_block_number,
              account_name = subquery.account_name,
              public_key = subquery.public_key,
              fio_balance_suf = subquery.fio_balance_suf,
              block_timestamp = subquery.block_timestamp
          FROM (
                 SELECT * FROM accountsaudit
                    WHERE pk_accounts_audit_id = retrow.max
                ) AS subquery
          WHERE pk_account_id = retrow.fk_account_id;
    rowsaffected = rowsaffected +1;
     FETCH NEXT FROM tcurse INTO retrow;
  END LOOP;
  CLOSE tcurse;  
  RETURN rowsaffected;
    END;
$BODY$;