const {Ice} = require("ice/src/index");
const {Bank} = require("../ice/Bank");
const inquirer = require("inquirer");

let communicator;
let base;
let bank;
let account;

let credentials;

const createCredentials = (login, password) => {
    let map = new Map();
    map.set('id', login);
    map.set('pwd', password);
    return map;
};

const getPort = async () => {
    let port;
    await inquirer
        .prompt([
            {
                type: 'text',
                message: 'Enter bank port',
                name: 'port',
                validate: (value) => {
                    if (/^\d+$/.test(value)) return true;
                    else return 'Port must contain only digits';
                }
            }
        ])
        .then(answer => {
            port = answer.port;
        });
    return port;
};

const connect = async (port) => {
    communicator = Ice.initialize();
    base = communicator.stringToProxy(`management:tcp -h localhost -p ${port}`);
    bank = await Bank.AccountManagementPrx.checkedCast(base);
};

const logInToBank = async () => {
    await inquirer
        .prompt([
            {
                type: 'text',
                message: 'Enter login',
                name: 'login'
            },
            {
                type: 'password',
                message: 'Enter password',
                name: 'password',
                mask: '*'
            }
        ])
        .then(answer => {
            login = answer.login;
            password = answer.password;
        });
    credentials = createCredentials(login, password);
    account = await bank.login(context=credentials);
    if (await account.ice_isA('::Bank::PremiumAccount')){
        account = await Bank.PremiumAccountPrx.checkedCast(account);
    }
};

const registerAnAccount = async () => {
    let name;
    let surname;
    let id;
    let income;
    await inquirer
        .prompt([
            {
                type: 'text',
                message: 'Enter your name',
                name: 'name'
            },
            {
                type: 'text',
                message: 'Enter your surname',
                name: 'surname'
            },
            {
                type: 'text',
                message: 'Enter your PESEL (this will be used as a login)',
                name: 'id'
            },
            {
                type: 'number',
                message: 'Enter your monthly income',
                name: 'income',
                validate: (value) => {
                    if (/([0-9]*[.])?[0-9]+/.test(value)) return true;
                    else return 'Enter a valid income';
                }
            }
        ])
        .then(answer => {
            name = answer.name;
            surname = answer.surname;
            id = answer.id;
            income = answer.income;
        });
    let data = new Bank.ClientData(name, surname, id, income);
    let accountCreationData = await bank.register(data);

    console.log();
    console.log('Successfully created an account.');
    console.log(`Your account type is: ${accountCreationData.accountType}`);
    console.log(`Your password is: ${accountCreationData.password}`);
    console.log();
};

const getAccountData = async () => {
    let accountData = await account.getAccountData(context=credentials);
    console.log();
    console.log(`Your account type: ${accountData.accountType}`);
    console.log(`Available funds: ${accountData.funds}`);

    if (await account.ice_isA('::Bank::PremiumAccount')){
        console.log('Loans:');
        console.log(accountData.loans);
    }

    console.log();
};

const getLoan = async () => {
    if (! await account.ice_isA("::Bank::PremiumAccount")){
        console.log();
        console.log('Your account does not support loans!');
        console.log();
        return;
    }
    let amount;
    let currency;
    let loan_length;

    await inquirer
        .prompt([
            {
                type: 'text',
                message: 'Enter currency',
                name: 'currency'
            },
            {
                type: 'number',
                message: 'Enter amount',
                name: 'amount',
                validate: (value) => {
                    if (/([0-9]*[.])?[0-9]+/.test(value)) return true;
                    else return 'Enter a number';
                }
            },
            {
                type: 'number',
                message: 'Enter loan length in days',
                name: 'loan_length',
                validate: (value) => {
                    if (/^\d+$/.test(value)) return true;
                    else return 'Number of days must be made of only digits';
                }
            }
        ])
        .then(answer => {
            amount = answer.amount;
            currency = answer.currency;
            loan_length = answer.loan_length;
        });
    try {
        let loan = await account.getLoan(amount, currency, loan_length, context=credentials);
        console.log();
        if (loan > 0) {
            console.log(`Loan attained successfully. You got: ${loan}`);
        } else {
            console.log(`Loan did not get attained`);
        }
        console.log();
    } catch (ex){
        console.log();
        console.log(ex);
        console.log();
    }
};

const mainChoice = async () => {
    while (true) {
        let choice;
        await inquirer
            .prompt([
                {
                    type: 'list',
                    message: 'Select action',
                    name: 'selected',
                    choices: [
                        'Log in',
                        'Register',
                        'Exit'
                    ]
                }
            ])
            .then(answer => {
                choice = answer.selected;
            });
        if (choice === 'Exit') {
            return;
        } else if (choice === 'Log in') {
            while (true) {
                try {
                    await logInToBank();
                    break;
                } catch (ex) {
                    console.log();
                    console.log(ex.message);
                    console.log();
                }
            }
            break;
        } else if (choice === 'Register') {
            while (true) {
                try {
                    await registerAnAccount();
                    break;
                } catch (ex) {
                    console.log();
                    console.log(ex.message);
                    console.log();
                }
            }
        }
    }
    while(true){
        let choice;
        await inquirer
            .prompt([
                {
                    type: 'list',
                    message: 'Select account action',
                    name: 'selected',
                    choices: [
                        'Get info',
                        'Get loan',
                        'Log out'
                    ]
                }
            ])
            .then(answer => {
                choice = answer.selected;
            });
        if (choice === 'Log out'){
            account = null;
            credentials = null;

            console.log();
            console.log('Logged out successfully');
            console.log();
            await mainChoice();
            return;
        } else if (choice === 'Get info'){
            await getAccountData();
        } else if (choice === 'Get loan'){
            await getLoan();
        }
    }
};

// main
(async function () {
    try {
        console.clear();

        let port = await getPort();

        await connect(port);

        if (bank) {
            await mainChoice();
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
