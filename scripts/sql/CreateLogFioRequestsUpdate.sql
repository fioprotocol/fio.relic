CREATE OR REPLACE FUNCTION logfiorequestsupdate( ) 
RETURNS TRIGGER 
LANGUAGE 'plpgsql'
COST 100 
VOLATILE AS 
$BODY$  

  declare blocktimestamp timestamp;
    BEGIN  
        SELECT stamp INTO blocktimestamp FROM blocks
        WHERE pk_block_number = OLD.fk_block_number;
        INSERT INTO fiorequestsaudit (
            pk_fio_requests_audit_id,
            fk_fio_requests_id,
            fk_block_number,
            fio_chain_request_id,
            fk_payer_handle_id,
            fk_payee_handle_id,
            request_status,
            fk_transaction_id,
            block_timestamp,
            table_operation,
            table_timestamp
            ) VALUES (
                DEFAULT,
                OLD.pk_fio_requests_id,
                OLD.fk_block_number,
                OLD.fio_chain_request_id,
                OLD.fk_payer_handle_id,
                OLD.fk_payee_handle_id,
                OLD.request_status,
                OLD.fk_transaction_id,
                OLD.block_timestamp,
                'update',
                blocktimestamp
            );
        RETURN NEW;
    END;
$BODY$;