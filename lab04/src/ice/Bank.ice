module Bank{

    enum AccountType { Standard, Premium }

    enum Currency { USD, EUR, GBP, PLN }

    exception AccountException{
        string reason;
    };

    exception InvalidCredentialsException extends AccountException{
    };

    exception AccountAlreadyExistsException extends AccountException{
    };

    exception UnrecognizedCurrencyException{
        string reason;
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
        AccountData getAccountData() throws InvalidCredentialsException;
    };

    interface PremiumAccount extends Account{
        double getLoan(double amount, string currency, int length) throws UnrecognizedCurrencyException, InvalidCredentialsException;
    };

    interface AccountManagement{
        RegistrationInfo register(ClientData clientData) throws AccountAlreadyExistsException;
        Account* login() throws InvalidCredentialsException;
    };

};