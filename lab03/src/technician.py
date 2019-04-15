import pika
import time
import random

from src.msg import Msg


def callback(ch, method, properties, body):
    message = Msg.decode(body)
    if message.sender.startswith('doc'):
        print('Received task from %s: %s examination for %s' % (message.sender, message.bodypart, message.patient))
        time.sleep(random.randint(5, 11))
        print('Finished examining')
        message.status = True
        channel.basic_publish(exchange='topic',
                              routing_key=message.sender,
                              body=str(message))
        ch.basic_ack(delivery_tag=method.delivery_tag)
    else:
        print('Info message: %s' % message.bodypart)


if __name__ == '__main__':
    specializations = ('hip', 'knee', 'elbow')

    while True:
        spec1 = input('Enter first specialization: ')
        spec2 = input('Enter second specialization: ')
        if spec1.startswith(specializations):
            if spec1 != spec2:
                break
            else:
                print('Please specify 2 different specializations\n')
        else:
            print('Please enter proper specializations. Possible specializations are: '
                  + ', '.join(specializations) + '\n')

    connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))

    channel = connection.channel()

    tech_q = 'technician.%s' % str(time.time())
    channel.queue_declare(queue=tech_q)
    channel.queue_declare(queue=spec1)
    channel.queue_declare(queue=spec2)

    channel.exchange_declare(exchange='fanout', exchange_type='fanout')
    channel.exchange_declare(exchange='topic', exchange_type='topic')

    channel.queue_bind(queue=spec1, exchange='topic')
    channel.queue_bind(queue=spec2, exchange='topic')
    channel.queue_bind(queue=tech_q, exchange='fanout')

    print('Awaiting tasks...')
    channel.basic_consume(queue=spec1,
                          on_message_callback=callback)

    channel.basic_consume(queue=spec2,
                          on_message_callback=callback)

    channel.basic_consume(queue=tech_q,
                          on_message_callback=callback)

    channel.basic_qos(prefetch_count=1)
    channel.start_consuming()
