from __future__ import print_function

import grpc

import currency_pb2
import currency_pb2_grpc

import sys
import Ice
import Demo


class PrinterI(Demo.Printer):
    def printString(self, s, current=None):
        print(s)


def run():
    with grpc.insecure_channel('localhost:50051') as channel:
        stub = currency_pb2_grpc.CurrencySubscriptionStub(channel)

        prices = stub.Subscribe(
            currency_pb2.SubscribeRequest(
                currencies=[currency_pb2.Currency.Value('PLN'), currency_pb2.Currency.Value('USD')])
        )
        while True:
            curr = prices.next()
            print('currency: %s' % currency_pb2.Currency.Name(curr.currency))
            print('value: %f' % curr.value)
            print()


def ice_run():
    with Ice.initialize(sys.argv) as communicator:
        adapter = communicator.createObjectAdapterWithEndpoints("SimplePrinterAdapter", "default -p 10000")
        object = PrinterI()
        adapter.add(object, communicator.stringToIdentity("SimplePrinter"))
        adapter.activate()
        communicator.waitForShutdown()


if __name__ == '__main__':
    # run()
    ice_run()
