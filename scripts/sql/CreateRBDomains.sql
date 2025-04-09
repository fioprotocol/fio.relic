CREATE OR REPLACE FUNCTION rbdomains( 
    bnumber bigint
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE    
AS 
$BODY$  


DECLARE 
    tcurse CURSOR FOR 
         SELECT fk_domain_id, MIN(pk_domains_audit_id) FROM domainsaudit 
          WHERE fk_table_operation_block_number >= bnumber  GROUP BY  fk_domain_id;

    retrow RECORD;
    rowsaffected bigint;

    ins_pk_domains_audit_id bigint;
     ins_fk_domain_id bigint ;
     ins_fk_block_number bigint;
     ins_fk_table_operation_block_number bigint;
     ins_domain_name varchar(62);
     ins_fk_owner_account_id bigint ;
     ins_is_public boolean;
     ins_expiration_timestamp timestamp;
     ins_domain_status varchar(20);
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
        fk_domain_id ,
        fk_block_number ,
        fk_table_operation_block_number,
        domain_name ,
        fk_owner_account_id ,
        is_public ,
        expiration_timestamp ,
        domain_status ,
        table_operation ,
        table_timestamp 

    INTO
        ins_fk_domain_id ,
        ins_fk_block_number ,
        ins_fk_table_operation_block_number,
        ins_domain_name ,
        ins_fk_owner_account_id ,
        ins_is_public ,
        ins_expiration_timestamp ,
        ins_domain_status ,
        ins_table_operation ,
        ins_table_timestamp 
    FROM domainsaudit WHERE
               pk_domains_audit_id = retrow.min;

        IF NOT FOUND THEN
               RAISE EXCEPTION 'failed to find data required for rollback';
        END IF;

     IF NOT (ins_table_operation = 'update') THEN
         RAISE 'rollback of operations other than updates not supported on audit table';
     END IF;

     UPDATE domains
          SET fk_block_number = ins_fk_block_number,
              domain_name = ins_domain_name,
              fk_owner_account_id = ins_fk_owner_account_id,
              is_public = ins_is_public,
              expiration_timestamp = ins_expiration_timestamp,
              domain_status = ins_domain_status
          WHERE pk_domain_id = retrow.fk_domain_id;

    rowsaffected = rowsaffected +1;
     FETCH NEXT FROM tcurse INTO retrow;
  END LOOP;
  CLOSE tcurse;  
  RETURN rowsaffected;
    END;
$BODY$;