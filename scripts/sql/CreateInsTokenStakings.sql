
CREATE OR REPLACE FUNCTION instokenstakings(  
    fktransactionid bigint,
    fkblocknumber bigint,
    stakingaccount varchar(12),
    sufamount bigint
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare stakingaccountid bigint;

    BEGIN  
        IF (fkblocknumber< 0) THEN 
            RETURN -1;
        END IF;
        SELECT pk_account_id from accounts 
            WHERE account_name = stakingaccount INTO stakingaccountid ;
       
        INSERT INTO tokenstakings ( 
            pk_token_staking_id, 
            fk_block_number, 
            fk_staker_account_id,  
            fk_transaction_id, 
            fio_suf_amount
        ) VALUES ( 
            DEFAULT,
            fkblocknumber,
            stakingaccountid,
            fktransactionid,
            sufamount
        ) RETURNING pk_token_staking_id INTO pkid ;
        RETURN pkid;
    END;
$BODY$;