CREATE OR REPLACE FUNCTION insblocks( 
    bnumber bigint,
    btimestamp text,
    blockid  text,
    produceraccountname varchar(12),
    scheduleversion text
) RETURNS int
LANGUAGE 'plpgsql' 
COST 100 
VOLATILE AS 
$BODY$  
    BEGIN  
        IF (bnumber< 0) THEN 
            RETURN 0;
        END IF;
        INSERT INTO blocks VALUES (
            bnumber,
            btimestamp::timestamp,
            blockid,
            produceraccountname,
            scheduleversion
            );
        RETURN 1;
    END;
$BODY$;