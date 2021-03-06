import pika

from src.msg import Msg
from threading import Thread


def receiving_callback(ch, method, properties, body):
    received = Msg.decode(body)
    if received.sender.startswith('admin'):
        print('Info message: %s' % received.bodypart)
    else:
        print('Received: ' + str(received))


def receiving_routine():
    channel.basic_consume(queue=doc_q,
                          auto_ack=True,
                          on_message_callback=receiving_callback)
    channel.start_consuming()


if __name__ == '__main__':
    specializations = ('hip', 'knee', 'elbow')

    doc_id = input('Enter doctor ID: ')
    doc_q = 'doc.%s' % doc_id

    connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
    channel = connection.channel()

    channel.queue_declare(queue=doc_q)
    channel.queue_declare(queue='hip')
    channel.queue_declare(queue='knee')
    channel.queue_declare(queue='elbow')

    channel.exchange_declare(exchange='fanout', exchange_type='fanout')
    channel.exchange_declare(exchange='topic', exchange_type='topic')

    channel.queue_bind(exchange='topic', queue=doc_q, routing_key=doc_q)
    channel.queue_bind(exchange='fanout', queue=doc_q, routing_key=doc_q)

    receiver = Thread(target=receiving_routine)
    receiver.start()

    while True:
        input('Press any key to add a new patient...\n')
        surname = input("Enter patient's surname: ")
        action = input('Select action (hip, knee, elbow): ')
        if action.startswith(specializations):
            to_send = str(Msg(doc_q, action, surname, False))
            channel.basic_publish(exchange='topic',
                                  routing_key=action,
                                  body=to_send)
        elif action.startswith('exit'):
            break
        else:
            print('Selected unknown action, try again.')

    connection.close()
