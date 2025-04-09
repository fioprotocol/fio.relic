CREATE OR REPLACE FUNCTION lognftsignaturesupdate( ) 
RETURNS TRIGGER 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  

  declare blocktimestamp timestamp;
    
    BEGIN  
        SELECT stamp INTO blocktimestamp FROM blocks
        WHERE pk_block_number = OLD.fk_block_number;
   
        INSERT INTO nftsignaturesaudit (
            pk_nft_signatures_audit_id,
            fk_nft_signatures_id,
            fk_block_number,
            fk_table_operation_block_number,
            fk_handle_id,
            chain_code,
            contract_address,
            token_id,
            nft_url,
            nft_hash,
            nft_meta_data,
            table_operation,
            table_timestamp
            ) VALUES (
                DEFAULT,
                OLD.pk_nft_signatures_id,
                OLD.fk_block_number,
                NEW.fk_block_number,
                OLD.fk_handle_id,
                OLD.chain_code,
                OLD.contract_address,
                OLD.token_id,
                OLD.nft_url,
                OLD.nft_hash,
                OLD.nft_meta_data,
                'update',
                blocktimestamp
            );
        RETURN NEW;
    END;
$BODY$;