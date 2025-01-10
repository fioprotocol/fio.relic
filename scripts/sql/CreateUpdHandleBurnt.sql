CREATE OR REPLACE FUNCTION updhandleburnt( 
    blocknumber bigint,
    fiohandle    varchar(64),
    bundledtxcount integer,
    expirationtimestamp timestamp
     
) RETURNS int 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare handleexpire timestamp;
    declare handlebundlecount integer;
    
    BEGIN  
    SELECT pk_handle_id, bundled_tx_count, expiration_timestamp
         INTO pkid,handlebundlecount,handleexpire FROM handles 
         WHERE handle = fiohandle; 

    IF FOUND THEN
       IF (
          bundledtxcount = handlebundlecount AND
          expirationtimestamp = handleexpire) THEN
          UPDATE handles SET
                 fk_block_number = blocknumber,
                 handle_status = 'burnt'
                 WHERE pk_handle_id = pkid;
            RETURN 1;
        ELSE
            RETURN 0;
        END IF;
    END IF;
    END;
$BODY$;