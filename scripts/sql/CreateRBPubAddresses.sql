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
             SELECT fk_pub_addresses_id from pubaddressesaudit
                WHERE FK_block_number >= bnumber 
             UNION 
             SELECT pk_pub_addresses_id from pubaddresses 
                WHERE fk_block_number >= bnumber
          ) AND fk_block_number < bnumber  GROUP BY  fk_pub_addresses_id;

    retrow RECORD;
    rowsaffected bigint;
    operation varchar;

    BEGIN  
        IF (bnumber< 0) THEN 
            RETURN 0;
        END IF;

 OPEN tcurse;
 rowsaffected = 0;
 FETCH FROM tcurse INTO retrow;
  WHILE FOUND LOOP

   select table_operation INTO operation FROM pubaddressesaudit where 
        pk_pub_addresses_audit_id = retrow.max;

   IF (operation = 'update') THEN
    if NOT EXISTS (select from pubaddresses where pk_pub_addresses_id = retrow.fk_pub_addresses_id) THEN
       INSERT INTO pubaddresses ( 
                fk_block_number,
                fk_handle_id,
                chain_code,
                token_code,
                pub_address
            ) SELECT
                fk_block_number,
                fk_handle_id,
                chain_code,
                token_code,
                pub_address
             FROM pubaddressesaudit WHERE
               fk_pub_addresses_id = retrow.fk_pub_addresses_id;
    ELSE  
     UPDATE pubaddresses
          SET fk_block_number = subquery.fk_block_number,
              fk_handle_id = subquery.fk_handle_id,
              chain_code = subquery.chain_code,
              token_code = subquery.token_code,
              pub_address = subquery.pub_address
          FROM (
                 SELECT * FROM pubaddressesaudit
                    WHERE pk_pub_addresses_audit_id = retrow.max
                ) AS subquery
          WHERE pk_pub_addresses_id = retrow.fk_pub_addresses_id;
    END IF;
   ELSEIF (operation = 'delete') THEN
     DELETE FROM pubaddresses WHERE pk_pub_addresses_id
         = retrow.fk_pub_addresses_id;
   END IF;
   rowsaffected = rowsaffected +1;
     FETCH NEXT FROM tcurse INTO retrow;
  END LOOP;
 

  CLOSE tcurse;  
  RETURN rowsaffected;
    END;
$BODY$;