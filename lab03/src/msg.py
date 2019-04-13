class Msg(object):

    @staticmethod
    def decode(message):
        message = str(message)
        args = message.split("'")[1].split(':')

        sender = args[0]
        bodypart = args[1]
        patient = args[2]
        status = args[3]
        return Msg(sender, bodypart, patient, status)

    def __init__(self, sender, bodypart, patient, status):
        self.sender = sender
        self.bodypart = bodypart
        self.patient = patient
        self.status = status

    def __str__(self):
        return '%s:%s:%s:%s' % (self.sender, self.bodypart, self.patient, self.status)
