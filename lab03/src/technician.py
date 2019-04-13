import pika
import time
import random

from src.msg import Msg


def callback(ch, method, properties, body):
    message = Msg.decode(body)
    print('Received task from %s: %s examination for %s' % (message.sender, message.bodypart, message.patient))
    time.sleep(random.randint(5, 11))
    print('Finished examining')
    message.status = True
    channel.basic_publish(exchange='',
                          routing_key=message.sender,
                          body=str(message))
    ch.basic_ack(delivery_tag=method.delivery_tag)


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
    channel.queue_declare(queue=spec1)
    channel.queue_declare(queue=spec2)

    print('Awaiting tasks...')
    channel.basic_consume(queue=spec1,
                          on_message_callback=callback)

    channel.basic_consume(queue=spec2,
                          on_message_callback=callback)

    channel.start_consuming()
