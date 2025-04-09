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

    SELECT fk_pub_addresses_id, MIN(pk_pub_addresses_audit_id) FROM pubaddressesaudit 
          WHERE fk_table_operation_block_number >= bnumber  GROUP BY  fk_pub_addresses_id;

    retrow RECORD;
    rowsaffected bigint;

    ins_chain_code varchar(12);
    ins_token_code varchar(12);
    ins_fk_block_number bigint;
    ins_fk_handle_id bigint;
    ins_pub_address varchar(128);

    BEGIN  
        IF (bnumber< 0) THEN 
            RETURN 0;
        END IF;

 OPEN tcurse;
 rowsaffected = 0;
 FETCH FROM tcurse INTO retrow;
  WHILE FOUND LOOP
    SELECT
                fk_block_number,
                fk_handle_id,
                chain_code,
                token_code,
                pub_address
            INTO
                ins_fk_block_number,
                ins_fk_handle_id ,
                ins_chain_code ,
                ins_token_code,
                ins_pub_address
             FROM pubaddressesaudit WHERE
               pk_pub_addresses_audit_id = retrow.min;
    IF NOT FOUND THEN
               RAISE EXCEPTION 'failed to find data required for rollback';
    END IF;
    if NOT EXISTS (select from pubaddresses where pk_pub_addresses_id = retrow.fk_pub_addresses_id) THEN
            INSERT INTO pubaddresses ( 
                pk_pub_addresses_id,
                fk_block_number,
                fk_handle_id,
                chain_code,
                token_code,
                pub_address
            ) VALUES (
                DEFAULT,
                ins_fk_block_number,
                ins_fk_handle_id ,
                ins_chain_code ,
                ins_token_code ,
                ins_pub_address 
            );
    ELSE  
     UPDATE pubaddresses
          SET fk_block_number = ins_fk_block_number,
              fk_handle_id = ins_fk_handle_id,
              chain_code = ins_chain_code,
              token_code = ins_token_code,
              pub_address = ins_pub_address
          WHERE pk_pub_addresses_id = retrow.fk_pub_addresses_id;
    END IF;

   rowsaffected = rowsaffected +1;
     FETCH NEXT FROM tcurse INTO retrow;
  END LOOP;
 

  CLOSE tcurse;  
  RETURN rowsaffected;
    END;
$BODY$;