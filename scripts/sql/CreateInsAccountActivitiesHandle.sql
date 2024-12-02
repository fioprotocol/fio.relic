

CREATE OR REPLACE FUNCTION insaccountactivitieshandle(  
    fktransactionid bigint,
    fkblocknumber bigint,
    fiohandle varchar(64),
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
        SELECT fk_owner_account_id from handles 
            WHERE handle = fiohandle INTO actionaccountid ;
            
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