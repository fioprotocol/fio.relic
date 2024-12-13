/* this procedure will remove all records in the DB with block number >= the specified block number */
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
        SELECT fk_handle_id, MAX(pk_handles_audit_id) FROM handlesaudit 
          WHERE fk_handle_id IN (
              SELECT pk_handle_id FROM handles 
                WHERE fk_block_number >= bnumber AND EXISTS (
                  SELECT fk_handle_id FROM handlesaudit 
                      WHERE handles.pk_handle_id = handlesaudit.fk_handle_id
                )
          ) AND fk_block_number < bnumber  GROUP BY  fk_handle_id;
    retrow RECORD;
    rowsaffected bigint;

    BEGIN  
        IF (bnumber< 0) THEN 
            RETURN 0;
        END IF;

 OPEN hacurse;
 rowsaffected = 0;
 FETCH FROM hacurse INTO retrow;
  WHILE FOUND LOOP
     UPDATE handles
          SET fk_block_number = subquery.fk_block_number,
              fk_domain_id = subquery.fk_domain_id,
              handle = subquery.handle,
              fk_owner_account_id = subquery.fk_owner_account_id,
              encryption_key = subquery.encryption_key,
              is_encrypt_key_set = subquery.is_encrypt_key_set,
              bundled_tx_count = subquery.bundled_tx_count,
              expiration_stamp = subquery.expiration_stamp,
              handle_status = subquery.handle_status
          FROM (
                 SELECT * FROM handlesaudit
                    WHERE pk_handles_audit_id = retrow.max
                ) AS subquery
          WHERE pk_handle_id = retrow.fk_handle_id;
    rowsaffected = rowsaffected +1;
     FETCH NEXT FROM hacurse INTO retrow;
  END LOOP;
  CLOSE hacurse;  
  RETURN rowsaffected;
    END;
$BODY$;