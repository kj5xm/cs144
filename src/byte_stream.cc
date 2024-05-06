#include <format>
#include <new>
#include <stdexcept>
#include <string_view>
#include <iostream>
#include <unistd.h>
#include "byte_stream.hh"
#include "stdint.h"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}


void Writer::push( string data )
{
  // Your code here.
  //(void)data;
  if(is_closed()) {
    return;
  }

  if(data.empty()|| available_capacity() == 0) {
    return;
  }

  auto n = min(data.size(), available_capacity());
  if(n < data.size()) {
    data = data.substr(0, n);
  }

  bytes_buffered_ += n;
  bytes_pushed_ += n;
  data_queue_.push(std::move(data));
  //view_queue_.emplace(data_queue_.back().c_str(), n); 
  view_queue_.emplace(data_queue_.back()); 
}

void Writer::close()
{
  // Your code here.
  closed_ = true;
}

bool Writer::is_closed() const
{
  // Your code here.
  return closed_;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - bytes_buffered_;
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return bytes_pushed_;
}

string_view Reader::peek() const
{
  // Your code here.
  if(view_queue_ .empty()) {
    return {};
  } 

  return view_queue_.front();
}

bool Reader::is_finished() const
{
  // Your code here.
  return closed_ && data_queue_.empty();
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  auto n = min(bytes_buffered_, len);

  while(n > 0) {
    auto front_size = view_queue_.front().size();
    if(n < front_size) {
      view_queue_.front().remove_prefix(n);
      bytes_buffered_ -= n;
      bytes_poped_ += n;
      return;
    }

    data_queue_.pop();
    view_queue_.pop();
    bytes_buffered_ -= front_size;
    bytes_poped_ += front_size;
    n -= front_size;
  }
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return bytes_buffered_;
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return bytes_poped_;
}
