import sys
import time
import grpc
import random
from concurrent import futures
from threading import RLock, Condition, Thread

from pb2 import currency_pb2, currency_pb2_grpc

_SECONDS_IN_DAY = 60 * 60 * 24

currency_table = {}
recent_changes = []


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


def init():
    available_currencies = currency_pb2.Currency.values()
    for currency in available_currencies:
        currency_table[currency] = round(random.uniform(1, 6), 2)


def start_server(port):
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    currency_pb2_grpc.add_CurrencySubscriptionServicer_to_server(
        CurrencySubscription(), server)
    server.add_insecure_port('localhost:%s' % port)
    server.start()
    try:
        while True:
            time.sleep(_SECONDS_IN_DAY)
    except KeyboardInterrupt:
        server.stop(0)


def change_values():
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

    port = sys.argv[1]

    Thread(target=start_server, args=[port]).start()
    change_values()
