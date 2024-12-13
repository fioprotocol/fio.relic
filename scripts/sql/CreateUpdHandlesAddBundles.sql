
CREATE OR REPLACE FUNCTION updhandlesaddbundles(
    blocknumber bigint,
    fiohandle varchar(64),
    bundlesets integer
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
            fk_block_number = blocknumber, 
            bundled_tx_count = bundlecnt + (100 * bundlesets)
            WHERE pk_handle_id = pkid;
        RETURN pkid;
    END;
$BODY$;