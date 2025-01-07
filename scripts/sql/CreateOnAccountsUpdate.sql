
CREATE OR REPLACE TRIGGER onaccountsupdate
AFTER UPDATE ON accounts
    FOR EACH ROW
    WHEN (OLD.* IS DISTINCT FROM NEW.*)
    EXECUTE FUNCTION logaccountsupdate();
