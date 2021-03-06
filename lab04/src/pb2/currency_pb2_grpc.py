# Generated by the gRPC Python protocol compiler plugin. DO NOT EDIT!
import grpc

from pb2 import currency_pb2 as currency__pb2


class CurrencySubscriptionStub(object):
  # missing associated documentation comment in .proto file
  pass

  def __init__(self, channel):
    """Constructor.

    Args:
      channel: A grpc.Channel.
    """
    self.Subscribe = channel.unary_stream(
        '/CurrencySubscription/Subscribe',
        request_serializer=currency__pb2.SubscribeRequest.SerializeToString,
        response_deserializer=currency__pb2.SubscribeResponse.FromString,
        )


class CurrencySubscriptionServicer(object):
  # missing associated documentation comment in .proto file
  pass

  def Subscribe(self, request, context):
    # missing associated documentation comment in .proto file
    pass
    context.set_code(grpc.StatusCode.UNIMPLEMENTED)
    context.set_details('Method not implemented!')
    raise NotImplementedError('Method not implemented!')


def add_CurrencySubscriptionServicer_to_server(servicer, server):
  rpc_method_handlers = {
      'Subscribe': grpc.unary_stream_rpc_method_handler(
          servicer.Subscribe,
          request_deserializer=currency__pb2.SubscribeRequest.FromString,
          response_serializer=currency__pb2.SubscribeResponse.SerializeToString,
      ),
  }
  generic_handler = grpc.method_handlers_generic_handler(
      'CurrencySubscription', rpc_method_handlers)
  server.add_generic_rpc_handlers((generic_handler,))
