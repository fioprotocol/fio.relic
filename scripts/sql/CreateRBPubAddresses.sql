CREATE OR REPLACE FUNCTION rbpubaddresses( 
    bnumber bigint
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE    
AS 
$BODY$  


DECLARE 
    tcurse CURSOR FOR 
        SELECT fk_pub_addresses_id, MAX(pk_pub_addresses_audit_id) FROM pubaddressesaudit 
          WHERE fk_pub_addresses_id IN (
              SELECT pk_pub_addresses_id FROM pubaddresses 
                WHERE fk_block_number >= bnumber AND EXISTS (
                  SELECT fk_pub_addresses_id FROM pubaddressesaudit 
                      WHERE pubaddresses.pk_pub_addresses_id = pubaddressesaudit.fk_pub_addresses_id
                )
          ) AND fk_block_number < bnumber  GROUP BY  fk_pub_addresses_id;
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
   IF (retrow.table_operation = 'update') THEN
        
     UPDATE pubaddresses
          SET fk_block_number = subquery.fk_block_number,
              fk_handle_id = subquery.fk_handle_id,
              handle = subquery.handle,
              chain_code = subquery.chain_code,
              token_code = subquery.token_code,
              pub_address = subquery.pub_address
          FROM (
                 SELECT * FROM pubaddressesaudit
                    WHERE pk_pub_addresses_audit_id = retrow.max
                ) AS subquery
          WHERE pk_pub_addresses_id = retrow.fk_pub_addresses_id;
    
   ELSEIF (retrow.table_operation = 'delete') THEN
    INSERT INTO pubaddresses ( 
                pk_pub_addresses_id,
                fk_block_number,
                fk_handle_id,
                chain_code,
                token_code,
                pub_address
            ) VALUES ( 
                retrow.fk_pub_addresses_id,
                retrow.fk_block_number,
                retrow.fk_handle_id,
                retrow.chain_code,
                retrow.token_code,
                retrow.pub_address
            );
   END IF;
   rowsaffected = rowsaffected +1;
     FETCH NEXT FROM tcurse INTO retrow;
  END LOOP;
 

  CLOSE tcurse;  
  RETURN rowsaffected;
    END;
$BODY$;