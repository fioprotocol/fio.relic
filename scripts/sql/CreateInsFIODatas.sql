

CREATE OR REPLACE FUNCTION insfiodatas(  
    fktransactionid bigint,
    fkblocknumber bigint,
    fkchainrequestid bigint,
    payerhandle varchar(64),
    payeehandle varchar(64),
    binarycontent bytea,
    requeststatus varchar(20),
    blocktimestamp timestamp
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare payerhandleid bigint;
    declare payeraccountid bigint;
    declare payeeaccountid bigint;
    declare payeehandleid bigint;
    declare payerpubkey varchar(64);
    declare payeepubkey varchar(64);
    BEGIN  
        IF (fkblocknumber< 0) THEN 
            RETURN -1;
        END IF;
        SELECT pk_handle_id, fk_owner_account_id 
            INTO payerhandleid, payeraccountid from handles 
            WHERE handle = payerhandle ;
        SELECT pk_handle_id, fk_owner_account_id 
            INTO payeehandleid, payeeaccountid from handles 
            WHERE handle = payeehandle ;
        SELECT public_key INTO payerpubkey from accounts 
            WHERE pk_account_id = payeraccountid;
        SELECT public_key INTO payeepubkey from accounts 
            WHERE pk_account_id = payeeaccountid;

      
        INSERT INTO fiodatas ( 
            pk_fio_data_id,
             fk_block_number,
             fio_chain_request_id,
            fk_payer_handle_id,
            fk_payee_handle_id,
            binary_content,
            payer_pub_key,
            payee_pub_key,
            data_status,
            fk_transaction_id,
            block_timestamp
        ) VALUES ( 
            DEFAULT,
            fkblocknumber,
            fkchainrequestid,
            payerhandleid,
            payeehandleid,
            binarycontent,
            payerpubkey,
            payeepubkey,
            requeststatus,
            fktransactionid,
            blocktimestamp
        ) RETURNING pk_fio_data_id INTO pkid ;
        RETURN pkid;
    END;
$BODY$;