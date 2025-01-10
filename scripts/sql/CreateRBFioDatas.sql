CREATE OR REPLACE FUNCTION rbfiodatas( 
    bnumber bigint
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE    
AS 
$BODY$  


DECLARE 
    tcurse CURSOR FOR 
        SELECT fk_fio_datas_id, MAX(pk_fio_datas_audit_id) FROM fiodatasaudit 
          WHERE fk_fio_datas_id IN (
              SELECT pk_fio_data_id FROM fiodatas 
                WHERE fk_block_number >= bnumber AND EXISTS (
                  SELECT fk_fio_datas_id FROM fiodatasaudit 
                      WHERE fiodatas.pk_fio_data_id = fiodatasaudit.fk_fio_datas_id
                )
          ) AND fk_block_number < bnumber  GROUP BY  fk_fio_datas_id;
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
     UPDATE fiodatas
          SET fk_block_number = subquery.fk_block_number,
              fio_chain_request_id = subquery.fio_chain_request_id,
              fk_payer_handle_id = subquery.fk_payer_handle_id,
              fk_payee_handle_id = subquery.fk_payee_handle_id,
              binary_content = subquery.binary_content,
              payer_pub_key = subquery.payer_pub_key,
              payee_pub_key = subquery.payee_pub_key,
              request_status = subquery.request_status,
              fk_transaction_id = subquery.fk_transaction_id,
              block_timestamp = subquery.block_timestamp
          FROM (
                 SELECT * FROM fiodatasaudit
                    WHERE pk_fio_datas_audit_id = retrow.max
                ) AS subquery
          WHERE pk_fio_data_id = retrow.fk_fio_datas_id;
    rowsaffected = rowsaffected +1;
     FETCH NEXT FROM tcurse INTO retrow;
  END LOOP;
  CLOSE tcurse;  
  RETURN rowsaffected;
    END;
$BODY$;