import grpc
from threading import Thread

import currency_pb2
import currency_pb2_grpc

import sys
import Ice
import Bank

currency_table = {}


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


def ice_run():
    with Ice.initialize(sys.argv) as communicator:
        adapter = communicator.createObjectAdapterWithEndpoints("SimplePrinterAdapter", "default -p 10000")
        object = PrinterI()
        adapter.add(object, communicator.stringToIdentity("SimplePrinter"))
        adapter.activate()
        communicator.waitForShutdown()


if __name__ == '__main__':
    currencies = map(str.upper, sys.argv[1:])
    Thread(target=start_currency_client, args=[currencies]).start()
    # ice_run()
