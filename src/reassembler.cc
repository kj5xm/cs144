#include "reassembler.hh"
#include "byte_stream.hh"
#include <cstdint>
#include <iostream>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{

  first_unassembled_ = writer().bytes_pushed();
  first_unaccepted_ = writer().bytes_pushed() + writer().available_capacity();

  if ( is_last_substring ) {
    bytes_last_ = first_index + data.size();
  }

  if (data.size() > 0 && (first_index + data.size() - 1 < first_unassembled_ || first_index >= first_unaccepted_) ) {
    return;
  }

  uint64_t str_begin_ = first_index;
  uint64_t str_end_ = first_index + data.size();

  if ( first_index < first_unassembled_ ) {
    str_begin_ = first_unassembled_;
  }

  if ( first_index + data.size() > first_unaccepted_ ) {
    str_end_ = first_unaccepted_;
  }

  if(data.size() > 0) {
    if ( storage_.empty() ) {
      storage_.emplace(str_begin_, data.substr( str_begin_ - first_index, str_end_ - str_begin_));
      bytes_pending_ += str_end_ - str_begin_;
    } else {

      uint64_t str_ptr_ = str_begin_;

      for ( auto it = storage_.begin(); it != storage_.end(); it++ ) {
        if ( str_ptr_ <= it->first ) {
          uint64_t substr_end_ = min( str_end_, it->first );
          uint64_t substr_len_ = substr_end_ - str_ptr_;

          if(substr_len_ > 0) {
            storage_.emplace( str_ptr_, data.substr( str_ptr_ - first_index, substr_len_) );
            bytes_pending_ += substr_len_;
          }
        }

        if(str_ptr_ < it->first + it->second.size()){
          str_ptr_ = it->first + it->second.size();
        }

        if ( str_ptr_ >= str_end_) {
          break;
        }
      }

      if(str_ptr_ < str_end_) {
        uint64_t substr_len_ = str_end_ - str_ptr_;
        storage_.emplace( str_ptr_, data.substr( str_ptr_ - first_index, substr_len_) );
        bytes_pending_ += substr_len_;
      }
    }
  }

  for ( auto it = storage_.cbegin(); it != storage_.cend();  ) {
    if ( it->first == writer().bytes_pushed() ) {
      const_cast<Writer&>( writer() ).push( it->second );

      bytes_pending_ -= it->second.size();
      storage_.erase(it++);
    } else {
      break;
    }
  }

  if ( writer().bytes_pushed() == bytes_last_ ) {
    const_cast<Writer&>( writer() ).close();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return bytes_pending_;
}
