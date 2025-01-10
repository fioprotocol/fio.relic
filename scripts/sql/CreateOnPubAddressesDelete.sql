
CREATE OR REPLACE TRIGGER onpubaddressesdelete
AFTER DELETE ON pubaddresses
    FOR EACH ROW
    EXECUTE FUNCTION logpubaddressesdelete();
