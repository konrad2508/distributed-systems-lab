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
    def getAccountData(self, current=None):
        return Bank.AccountData(Bank.AccountType.Standard, 500)


class PremiumAccountI(Bank.PremiumAccount, AccountI):
    def getLoan(self, amount, currency, current=None):
        return True


class AccountManagementI(Bank.AccountManagement):
    def register(self, clientData, current=None):
        pass

    def login(self, id, password, current=None):
        pass


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

            print(currency_table)


def start_bank_server():
    with Ice.initialize(sys.argv) as communicator:
        adapter = communicator.createObjectAdapterWithEndpoints("Bank", "tcp -h localhost -p 10000")

        adapter.add(AccountI(), Ice.stringToIdentity("standard"))
        adapter.add(PremiumAccountI(), Ice.stringToIdentity("premium"))
        adapter.add(AccountManagementI(), Ice.stringToIdentity("management"))

        adapter.activate()
        communicator.waitForShutdown()


######################################
# program routine
######################################

currency_table = {}

if __name__ == '__main__':
    currencies = map(str.upper, sys.argv[1:])

    Thread(target=start_currency_client, args=[currencies]).start()
    start_bank_server()
