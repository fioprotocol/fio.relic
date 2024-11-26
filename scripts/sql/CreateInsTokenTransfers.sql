
CREATE OR REPLACE FUNCTION instokentransfers(  
    fktransactionid bigint,
    fkblocknumber bigint,
    payeraccount varchar(12),
    payeeaccount varchar(12),
    sufamount bigint,
    tokentransfertype varchar(20),
    transfermemo text,
    blocktimestamp timestamp
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare payeraccountid bigint;
    declare payeeaccountid bigint;
    BEGIN  
        IF (fkblocknumber< 0) THEN 
            RETURN -1;
        END IF;
        SELECT pk_account_id from accounts 
            WHERE account_name = payeraccount INTO payeraccountid ;
        SELECT pk_account_id from accounts 
            WHERE account_name = payeeaccount INTO payeeaccountid;
            /*fio creates accounts as part of token transfers, 
              so if the account is not found, then we must assume it will
              be created during the actions that make up this tx...
              we dont do validation here, just create the account cuz
              history node has integrity. pub key of the account will be
              updated while processing the actions in this tx.
              */
      IF NOT FOUND THEN
        INSERT INTO accounts VALUES (
            DEFAULT,
            fkblocknumber,
            payeeaccount,
            'UNKNOWN',
            blocktimestamp
        )RETURNING pk_account_id INTO payeeaccountid;
      END IF;
       
        INSERT INTO tokentransfers ( 
            pk_token_transfers_id, 
            fk_block_number, 
            fk_payer_account_id, 
            fk_payee_account_id, 
            fk_transaction_id, 
            fio_suf_amount,
            token_transfer_type, 
            transfer_memo, 
            block_timestamp 
        ) VALUES ( 
            DEFAULT,
            fkblocknumber,
            payeraccountid,
            payeeaccountid,
            fktransactionid,
            sufamount,
            tokentransfertype,
            transfermemo,
            blocktimestamp
        ) RETURNING pk_token_transfers_id INTO pkid ;
        RETURN pkid;
    END;
$BODY$;