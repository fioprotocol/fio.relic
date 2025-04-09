CREATE OR REPLACE FUNCTION rbfork( 
    bnumber bigint
) RETURNS int     
LANGUAGE 'plpgsql'   
COST 100 
VOLATILE    
AS 
$BODY$  
declare auditid bigint;
    BEGIN  
        IF (bnumber< 0) THEN 
            RETURN 0;
        END IF;

       PERFORM rbhandles(bnumber);
       PERFORM rbdomains(bnumber);
       PERFORM rbaccounts(bnumber);
       PERFORM rbfiorequests(bnumber);
       PERFORM rbfiodatas(bnumber);
       PERFORM rbpubaddresses(bnumber);
       PERFORM rbnftsignatures(bnumber);
       
       DELETE FROM fiodatasaudit WHERE fk_block_number >= bnumber;
       DELETE FROM fiodatasaudit WHERE fk_table_operation_block_number >= bnumber;
      
       DELETE FROM fiodatas WHERE  fk_block_number >= bnumber;
       DELETE FROM fiorequestsaudit WHERE fk_block_number >= bnumber;
       DELETE FROM fiorequestsaudit WHERE fk_table_operation_block_number >= bnumber;
      
       DELETE FROM fiorequests WHERE  fk_block_number >= bnumber;
       DELETE FROM handleactivities WHERE fk_block_number >= bnumber;
       DELETE FROM pubaddresses WHERE  fk_block_number >= bnumber;
       --though these next 2 deletes may not seem necessary, postgres needs it to clean up fk constraints properly.
       DELETE FROM pubaddressesaudit where fk_block_number >= bnumber;
       DELETE FROM pubaddressesaudit WHERE fk_table_operation_block_number >= bnumber;
       DELETE FROM handlesaudit WHERE fk_block_number  >= bnumber;
       DELETE FROM handlesaudit WHERE fk_table_operation_block_number >= bnumber;
      
       DELETE FROM handles WHERE  fk_block_number  >= bnumber;
       DELETE FROM domainactivities WHERE fk_block_number >= bnumber;
       DELETE FROM domainsaudit WHERE  fk_block_number  >= bnumber;
       DELETE FROM domainsaudit WHERE fk_table_operation_block_number >= bnumber;
       DELETE FROM domains WHERE  fk_block_number >= bnumber;
        DELETE FROM traces WHERE  fk_block_number >= bnumber;
        DELETE FROM tokenstakings WHERE fk_block_number >= bnumber;
        DELETE FROM accountactivities WHERE fk_block_number >= bnumber;
        DELETE FROM accountsaudit WHERE  fk_block_number  >= bnumber;
        DELETE FROM accountsaudit WHERE fk_table_operation_block_number >= bnumber;
        DELETE FROM tokentransfers WHERE fk_block_number >= bnumber;
        DELETE FROM transactions WHERE  fk_block_number >= bnumber;
        DELETE FROM accounts WHERE fk_block_number  >= bnumber;
         DELETE FROM nftsignatures WHERE  fk_block_number >= bnumber;
       DELETE FROM nftsignaturesaudit WHERE fk_block_number >= bnumber;
        DELETE FROM nftsignaturesaudit WHERE fk_table_operation_block_number >= bnumber;
     
       DELETE FROM blocks where blocks.pk_block_number >= bnumber; 
      
        RETURN 1;
    END;
$BODY$;