const {Ice} = require("ice/src/index");
const {Bank} = require("../ice/Bank");

(async function () {
    let communicator;
    try {
        communicator = Ice.initialize();
        const base = communicator.stringToProxy("management:tcp -h localhost -p 10000");
        const bank = await Bank.AccountManagementPrx.checkedCast(base);
        if (bank) {
            let data = new Bank.ClientData("user", "userable", "11223344", 1200);
            let accountCreationData = await bank.register(data);

            let account = await bank.login("11223344", accountCreationData.password);

            let ret2 = await account.getAccountData();

            console.log('yatta!');
            console.log(ret2);

        } else {
            console.log("Invalid proxy");
        }
    } catch (ex) {
        console.log(ex.toString());
        process.exitCode = 1;
    } finally {
        if (communicator) {
            await communicator.destroy();
        }
    }
}());