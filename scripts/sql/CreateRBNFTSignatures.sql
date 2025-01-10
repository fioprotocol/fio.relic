CREATE OR REPLACE FUNCTION rbnftsignatures( 
    bnumber bigint
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE    
AS 
$BODY$  


DECLARE 
    tcurse CURSOR FOR 
        SELECT fk_nft_signatures_id, MAX(pk_nft_signatures_audit_id) FROM nftsignaturesaudit 
          WHERE fk_nft_signatures_id IN (
              SELECT pk_nft_signatures_id FROM nftsignatures 
                WHERE fk_block_number >= bnumber AND EXISTS (
                  SELECT fk_nft_signatures_id FROM nftsignaturesaudit 
                      WHERE nftsignatures.pk_nft_signatures_id = nftsignaturesaudit.fk_nft_signatures_id
                )
          ) AND fk_block_number < bnumber  GROUP BY  fk_nft_signatures_id;
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
        
     UPDATE nftsignatures
          SET  fk_block_number = subquery.fk_block_number,
               fk_handle_id = subquery.fk_handle_id,
               nft_url = subquery.nft_url,
               nft_hash = subquery.nft_hash,
               nft_meta_data = subquery.nft_meta_data
          FROM (
                 SELECT * FROM nftsignaturesaudit
                    WHERE pk_nft_signatures_audit_id = retrow.max
                ) AS subquery
          WHERE pk_nft_signatures_id = retrow.fk_nft_signatures_id;
    
   ELSEIF (retrow.table_operation = 'delete') THEN
    INSERT INTO nft_signatures ( 
                 pk_nft_signatures_id,
                fk_block_number,
                fk_handle_id,
                chain_code,
                contract_address,
                token_id,
                nft_url,
                nft_hash,
                nft_meta_data
            ) VALUES ( 
                subquery.pk_pub_addresses_id,
                subquery.fk_block_number,
                subquery.fk_handle_id,
                subquery.chain_code,
                subquery.token_code,
                subquery.pub_address
            );
   END IF;
   rowsaffected = rowsaffected +1;
     FETCH NEXT FROM tcurse INTO retrow;
  END LOOP;
 

  CLOSE tcurse;  
  RETURN rowsaffected;
    END;
$BODY$;