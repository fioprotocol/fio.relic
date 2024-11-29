CREATE OR REPLACE FUNCTION instraces(  
    fktransactionid bigint,
    fkblocknumber bigint,
    actionaccount varchar(12),
    receiveraccountname varchar(12),
    actionordinal int,
    actionname varchar(12),
    requestdata text,
    blocktimestamp timestamp
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare actionaccountid bigint;
    declare accountid bigint;
    BEGIN  
        IF (fkblocknumber< 0) THEN 
            RETURN -1;
        END IF;
        SELECT pk_account_id from accounts 
            WHERE account_name = actionaccount INTO actionaccountid ;
            --contract accounts get created very early in genesis.
            --chronicle cannot always deserialize these early events during genesis.
            --so if we dont have them, then just add them to the accounts
            --at the block when we first see them used.
        IF NOT FOUND THEN
              INSERT INTO accounts VALUES (
                DEFAULT,
                fkblocknumber,
                actionaccount,
                'UNKNOWN',
                blocktimestamp
                )RETURNING pk_account_id INTO actionaccountid ;
        END IF;
        SELECT pk_account_id from accounts 
            WHERE account_name = receiveraccountname INTO accountid ;
             --contract accounts get created very early in genesis.
            --chronicle cannot always deserialize these early events during genesis.
            --so if we dont have a fio contract account them, then just add them to the accounts
            --at the block when we first see them used.
        IF NOT FOUND THEN
            IF STARTS_WITH(receiveraccountname,'fio.')  OR
            STARTS_WITH(receiveraccountname,'eosio.') THEN 
              INSERT INTO accounts VALUES (
                DEFAULT,
                fkblocknumber,
                receiveraccountname,
                'UNKNOWN',
                blocktimestamp
                )RETURNING pk_account_id INTO accountid ;
            END IF;
        END IF;
        INSERT INTO traces ( 
            pk_trace_id,
            fk_transaction_id,
            fk_block_number,
            fk_action_account_id,
            fk_receiver_account_id,
            action_ordinal,
            action_name,
            request_data
        ) VALUES ( 
            DEFAULT,
            fktransactionid,
            fkblocknumber,
            actionaccountid,
            accountId,
            actionordinal,
            actionname,
            requestdata
        ) RETURNING pk_trace_id INTO pkid ;
        RETURN pkid;
    END;
$BODY$;