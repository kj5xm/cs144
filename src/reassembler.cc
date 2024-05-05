#include "reassembler.hh"
#include "byte_stream.hh"
#include <cstdint>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  if(is_last_substring) {
    const_cast<Writer&>(writer()).close();
  }

  first_unassembled_ = writer().bytes_pushed();
  first_unaccepted_ = writer().bytes_pushed() + writer().available_capacity();

  if(first_index + data.size() - 1 < first_unassembled_ || first_index >= first_unaccepted_ ) {
     return;
  }

    uint64_t str_begin_ = first_index;
    uint64_t str_end_ = first_index + data.size();
    //uint64_t new_size_ = data.size();

  if(first_index < first_unassembled_) {
    str_begin_ = first_unassembled_;
  }

  if(first_index + data.size() > first_unaccepted_) {
    str_end_ = first_unaccepted_;
  }

  uint64_t str_ptr_ = str_begin_;
  for(auto it = storage_.begin(); it != storage_.end(); it++) {
    if(str_ptr_ < it->first) {
      uint64_t substr_end_ = min(str_end_, it->first);

      storage_.emplace(str_ptr_, data.substr(str_ptr_ - first_index, substr_end_ - str_ptr_));
      if(str_end_ < it->first) {
        break;
      }
      str_ptr_ = it->first;
    }

    if(str_end_ < it->first + it->second.size()) {
      break;
    }
    str_ptr_ = it->first + it->second.size();
  }

}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return  last_internal_ - writer().bytes_pushed();
}
