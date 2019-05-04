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
            raise Bank.AccountException('Invalid credentials!')

        return Bank.AccountData(self.type, self.funds)


class PremiumAccountI(Bank.PremiumAccount, AccountI):
    def __init__(self, data, type, pwd):
        super().__init__(data, type, pwd)
        self.loan_history = []

    def getLoan(self, amount, currency, length, current=None):
        if not current.ctx or current.ctx['id'] != self.get_id() or current.ctx['pwd'] != self.get_pwd():
            raise Bank.AccountException('Invalid credentials!')

        currency = str.upper(str(currency))

        if currency not in list(currency_table.keys()):
            raise Bank.CurrencyException('Bank does not support requested currency!')

        loan = amount * currency_table[currency]
        self.funds += loan

        self.loan_history.append({
            'currency': str(currency),
            'amount': str(amount),
            'length': str(length)
        })

        return loan

    def getAccountData(self, current=None):
        if not current.ctx or current.ctx['id'] != self.get_id() or current.ctx['pwd'] != self.get_pwd():
            raise Bank.AccountException('Invalid credentials!')

        return Bank.AccountData(self.type, self.funds, loans=self.loan_history)


class AccountManagementI(Bank.AccountManagement):
    def register(self, clientData, current=None):
        if any([clientData.id == acc.get_id() for acc in account_table]):
            raise Bank.AccountException('Account already exists!')

        account_type = Bank.AccountType.Premium if clientData.income >= 1000 else Bank.AccountType.Standard
        account_pwd = 'TriHard 7'

        if account_type == Bank.AccountType.Premium:
            new_account = PremiumAccountI(clientData, account_type, account_pwd)
        else:
            new_account = AccountI(clientData, account_type, account_pwd)
        account_table.append(new_account)

        return Bank.RegistrationInfo(account_type, account_pwd)

    def login(self, current=None):
        if not current.ctx:
            raise Bank.AccountException('Specify login and password!')

        id = current.ctx['id']
        pwd = current.ctx['pwd']

        account = None
        for acc in account_table:
            if acc.get_id() == id:
                if acc.get_pwd() == pwd:
                    account = acc
                else:
                    raise Bank.AccountException('Invalid password!')

        if account is None:
            raise Bank.AccountException('Account with that id does not exist!')

        if account.get_type() == Bank.AccountType.Premium:
            identity = Ice.stringToIdentity("%s/%s" % ('premium', id))
            proxy = Bank.PremiumAccountPrx.uncheckedCast(current.adapter.add(account, identity))
        else:
            identity = Ice.stringToIdentity("%s/%s" % ('standard', id))
            proxy = Bank.AccountPrx.uncheckedCast(current.adapter.add(account, identity))

        return proxy

    def logout(self, proxy, current=None):
        identity = proxy.ice_getIdentity()

        current.adapter.remove(identity)

######################################
# server and client methods
######################################

def start_currency_client(requested_currencies):
    with grpc.insecure_channel('localhost:50051') as channel:
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


def start_bank_server():
    with Ice.initialize(sys.argv) as communicator:
        adapter = communicator.createObjectAdapterWithEndpoints("Bank", "tcp -h localhost -p 10000")

        adapter.add(AccountManagementI(), Ice.stringToIdentity("management"))

        adapter.activate()
        communicator.waitForShutdown()


######################################
# program routine
######################################

currency_table = {}
account_table = []

if __name__ == '__main__':
    currencies = map(str.upper, sys.argv[1:])

    Thread(target=start_currency_client, args=[currencies]).start()
    start_bank_server()
