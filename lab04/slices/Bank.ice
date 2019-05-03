module Bank{

    struct ClientData{
        string name;
        string surname;
        string id;
        int income;
    };

    interface AccountManagement{
        Account register(ClientData clientData);
        Account login();
    };

    interface Account{

    };

    interface PremiumAccount extends Account{

    };

};