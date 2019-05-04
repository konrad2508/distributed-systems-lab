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

    dictionary<string, string> LoanHistory;
    sequence<LoanHistory> LoanHistorySeq;

    class AccountData{
        AccountType accountType;
        double funds;
        optional(1) LoanHistorySeq loans;
    };

    struct RegistrationInfo{
        AccountType accountType;
        string password;
    }

    interface Account{
        AccountData getAccountData() throws AccountException;
    };

    interface PremiumAccount extends Account{
        double getLoan(double amount, string currency, int length) throws CurrencyException, AccountException;
    };

    interface AccountManagement{
        RegistrationInfo register(ClientData clientData) throws AccountException;
        Account* login() throws AccountException;
    };

};