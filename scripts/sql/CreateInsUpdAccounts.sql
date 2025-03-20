CREATE OR REPLACE FUNCTION insupdaccounts( 
    fkblocknumber  bigint,
    accountname    varchar(12),
    publickey      varchar(53),
    blocktimestamp timestamp,
    doupdates      boolean
) RETURNS int 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    BEGIN  
    SELECT pk_account_id INTO pkid FROM accounts
         WHERE account_name = accountname; 
    IF NOT FOUND THEN
       IF accountname = 'fio.token' THEN
         INSERT INTO accounts VALUES (
            DEFAULT,
            fkblocknumber,
            accountname,
            publickey,
            1000000000000000000,
            blocktimestamp
        ) RETURNING pk_account_id INTO pkid ;
       ELSE
        INSERT INTO accounts VALUES (
            DEFAULT,
            fkblocknumber,
            accountname,
            publickey,
            0,
            blocktimestamp
        ) RETURNING pk_account_id INTO pkid ;
        END IF;
    ELSE
     IF (doupdates) THEN
       UPDATE  accounts SET 
            fk_block_number = fkblocknumber,
            public_key = publickey,
            block_timestamp = blocktimestamp
        WHERE pk_account_id = pkid;
     END IF;
    END IF;
        RETURN pkid;
    END;
$BODY$;