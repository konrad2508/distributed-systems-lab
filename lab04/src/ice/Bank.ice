module Bank{

    enum AccountType { Standard, Premium }

    enum Currency { USD, EUR, GBP, PLN }

    exception AccountException{
        string message;
    };

    exception CurrencyException{
        string message;
    };

    struct ClientData{
        string name;
        string surname;
        string id;
        double income;
    };

    struct AccountData{
        AccountType accountType;
        double funds;
    };

    struct RegistrationInfo{
        AccountType accountType;
        string password;
    }

    interface Account{
        AccountData getAccountData();
    };

    interface PremiumAccount extends Account{
        double getLoan(double amount, string currency) throws CurrencyException;
    };

    interface AccountManagement{
        RegistrationInfo register(ClientData clientData) throws AccountException;
        Account* login(string id, string password) throws AccountException;
    };

};