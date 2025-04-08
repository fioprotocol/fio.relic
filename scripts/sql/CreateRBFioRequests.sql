CREATE OR REPLACE FUNCTION rbfiorequests( 
    bnumber bigint
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE    
AS 
$BODY$  


DECLARE 
    tcurse CURSOR FOR 
        SELECT fk_fio_requests_id, MIN(pk_fio_requests_audit_id) FROM fiorequestsaudit 
          WHERE fk_table_operation_block_number >= bnumber  GROUP BY  fk_fio_requests_id;


    retrow RECORD;
    rowsaffected bigint;

    ins_pk_fio_requests_audit_id bigint;
    ins_fk_fio_requests_id bigint ;
    ins_fk_block_number bigint;
    ins_fk_table_operation_block_number bigint ;
    ins_fio_chain_request_id bigint;
    ins_fk_payer_handle_id bigint ;
    ins_fk_payee_handle_id bigint ;
    ins_binary_content bytea;
    ins_payer_pub_key varchar(53);
    ins_payee_pub_key varchar(53);
    ins_request_status varchar(20);
    ins_fk_transaction_id bigint;
    ins_block_timestamp timestamp;
    ins_table_operation varchar(20);
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
    pk_fio_requests_audit_id,
    fk_fio_requests_id,
    fk_block_number,
    fk_table_operation_block_number,
    fio_chain_request_id,
    fk_payer_handle_id,
    fk_payee_handle_id ,
    binary_content,
    payer_pub_key,
    payee_pub_key,
    request_status ,
    fk_transaction_id ,
    block_timestamp,
    table_operation,
    table_timestamp

 INTO
    ins_pk_fio_requests_audit_id,
    ins_fk_fio_requests_id,
    ins_fk_block_number,
    ins_fk_table_operation_block_number,
    ins_fio_chain_request_id,
    ins_fk_payer_handle_id,
    ins_fk_payee_handle_id ,
    ins_binary_content,
    ins_payer_pub_key,
    ins_payee_pub_key,
    ins_request_status ,
    ins_fk_transaction_id ,
    ins_block_timestamp,
    ins_table_operation,
    ins_table_timestamp
    FROM fiorequestsaudit WHERE
               pk_fio_requests_audit_id = retrow.min;

        IF NOT FOUND THEN
               RAISE EXCEPTION 'failed to find data required for rollback';
        END IF;

     IF NOT (ins_table_operation = 'update') THEN
         RAISE 'rollback of operations other than updates not supported on audit table';
     END IF;



     UPDATE fiorequests
          SET fk_block_number = ins_fk_block_number,
              fio_chain_request_id = ins_fio_chain_request_id,
              fk_payer_handle_id = ins_fk_payer_handle_id,
              fk_payee_handle_id = ins_fk_payee_handle_id,
              binary_content = ins_binary_content,
              payer_pub_key = ins_payer_pub_key,
              payee_pub_key = ins_payee_pub_key,
              request_status = ins_request_status,
              fk_transaction_id = ins_fk_transaction_id,
              block_timestamp = ins_block_timestamp
          WHERE pk_fio_requests_id = retrow.fk_fio_requests_id;
    rowsaffected = rowsaffected +1;
     FETCH NEXT FROM tcurse INTO retrow;
  END LOOP;
  CLOSE tcurse;  
  RETURN rowsaffected;
    END;
$BODY$;