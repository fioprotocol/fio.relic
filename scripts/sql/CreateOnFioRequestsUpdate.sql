
CREATE OR REPLACE TRIGGER onfiorequestsupdate
AFTER UPDATE ON fiorequests
    FOR EACH ROW
    WHEN (OLD.* IS DISTINCT FROM NEW.*)
    EXECUTE FUNCTION logfiorequestsupdate();
