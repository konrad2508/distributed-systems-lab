const {Ice} = require("ice");
const {Bank} = require("./Bank");

(async function() {
    let communicator;
    try
    {
        communicator = Ice.initialize();
        const base = communicator.stringToProxy("premium:tcp -h localhost -p 10000");
        const bank = await Bank.PremiumAccountPrx.checkedCast(base);
        if(bank)
        {
            let ret = await bank.getAccountData();
            console.log(ret)
        }
        else
        {
            console.log("Invalid proxy");
        }
    }
    catch(ex)
    {
        console.log(ex.toString());
        process.exitCode = 1;
    }
    finally
    {
        if(communicator)
        {
            await communicator.destroy();
        }
    }
}());