#include "tcp_receiver.hh"
#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "wrapping_integers.hh"
#include <cstdint>
#include <iostream>
#include <optional>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if (message.RST) {
    const_cast<Writer&>(writer()).set_error(); 
    reader().set_error(); 
    const_cast<Writer&>(writer()).close();
    error_ = true;
  }

  if (!received_syn_) {
    if (!message.SYN) {
      return;
    }
    iyn_ = message.seqno;
    received_syn_ = true;
  }

  if (received_syn_ && !message.RST && !writer().is_closed()) {
    uint64_t stream_index = (message.seqno+message.SYN).unwrap(iyn_, writer().bytes_pushed()) - 1;
    reassembler_.insert(stream_index, message.payload, message.FIN);
  }

  if (received_syn_ && message.FIN) {
    received_fin_ = true;
    fin_ = message.seqno + message.sequence_length();
  }

  if (received_syn_ && message.FIN && reassembler_.bytes_pending() == 0) {
    const_cast<Writer&>(writer()).close();
  }

}

TCPReceiverMessage TCPReceiver::send() const
{
  TCPReceiverMessage message;
  if (received_syn_) {
    uint64_t next_write = writer().bytes_pushed() + 1;
    if(received_fin_ && reassembler_.bytes_pending() == 0) {
      next_write = received_fin_ ? next_write + 1 : next_write;
    }
    message.ackno = Wrap32::wrap(next_write, iyn_);
  } else {
    message.ackno = nullopt;
  }

  if (writer().has_error() || reader().has_error() || error_) {
    message.RST = true;
  }

  message.window_size = writer().available_capacity() > UINT16_MAX ? UINT16_MAX : writer().available_capacity();

  return message;
}
