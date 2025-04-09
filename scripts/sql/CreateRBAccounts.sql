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
       SELECT fk_account_id, MIN(pk_accounts_audit_id) FROM accountsaudit 
          WHERE fk_table_operation_block_number >= bnumber  GROUP BY  fk_account_id;

    retrow RECORD;
    rowsaffected bigint;


   ins_fk_account_id bigint;
   ins_fk_block_number bigint;
   ins_fk_table_operation_block_number bigint;
   ins_account_name varchar(12);
   ins_public_key varchar(53);
   ins_fio_balance_suf bigint;
   ins_block_timestamp timestamp;
   ins_table_operation varchar(20) ;
   ins_table_timestamp timestamp;


    BEGIN  
        IF (bnumber< 0) THEN 
            RETURN 0;
        END IF;

 OPEN tcurse;
 rowsaffected = 0;
 FETCH FROM tcurse INTO retrow;
  WHILE FOUND LOOP

        SELECT
                fk_account_id,
                fk_block_number,
                fk_table_operation_block_number,
                account_name,
                public_key,
                fio_balance_suf,
                block_timestamp,
                table_operation,
                table_timestamp

            INTO
                ins_fk_account_id,
                ins_fk_block_number,
                ins_fk_table_operation_block_number,
                ins_account_name,
                ins_public_key,
                ins_fio_balance_suf,
                ins_block_timestamp,
                ins_table_operation,
                ins_table_timestamp
             FROM accountsaudit WHERE
               pk_accounts_audit_id = retrow.min;

        IF NOT FOUND THEN
               RAISE EXCEPTION 'failed to find data required for rollback';
        END IF;

     IF NOT (ins_table_operation = 'update') THEN
         RAISE 'rollback of operations other than updates not supported on audit table';
     END IF;

     UPDATE accounts
          SET fk_block_number = ins_fk_block_number,
              account_name = ins_account_name,
              public_key = ins_public_key,
              fio_balance_suf = ins_fio_balance_suf,
              block_timestamp = ins_block_timestamp
          WHERE pk_account_id = retrow.fk_account_id;
    rowsaffected = rowsaffected +1;
     FETCH NEXT FROM tcurse INTO retrow;
  END LOOP;
  CLOSE tcurse;  
  RETURN rowsaffected;
    END;
$BODY$;