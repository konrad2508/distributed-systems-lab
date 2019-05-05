import sys
import grpc
import Ice
from threading import Thread

from pb2 import currency_pb2, currency_pb2_grpc
import Bank


######################################
# bank stub
######################################

class AccountI(Bank.Account):
    def __init__(self, data, type, pwd):
        self.data = data
        self.type = type
        self.pwd = pwd
        self.funds = 0

    def get_id(self):
        return self.data.id

    def get_pwd(self):
        return self.pwd

    def get_type(self):
        return self.type

    def getAccountData(self, current=None):
        if not current.ctx or current.ctx['id'] != self.get_id() or current.ctx['pwd'] != self.get_pwd():
            raise Bank.InvalidCredentialsException('Invalid credentials!')

        return Bank.AccountData(self.type, self.funds)


class PremiumAccountI(Bank.PremiumAccount, AccountI):
    def __init__(self, data, type, pwd):
        super().__init__(data, type, pwd)
        self.loan_history = []

    def getLoan(self, amount, currency, length, current=None):
        if not current.ctx or current.ctx['id'] != self.get_id() or current.ctx['pwd'] != self.get_pwd():
            raise Bank.InvalidCredentialsException('Invalid credentials!')

        currency = str.upper(str(currency))

        if currency not in list(currency_table.keys()):
            raise Bank.InvalidCredentialsException('Bank does not support requested currency!')

        loan = amount * currency_table[currency]
        self.funds += loan

        self.loan_history.append({
            'currency': str(currency),
            'amount': str(amount),
            'length': str(length)
        })

        print(self.loan_history)

        return loan

    def getAccountData(self, current=None):
        if not current.ctx or current.ctx['id'] != self.get_id() or current.ctx['pwd'] != self.get_pwd():
            raise Bank.InvalidCredentialsException('Invalid credentials!')

        return Bank.AccountData(self.type, self.funds, loans=self.loan_history)


class AccountManagementI(Bank.AccountManagement):
    def register(self, clientData, current=None):
        if any([clientData.id == acc['account'].get_id() for acc in account_table]):
            raise Bank.AccountAlreadyExistsException('Account already exists!')

        account_type = Bank.AccountType.Premium if clientData.income >= 1000 else Bank.AccountType.Standard
        account_pwd = '123'

        if account_type == Bank.AccountType.Premium:
            new_account = PremiumAccountI(clientData, account_type, account_pwd)

            identity = Ice.stringToIdentity("%s/%s" % ('premium', clientData.id))
            proxy = Bank.PremiumAccountPrx.uncheckedCast(current.adapter.add(new_account, identity))

        else:
            new_account = AccountI(clientData, account_type, account_pwd)

            identity = Ice.stringToIdentity("%s/%s" % ('standard', clientData.id))
            proxy = Bank.AccountPrx.uncheckedCast(current.adapter.add(new_account, identity))

        account_obj = {
            'account': new_account,
            'proxy': proxy
        }
        account_table.append(account_obj)

        print(account_table)

        return Bank.RegistrationInfo(account_type, account_pwd)

    def login(self, current=None):
        if not current.ctx:
            raise Bank.InvalidCredentialsException('Specify login and password!')

        id = current.ctx['id']
        pwd = current.ctx['pwd']

        account = None
        for acc in account_table:
            if acc['account'].get_id() == id:
                if acc['account'].get_pwd() == pwd:
                    account = acc['proxy']
                else:
                    raise Bank.InvalidCredentialsException('Invalid password!')

        if account is None:
            raise Bank.InvalidCredentialsException('Account with that id does not exist!')

        return account


######################################
# server and client methods
######################################

def start_currency_client(port, requested_currencies):
    with grpc.insecure_channel('localhost:%s' % port) as channel:
        stub = currency_pb2_grpc.CurrencySubscriptionStub(channel)

        currencies_values = map(currency_pb2.Currency.Value, requested_currencies)

        prices = stub.Subscribe(
            currency_pb2.SubscribeRequest(
                currencies=currencies_values)
        )

        while True:
            curr = prices.next()

            curr_name = currency_pb2.Currency.Name(curr.currency)
            curr_value = curr.value
            currency_table[curr_name] = curr_value

            print(currency_table)


def start_bank_server(port):
    with Ice.initialize(sys.argv) as communicator:
        adapter = communicator.createObjectAdapterWithEndpoints("Bank", "tcp -h localhost -p %s" % port)

        adapter.add(AccountManagementI(), Ice.stringToIdentity("management"))

        adapter.activate()
        communicator.waitForShutdown()


######################################
# program routine
######################################

currency_table = {}
account_table = []

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print('Wrong number of arguments!')
        exit(1)

    currency_port = sys.argv[1]
    bank_port = sys.argv[2]
    currencies = map(str.upper, sys.argv[3:])

    Thread(target=start_currency_client, args=[currency_port, currencies]).start()
    start_bank_server(bank_port)
