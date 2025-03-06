/* TODO to be completed during dev!!!
DROP FUNCTION insfiodatas;
DROP FUNCTION insfiorequests;
DROP FUNCTION insnftsignatures;
DROP FUNCTION inspubaddresses;
DROP FUNCTION inshandleactivities;
DROP FUNCTION inshandles;


*/ 
DROP FUNCTION updhandleburnt;
DROP FUNCTION existspubaddress;
DROP FUNCTION upddomainburnt;
DROP FUNCTION insfiodatas;
DROP FUNCTION updfiorequestsstatus;
DROP FUNCTION insfiorequests;
DROP FUNCTION insaccountactivitieshandle;
DROP FUNCTION updhandlesstatus;
DROP FUNCTION updhandlessetencryptkey;
DROP FUNCTION delnftsignature;
DROP FUNCTION insupdnftsignatures;
DROP FUNCTION delpubaddress;
DROP FUNCTION insupdpubaddresses;
DROP FUNCTION updhandlesaddbundles;
DROP FUNCTION updhandlesencryptkey;
DROP FUNCTION updhandlesxferowner;
DROP FUNCTION updhandlesrenewbundles;
DROP FUNCTION delnftsignatures;
DROP FUNCTION delpubaddresses;
DROP FUNCTION inshandleactivities;
DROP FUNCTION inshandleactivitiesfiorequest;
DROP FUNCTION insupdhandles;
DROP FUNCTION upddomainexp;
DROP FUNCTION upddomainispublic;
DROP FUNCTION upddomainowner;
DROP FUNCTION instokenstakings;
DROP FUNCTION instokentransfers;
DROP FUNCTION insdomainactivities;
DROP FUNCTION insdomains;
DROP FUNCTION insaccountactivities;
DROP FUNCTION instraces;
DROP FUNCTION instransactions;
DROP FUNCTION insupdaccounts; 
DROP FUNCTION insblocks;
DROP TRIGGER onhandlesupdate ON handles;
DROP FUNCTION loghandlesupdate;
DROP TRIGGER ondomainsupdate ON domains;
DROP FUNCTION logdomainsupdate;
DROP TRIGGER onaccountsupdate ON accounts;
DROP FUNCTION logaccountsupdate;
DROP TRIGGER onfiorequestsupdate ON fiorequests;
DROP FUNCTION logfiorequestsupdate;
DROP TRIGGER onfiodatasupdate ON fiodatas;
DROP FUNCTION logfiodatasupdate;
DROP TRIGGER onpubaddressesupdate ON pubaddresses;
DROP FUNCTION logpubaddressesupdate;
DROP TRIGGER onpubaddressesdelete ON pubaddresses;
DROP FUNCTION logpubaddressesdelete;
DROP TRIGGER onnftsignaturesupdate ON nftsignatures;
DROP FUNCTION lognftsignaturesupdate;
DROP TRIGGER onnftsignaturesdelete ON nftsignatures;
DROP FUNCTION lognftsignaturesdelete;
DROP FUNCTION rbhandles;
DROP FUNCTION rbdomains;
DROP FUNCTION rbaccounts;
DROP FUNCTION rbfiorequests;
DROP FUNCTION rbfiodatas;
DROP FUNCTION rbpubaddresses;
DROP FUNCTION rbnftsignatures;
DROP FUNCTION rbfork;
DROP FUNCTION getaccountpubkeyandid;




