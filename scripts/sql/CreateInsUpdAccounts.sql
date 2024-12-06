CREATE OR REPLACE FUNCTION insupdaccounts( 
    fkblocknumber  bigint,
    accountname    varchar(12),
    publickey      varchar(53),
    blocktimestamp timestamp 
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
        INSERT INTO accounts VALUES (
            DEFAULT,
            fkblocknumber,
            accountname,
            publickey,
            blocktimestamp
        ) RETURNING pk_account_id INTO pkid ;
    ELSE
     UPDATE  accounts SET 
            fk_block_number = fkblocknumber,
            public_key = publickey,
            block_timestamp = blocktimestamp
        WHERE pk_account_id = pkid;

    END IF;
        RETURN pkid;
    END;
$BODY$;