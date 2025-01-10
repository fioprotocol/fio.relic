
CREATE OR REPLACE TRIGGER onnftsignaturesupdate
AFTER UPDATE ON nftsignatures
    FOR EACH ROW
     WHEN (OLD.* IS DISTINCT FROM NEW.*)
    EXECUTE FUNCTION lognftsignaturesupdate();
