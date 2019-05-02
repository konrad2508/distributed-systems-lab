import pika
import time
from threading import Thread
from datetime import datetime

from src.msg import Msg


def callback(ch, method, properties, body):
    try:
        received = Msg.decode(body)
        print(str(datetime.now()) + ': ' + str(received))
    except IndexError:
        print(str(body))


def receiving_routine():
    channel.basic_consume(queue=admin_q,
                          auto_ack=True,
                          on_message_callback=callback)
    channel.start_consuming()


if __name__ == '__main__':
    specializations = ('hip', 'knee', 'elbow')

    connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
    channel = connection.channel()

    admin_q = 'admin.%s' % str(time.time())
    channel.queue_declare(queue=admin_q)

    channel.exchange_declare(exchange='fanout', exchange_type='fanout')
    channel.exchange_declare(exchange='topic', exchange_type='topic')

    channel.queue_bind(queue=admin_q, exchange='topic', routing_key='#')

    receiver = Thread(target=receiving_routine)
    receiver.start()

    while True:
        msg_body = input('Enter message: ')
        msg = Msg('admin', msg_body, '', '')
        channel.basic_publish(exchange='fanout',
                              routing_key='',
                              body=str(msg))
