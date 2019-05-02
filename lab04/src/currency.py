import time
import grpc
import random
from concurrent import futures
from threading import RLock, Condition, Thread

import currency_pb2
import currency_pb2_grpc


class ObservableCurrencyTable(object):
    def __init__(self, currencies):
        self.observers = set()

        self.currency_table = {}
        for currency in currencies:
            self.currency_table[currency] = round(random.uniform(1, 6), 2)

    def get(self, key):
        return self.currency_table[key]

    def register(self, who):
        self.observers.add(who)

    def unregister(self, who):
        self.observers.discard(who)

    def publish(self, currency, new_value):
        self.currency_table[currency] = new_value

        for observer in self.observers:
            observer.update(currency, new_value)


class CurrencySubscription(currency_pb2_grpc.CurrencySubscriptionServicer):
    def Subscribe(self, request, context):
        print(request.currencies)

        for curr in request.currencies:
            yield currency_pb2.SubscribeResponse(currency=curr, value=currency_table.get(curr))

        while True:
            with condition:
                condition.wait()
                for curr in request.currencies:
                    if curr in recent_changes:
                        yield currency_pb2.SubscribeResponse(currency=curr, value=currency_table.get(curr))


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    currency_pb2_grpc.add_CurrencySubscriptionServicer_to_server(
        CurrencySubscription(), server)
    server.add_insecure_port('localhost:50051')
    server.start()
    try:
        while True:
            time.sleep(3600 * 24)
    except KeyboardInterrupt:
        server.stop(0)


currency_table = {}
recent_changes = []

def init():
    available_currencies = currency_pb2.Currency.values()
    for currency in available_currencies:
        currency_table[currency] = round(random.uniform(1, 6), 2)


def change():
    while True:
        time.sleep(5)

        recent_changes.clear()
        with condition:
            for currency in currency_table:
                choice = random.getrandbits(1)
                if choice:
                    new_val = round(random.uniform(1, 6), 2)
                    currency_table[currency] = new_val
                    recent_changes.append(currency)
            condition.notifyAll()


if __name__ == '__main__':
    lock = RLock()
    condition = Condition(lock)

    init()
    Thread(target=serve).start()
    change()
