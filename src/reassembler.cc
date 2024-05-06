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
    // cout << "bytes_last_ = " << bytes_last_ << endl;
  }

  if ( first_index + data.size() - 1 < first_unassembled_ || first_index >= first_unaccepted_ ) {
     std::cout << "return due to totally out range ! : " << data << std::endl;
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
      cout << "str_begin_ = " << str_begin_ << ", str_end_ = " << str_end_ << " , first_index = " << first_index
           << ", content = " << data.substr( str_begin_ - first_index, str_end_ - str_begin_ ) << endl;
      storage_.emplace(str_begin_, data.substr( str_begin_ - first_index, str_end_ - str_begin_));
      bytes_pending_ += str_end_ - str_begin_;
    } else {

      uint64_t str_ptr_ = str_begin_;

      for ( auto it = storage_.begin(); it != storage_.end(); it++ ) {
        if ( str_ptr_ < it->first ) {
          uint64_t substr_end_ = min( str_end_, it->first );

          cout << "str_ptr_ = " << str_ptr_ << ", substr_end_ = " << substr_end_
               << " , first_index = " << first_index
               << ", content = " << data.substr( str_ptr_ - first_index, substr_end_ - str_ptr_ ) << endl;
          storage_.emplace( str_ptr_, data.substr( str_ptr_ - first_index, substr_end_ - str_ptr_ ) );
          bytes_pending_ += substr_end_ - str_ptr_;

          if ( str_end_ <= it->first ) {
            break;
          }
          str_ptr_ = it->first;
        }

        if ( str_end_ < it->first + it->second.size() ) {
          break;
        }

        str_ptr_ = it->first + it->second.size();
      }
    }
  }

  for ( auto it = storage_.begin(); it != storage_.end(); it++ ) {
    if ( it->first == writer().bytes_pushed() ) {
      cout << "idx = " << it->first << ", size = " << it->second.size() << " , content = " << it->second << endl;
      const_cast<Writer&>( writer() ).push( it->second );

      bytes_pending_ -= it->second.size();
      storage_.erase( it);
    } else {
      break;
    }
  }

  if ( writer().bytes_pushed() == bytes_last_ ) {
    // cout << "close writer !" << endl;
    const_cast<Writer&>( writer() ).close();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return bytes_pending_;
}
