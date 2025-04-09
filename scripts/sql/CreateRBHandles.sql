CREATE OR REPLACE FUNCTION rbhandles( 
    bnumber bigint
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE    
AS 
$BODY$  


DECLARE 
    hacurse CURSOR FOR 
        SELECT fk_handle_id, MIN(pk_handles_audit_id) FROM handlesaudit 
          WHERE fk_table_operation_block_number >= bnumber  GROUP BY  fk_handle_id;

    retrow RECORD;
    rowsaffected bigint;

    ins_pk_handles_audit_id bigint;
    ins_fk_handle_id bigint ;
    ins_fk_block_number bigint ;
    ins_fk_table_operation_block_number bigint;
    ins_fk_domain_id bigint;
    ins_handle varchar(64);
    ins_fk_owner_account_id bigint;
    ins_encryption_key varchar(64);
    ins_is_encrypt_key_set boolean;
    ins_expiration_stamp timestamp;
    ins_handle_status varchar(20);
    ins_table_operation varchar(20);
    ins_table_timestamp timestamp;

    BEGIN  
        IF (bnumber< 0) THEN 
            RETURN 0;
        END IF;

 OPEN hacurse;
 rowsaffected = 0;
 FETCH FROM hacurse INTO retrow;
  WHILE FOUND LOOP

   SELECT      
    fk_handle_id,
    fk_block_number,
    fk_table_operation_block_number,
    fk_domain_id,
    handle,
    fk_owner_account_id,
    encryption_key,
    is_encrypt_key_set,
    expiration_stamp ,
    handle_status,
    table_operation,
    table_timestamp 


    INTO
        ins_fk_handle_id,
        ins_fk_block_number,
        ins_fk_table_operation_block_number,
        ins_fk_domain_id,
        ins_handle,
        ins_fk_owner_account_id,
        ins_encryption_key,
        ins_is_encrypt_key_set,
        ins_expiration_stamp ,
        ins_handle_status,
        ins_table_operation,
        ins_table_timestamp 
    FROM handlesaudit WHERE
               pk_handles_audit_id = retrow.min;

        IF NOT FOUND THEN
               RAISE EXCEPTION 'failed to find data required for rollback';
        END IF;

     IF NOT (ins_table_operation = 'update') THEN
         RAISE 'rollback of operations other than updates not supported on audit table';
     END IF;


     UPDATE handles
          SET fk_block_number = ins_fk_block_number,
              fk_domain_id = ins_fk_domain_id,
              handle = ins_handle,
              fk_owner_account_id = ins_fk_owner_account_id,
              encryption_key = ins_encryption_key,
              is_encrypt_key_set = ins_is_encrypt_key_set,
              expiration_stamp = ins_expiration_stamp,
              handle_status = ins_handle_status
          WHERE pk_handle_id = retrow.fk_handle_id;
    rowsaffected = rowsaffected +1;
     FETCH NEXT FROM hacurse INTO retrow;
  END LOOP;
  CLOSE hacurse;  
  RETURN rowsaffected;
    END;
$BODY$;