from __future__ import print_function

import grpc

import currency_pb2
import currency_pb2_grpc


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


if __name__ == '__main__':
    run()
