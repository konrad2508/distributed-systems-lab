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

    def getAccountData(self, current=None):
        return Bank.AccountData(self.type, self.funds)


class PremiumAccountI(Bank.PremiumAccount, AccountI):
    def getLoan(self, amount, currency, current=None):
        return True


class AccountManagementI(Bank.AccountManagement):
    def register(self, clientData, current=None):
        if any([clientData.id == acc.get_id() for acc in account_table]):
            raise Bank.AccountException('Account already exists!')

        account_type = Bank.AccountType.Premium if clientData.income >= 1000 else Bank.AccountType.Standard
        account_pwd = 'TriHard 7'

        new_account = AccountI(clientData, account_type, account_pwd)
        account_table.append(new_account)

        return Bank.RegistrationInfo(account_type, account_pwd)

    def login(self, id, password, current=None):
        account = None
        for acc in account_table:
            if acc.get_id() == id:
                if acc.get_pwd() == password:
                    account = acc
                else:
                    raise Bank.AccountException('Invalid password!')

        if account is None:
            raise Bank.AccountException('Account with that id does not exist!')

        proxy = Bank.PremiumAccountPrx.uncheckedCast(current.adapter.addWithUUID(account))

        return proxy


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

            # print(currency_table)


def start_bank_server():
    with Ice.initialize(sys.argv) as communicator:
        adapter = communicator.createObjectAdapterWithEndpoints("Bank", "tcp -h localhost -p 10000")

        # adapter.add(AccountI(), Ice.stringToIdentity("standard"))
        # adapter.add(PremiumAccountI(), Ice.stringToIdentity("premium"))
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
