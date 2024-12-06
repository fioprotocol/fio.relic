CREATE OR REPLACE FUNCTION instransactions(
    blocknumber bigint,
    blocktimestamp timestamp,
    transactionid varchar(80),
    actionaccount varchar(12), --the contract account that contains the action.
    accountname varchar(12), --the account the authorizes the action, the actor in the fio data.
    actionname varchar(12),
    tpidhandle varchar(64),
    feeamount integer,
    requestdata text,
    responsedata text,
    statusvalue varchar(120)
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare actionaccountid bigint;
    declare accountid bigint;
    BEGIN  
        IF (blocknumber< 0) THEN 
            RETURN -1;
        END IF;
        SELECT pk_account_id INTO actionaccountid from accounts 
            WHERE account_name = actionaccount;
            --contract accounts get created very early in genesis.
            --chronicle cannot always deserialize these early events during genesis.
            --so if we dont have them, then just add them to the accounts
            --at the block when we first see them used.
        IF NOT FOUND THEN
              INSERT INTO accounts VALUES (
                DEFAULT,
                blocknumber,
                actionaccount,
                'UNKNOWN',
                blocktimestamp
                )RETURNING pk_account_id INTO actionaccountid ;
        END IF;
        SELECT pk_account_id INTO accountid from accounts 
            WHERE account_name = accountname;
            --contract accounts get created very early in genesis.
            --chronicle cannot always deserialize these early events during genesis.
            --so if we dont have a fio contract account them, then just add them to the accounts
            --at the block when we first see them used.
        IF NOT FOUND THEN
            IF STARTS_WITH(accountname,'fio.') OR
              STARTS_WITH(accountname,'eosio.') THEN 
                INSERT INTO accounts VALUES (
                    DEFAULT,
                    blocknumber,
                    accountname,
                    'UNKNOWN',
                    blocktimestamp
                    )RETURNING pk_account_id INTO accountid ;
            END IF;
        END IF;
        INSERT INTO transactions (
            pk_transaction_id,
            fk_block_number,
            block_timestamp,
            transaction_id,
            fk_action_account_id,
            fk_account_id,
            action_name,
            tpid,
            fee,
            request_data,
            response_data,
            result_status 
        ) VALUES (
            DEFAULT,
            blocknumber,
            blocktimestamp,
            transactionid,
            actionaccountid,
            accountid,
            actionname,
            tpidhandle,
            feeamount,
            requestdata,
            responsedata,
            statusvalue
        ) RETURNING pk_transaction_id INTO pkid ;
        RETURN pkid;
    END;
$BODY$;