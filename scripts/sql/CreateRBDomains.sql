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
        SELECT fk_domain_id, MAX(pk_domains_audit_id) FROM domainsaudit 
          WHERE fk_domain_id IN (
              SELECT pk_domain_id FROM domains 
                WHERE fk_block_number >= bnumber AND EXISTS (
                  SELECT fk_domain_id FROM domainsaudit 
                      WHERE domains.pk_domain_id = domainsaudit.fk_domain_id
                )
          ) AND fk_block_number < bnumber  GROUP BY  fk_domain_id;
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
     UPDATE domains
          SET fk_block_number = subquery.fk_block_number,
              domain_name = subquery.domain_name,
              handle = subquery.handle,
              fk_owner_account_id = subquery.fk_owner_account_id,
              is_public = subquery.is_public,
              expiration_timestamp = subquery.expiration_timestamp,
              domain_status = subquery.domain_status
          FROM (
                 SELECT * FROM domainsaudit
                    WHERE pk_domains_audit_id = retrow.max
                ) AS subquery
          WHERE pk_domain_id = retrow.fk_domain_id;
    rowsaffected = rowsaffected +1;
     FETCH NEXT FROM tcurse INTO retrow;
  END LOOP;
  CLOSE tcurse;  
  RETURN rowsaffected;
    END;
$BODY$;