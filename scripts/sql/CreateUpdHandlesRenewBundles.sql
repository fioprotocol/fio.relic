
CREATE OR REPLACE FUNCTION updhandlesrenewbundles(
    fiohandle varchar(64),
    expirationtimestamp timestamp
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE AS 
$BODY$  
    declare pkid bigint;
    declare bundlecnt integer;

    BEGIN  
        SELECT pk_handle_id, bundled_tx_count INTO pkid, bundlecnt from handles 
            WHERE handle = fiohandle;

        UPDATE  handles SET 
            bundled_tx_count = bundlecnt + 100,
            expiration_stamp = expirationtimestamp
            WHERE pk_handle_id = pkid;
        RETURN pkid;
    END;
$BODY$;