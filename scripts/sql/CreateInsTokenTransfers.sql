
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
    declare payerbalance bigint;
    declare payeebalance bigint;
    declare payeeaccountid bigint;
    BEGIN  
        IF (fkblocknumber< 0) THEN 
            RETURN -1;
        END IF;
        SELECT pk_account_id, fio_balance_suf from accounts 
            WHERE account_name = payeraccount INTO payeraccountid, payerbalance ;
        SELECT pk_account_id, fio_balance_suf from accounts 
            WHERE account_name = payeeaccount INTO payeeaccountid, payeebalance ;
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
            '',
            0,
            blocktimestamp
        )RETURNING pk_account_id INTO payeeaccountid;
        payeebalance := 0;
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

 payeebalance := payeebalance + sufamount;
 payerbalance := payerbalance - sufamount;

 if payeebalance > 1000000000000000000 OR payeebalance < 0 THEN
     RAISE EXCEPTION 'out of bounds value computed for payee balance';
 END IF;
 if payerbalance > 1000000000000000000 OR payerbalance < 0 THEN
     RAISE EXCEPTION 'out of bounds value computed for payer balance : ';
 END IF;

 UPDATE  accounts SET 
            fio_balance_suf = payeebalance
 WHERE pk_account_id = payeeaccountid;

 UPDATE  accounts SET 
            fio_balance_suf = payerbalance
 WHERE pk_account_id = payeraccountid;

        RETURN pkid;
    END;
$BODY$;