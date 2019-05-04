//
// Copyright (c) ZeroC, Inc. All rights reserved.
//
//
// Ice version 3.7.2
//
// <auto-generated>
//
// Generated from file `Bank.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

/* eslint-disable */
/* jshint ignore: start */

(function(module, require, exports)
{
    const Ice = require("ice").Ice;
    const _ModuleRegistry = Ice._ModuleRegistry;
    const Slice = Ice.Slice;

    let Bank = _ModuleRegistry.module("Bank");

    Bank.AccountType = Slice.defineEnum([
        ['Standard', 0], ['Premium', 1]]);

    Bank.Currency = Slice.defineEnum([
        ['USD', 0], ['EUR', 1], ['GBP', 2], ['PLN', 3]]);

    Bank.AccountException = class extends Ice.UserException
    {
        constructor(message = "", _cause = "")
        {
            super(_cause);
            this.message = message;
        }

        static get _parent()
        {
            return Ice.UserException;
        }

        static get _id()
        {
            return "::Bank::AccountException";
        }

        _mostDerivedType()
        {
            return Bank.AccountException;
        }

        _writeMemberImpl(ostr)
        {
            ostr.writeString(this.message);
        }

        _readMemberImpl(istr)
        {
            this.message = istr.readString();
        }
    };

    Bank.CurrencyException = class extends Ice.UserException
    {
        constructor(message = "", _cause = "")
        {
            super(_cause);
            this.message = message;
        }

        static get _parent()
        {
            return Ice.UserException;
        }

        static get _id()
        {
            return "::Bank::CurrencyException";
        }

        _mostDerivedType()
        {
            return Bank.CurrencyException;
        }

        _writeMemberImpl(ostr)
        {
            ostr.writeString(this.message);
        }

        _readMemberImpl(istr)
        {
            this.message = istr.readString();
        }
    };

    Bank.ClientData = class
    {
        constructor(name = "", surname = "", id = "", income = 0)
        {
            this.name = name;
            this.surname = surname;
            this.id = id;
            this.income = income;
        }

        _write(ostr)
        {
            ostr.writeString(this.name);
            ostr.writeString(this.surname);
            ostr.writeString(this.id);
            ostr.writeInt(this.income);
        }

        _read(istr)
        {
            this.name = istr.readString();
            this.surname = istr.readString();
            this.id = istr.readString();
            this.income = istr.readInt();
        }

        static get minWireSize()
        {
            return  7;
        }
    };

    Slice.defineStruct(Bank.ClientData, true, true);

    Bank.AccountData = class
    {
        constructor(accountType = Bank.AccountType.Standard, funds = 0)
        {
            this.accountType = accountType;
            this.funds = funds;
        }

        _write(ostr)
        {
            Bank.AccountType._write(ostr, this.accountType);
            ostr.writeInt(this.funds);
        }

        _read(istr)
        {
            this.accountType = Bank.AccountType._read(istr);
            this.funds = istr.readInt();
        }

        static get minWireSize()
        {
            return  5;
        }
    };

    Slice.defineStruct(Bank.AccountData, true, true);

    Bank.RegistrationInfo = class
    {
        constructor(accountType = Bank.AccountType.Standard, password = "")
        {
            this.accountType = accountType;
            this.password = password;
        }

        _write(ostr)
        {
            Bank.AccountType._write(ostr, this.accountType);
            ostr.writeString(this.password);
        }

        _read(istr)
        {
            this.accountType = Bank.AccountType._read(istr);
            this.password = istr.readString();
        }

        static get minWireSize()
        {
            return  2;
        }
    };

    Slice.defineStruct(Bank.RegistrationInfo, true, true);

    const iceC_Bank_Account_ids = [
        "::Bank::Account",
        "::Ice::Object"
    ];

    Bank.Account = class extends Ice.Object
    {
    };

    Bank.AccountPrx = class extends Ice.ObjectPrx
    {
    };

    Slice.defineOperations(Bank.Account, Bank.AccountPrx, iceC_Bank_Account_ids, 0,
    {
        "getAccountData": [, , , , [Bank.AccountData], , , , , ]
    });

    const iceC_Bank_PremiumAccount_ids = [
        "::Bank::Account",
        "::Bank::PremiumAccount",
        "::Ice::Object"
    ];

    Bank.PremiumAccount = class extends Ice.Object
    {
        static get _iceImplements()
        {
            return [
                Bank.Account
            ];
        }
    };

    Bank.PremiumAccountPrx = class extends Ice.ObjectPrx
    {
        static get _implements()
        {
            return [
                Bank.AccountPrx];
        }
    };

    Slice.defineOperations(Bank.PremiumAccount, Bank.PremiumAccountPrx, iceC_Bank_PremiumAccount_ids, 1,
    {
        "getLoan": [, , , , [1], [[6], [Bank.Currency._helper]], ,
        [
            Bank.CurrencyException
        ], , ]
    });

    const iceC_Bank_AccountManagement_ids = [
        "::Bank::AccountManagement",
        "::Ice::Object"
    ];

    Bank.AccountManagement = class extends Ice.Object
    {
    };

    Bank.AccountManagementPrx = class extends Ice.ObjectPrx
    {
    };

    Slice.defineOperations(Bank.AccountManagement, Bank.AccountManagementPrx, iceC_Bank_AccountManagement_ids, 0,
    {
        "register": [, , , , [Bank.RegistrationInfo], [[Bank.ClientData]], ,
        [
            Bank.AccountException
        ], , ],
        "login": [, , , , ["Bank.AccountPrx"], [[7], [7]], ,
        [
            Bank.AccountException
        ], , ]
    });
    exports.Bank = Bank;
}
(typeof(global) !== "undefined" && typeof(global.process) !== "undefined" ? module : undefined,
 typeof(global) !== "undefined" && typeof(global.process) !== "undefined" ? require :
 (typeof WorkerGlobalScope !== "undefined" && self instanceof WorkerGlobalScope) ? self.Ice._require : window.Ice._require,
 typeof(global) !== "undefined" && typeof(global.process) !== "undefined" ? exports :
 (typeof WorkerGlobalScope !== "undefined" && self instanceof WorkerGlobalScope) ? self : window));
