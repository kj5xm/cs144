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

  if(data.size() > available_capacity()) {
    data = data.substr(0,available_capacity());
  }

  bytes_buffered_ += data.size();
  bytes_pushed_ += data.size();
  buffer_.push(std::move(data));
  if(view_.empty() && !buffer_.empty()) {
    view_ = buffer_.front();
  }
}

void Writer::close()
{
  // Your code here.
  closed_ = true;
}

void Writer::set_error()
{
  // Your code here.
  error_ = true;
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
  if(view_ .empty()) {
    string_view sv_empty = ""sv;
    return sv_empty;
  } 

  string_view sv(view_);
  return sv;
}

bool Reader::is_finished() const
{
  // Your code here.
  return closed_ && buffer_.empty();
}

bool Reader::has_error() const
{
  // Your code here.
  return error_;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  if(len > bytes_buffered_) {
    len = bytes_buffered_;
  }

  while(!buffer_.empty() && len > 0) {
    if(buffer_.front().size() <= len) {
      bytes_buffered_ -= buffer_.front().size();
      bytes_poped_ += buffer_.front().size();
      len -= buffer_.front().size();
      buffer_.pop();
    } else {
      buffer_.front() = buffer_.front().substr(len, buffer_.front().size()-1);
      bytes_buffered_ -= len;
      bytes_poped_ += len;
      len = 0;
    }
  }
  
  if(bytes_buffered_ != 0) {
      view_ = buffer_.front();
  } else {
    view_ = "";
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
