
CREATE OR REPLACE FUNCTION insupdnftsignatures(  
    fkblocknumber bigint,
    fiohandle varchar(64),
    chaincode varchar(12),
    contractaddress varchar(128),
    tokenid varchar(64),
    nfturl varchar(128),
    nfthash varchar(64),
    nftmetadata varchar(128)
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare handleid bigint;
    BEGIN  
        IF (fkblocknumber< 0) THEN 
            RETURN -1;
        END IF;
        SELECT pk_handle_id from handles 
            WHERE handle = fiohandle INTO handleid;

        SELECT pk_nft_signatures_id INTO pkid FROM nftsignatures
         WHERE fk_handle_id = handleid AND 
               chain_code = chaincode AND 
               contract_address = contractaddress AND
               token_id = tokenid ; 

         IF NOT FOUND THEN
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
                fkblocknumber,
                handleid,
                chaincode,
                contractaddress,
                tokenid,
                nfturl,
                nfthash,
                nftmetadata
            ) RETURNING pk_nft_signatures_id INTO pkid ;
        ELSE
            UPDATE nftsignatures SET 
               fk_block_number = fkblocknumber,
               fk_handle_id = handleid,
               nft_url = nfturl,
               nft_hash = nfthash,
               nft_meta_data = nftmetadata
            WHERE pk_nft_signatures_id = pkid;
        END IF;
        RETURN pkid;
    END;
$BODY$;