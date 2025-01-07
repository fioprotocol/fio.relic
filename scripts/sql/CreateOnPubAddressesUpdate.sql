
CREATE OR REPLACE TRIGGER onpubaddressesupdate
AFTER UPDATE ON pubaddresses
    FOR EACH ROW
    WHEN (OLD.* IS DISTINCT FROM NEW.*)
    EXECUTE FUNCTION logpubaddressesupdate();
