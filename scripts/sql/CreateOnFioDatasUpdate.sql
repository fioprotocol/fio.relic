
CREATE OR REPLACE TRIGGER onfiodatasupdate
AFTER UPDATE ON fiodatas
    FOR EACH ROW
    WHEN (OLD.* IS DISTINCT FROM NEW.*)
    EXECUTE FUNCTION logfiodatasupdate();
