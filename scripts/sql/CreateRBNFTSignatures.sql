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

        SELECT fk_nft_signatures_id, MIN(pk_nft_signatures_audit_id) FROM nftsignaturesaudit 
          WHERE fk_table_operation_block_number >= bnumber  GROUP BY  fk_nft_signatures_id;


    retrow RECORD;
    rowsaffected bigint;
    operation varchar;
    blockprevop bigint;


    ins_chain_code varchar(12);
    ins_contract_address varchar(128);
    ins_token_id varchar(128);
    ins_nft_url varchar(128);
    ins_nft_hash varchar(64);
    ins_nft_meta_data varchar(128);
    ins_fk_block_number bigint;
    ins_fk_handle_id bigint;

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
                contract_address,
                token_id,
                nft_url,
                nft_hash,
                nft_meta_data
            INTO
                ins_fk_block_number,
                ins_fk_handle_id ,
                ins_chain_code ,
                ins_contract_address ,
                ins_token_id ,
                ins_nft_url ,
                ins_nft_hash ,
                ins_nft_meta_data
             FROM nftsignaturesaudit WHERE
               pk_nft_signatures_audit_id = retrow.min;
    IF NOT FOUND THEN
               RAISE EXCEPTION 'failed to find data required for rollback';
    END IF;
    if NOT EXISTS (select from nftsignatures where pk_nft_signatures_id = retrow.fk_nft_signatures_id) THEN
            INSERT INTO nftsignatures ( 
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
                DEFAULT,
                blockprevop,
                ins_fk_handle_id ,
                ins_chain_code ,
                ins_contract_address ,
                ins_token_id ,
                ins_nft_url,
                ins_nft_hash ,
                ins_nft_meta_data
            );
    ELSE  
           UPDATE nftsignatures
                SET  fk_block_number = ins_fk_block_number,
                     fk_handle_id = ins_fk_handle_id,
                     nft_url = ins_nft_url,
                     nft_hash = ins_nft_hash,
                     nft_meta_data = ins_nft_meta_data
                WHERE pk_nft_signatures_id = retrow.fk_nft_signatures_id;
    END IF;

   rowsaffected = rowsaffected +1;
     FETCH NEXT FROM tcurse INTO retrow;
  END LOOP;
 

  CLOSE tcurse;  
  RETURN rowsaffected;
    END;
$BODY$;