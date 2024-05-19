#include "wrapping_integers.hh"
#include <cstdint>

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return zero_point + n;
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  int32_t offset = raw_value_ - wrap(checkpoint, zero_point).raw_value_;
  int64_t result = offset + checkpoint;
  result = result < 0 ? result + (1UL << 32) : result;
  return result;
}
