
CREATE OR REPLACE TRIGGER onnftsignaturesdelete
AFTER DELETE ON nftsignatures
    FOR EACH ROW
    EXECUTE FUNCTION lognftsignaturesdelete();
