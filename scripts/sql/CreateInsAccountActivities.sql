

CREATE OR REPLACE FUNCTION insaccountactivities(  
    fktransactionid bigint,
    fkblocknumber bigint,
    actionaccount varchar(12),
    activitytype varchar(20)
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare actionaccountid bigint;
    BEGIN  
        IF (fkblocknumber< 0) THEN 
            RETURN -1;
        END IF;
        SELECT pk_account_id from accounts 
            WHERE account_name = actionaccount INTO actionaccountid ;
        INSERT INTO accountactivities ( 
            pk_account_activity_id,
             fk_block_number,
             fk_account_id,
            fk_transaction_id,
            activity_type
        ) VALUES ( 
            DEFAULT,
            fkblocknumber,
            actionaccountid,
            fktransactionid,
            activitytype
        ) RETURNING pk_account_activity_id INTO pkid ;
        RETURN pkid;
    END;
$BODY$;