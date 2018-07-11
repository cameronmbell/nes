#ifndef _IMG_ENDIAN_HPP
#define _IMG_ENDIAN_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <type_traits>
#include <stdexcept>
#include <memory>

namespace img {
enum class order {
  big,
  lil
};

namespace endian {
  template <order Order, typename Out, typename Iter>
  constexpr typename std::enable_if<Order==order::big, Out>::type
  read(Iter in) noexcept {
    Out o = 0;

    for (int i = 0; i < sizeof(Out); i++) {
      o <<= 8;
      o |= static_cast<uint8_t>(*in++);
    }

    return o;
  }

  template <order Order, typename Out, typename Iter>
  constexpr typename std::enable_if<Order==order::lil, Out>::type
  read(Iter in) noexcept {
    Out o = 0;

    for (int i = 0; i < sizeof(Out); i++) {
      o |= static_cast<uint8_t>(*in++) << i * 8;
    }

    return o;
  }

  template <order Order, typename Iter, typename In>
  constexpr typename std::enable_if<Order==order::lil, void>::type
  write(Iter dst, const In& src) noexcept {
    for (int i = 0; i < sizeof(In); i++) {
      *dst++ = static_cast<uint8_t>((src >> (i * 8)) & 0xFF);
    }
  }

  template <order Order, typename Iter, typename In>
  constexpr typename std::enable_if<Order==order::big, void>::type
  write(Iter dst, In src) noexcept {
    for (int i = sizeof(In) - 1; i >= 0; i--) {
      *dst++ = static_cast<uint8_t>((src >> (i * 8)) & 0xFF);
    }
  }
}

class memory_buffer {
  std::size_t _size, _off;
  std::unique_ptr<char[]> _data;

public:
  static_assert(static_cast<std::size_t>(-1) >= 0, "std::size_t should be unsigned");

  template <typename Iter>
  memory_buffer(std::size_t size, const Iter begin, const Iter end)
    : _size(size), _off(0), _data(new char[size]) {
    static_assert(std::is_same<typename Iter::value_type, char>::value, "excepts char iterator");

    std::copy(begin, end, &_data[0]);
  }

  memory_buffer(std::size_t size)
    : _size(size), _off(0), _data(new char[size]) {
  }

  memory_buffer(std::size_t size, char fill)
    : _size(size), _off(0), _data(new char[size]) {
    std::fill(&_data[0], &_data[_size-1], fill);
  }

  memory_buffer(std::size_t size, char* from)
    : _size(size), _off(0), _data(from) {
  }

  memory_buffer(const std::string& bytestring)
    : memory_buffer(bytestring.size(), bytestring.cbegin(), bytestring.cend()) {
  }

  constexpr void seek_forward(std::size_t by) {
    _off += by;

    if (_off >= _size)
      throw std::out_of_range("offset past size");
  }

  constexpr void seek_back(std::size_t by) {
    _off -= by;

    if (_off >= _size)
      throw std::out_of_range("offset before size");
  }

  constexpr void seek_end(std::size_t by) {
    _off = _size - 1 - by;

    if (_off >= _size)
      throw std::out_of_range("offset before size");
  }

  constexpr void seek_to(std::size_t where) {
    _off = where;

    if (_off >= _size)
      throw std::out_of_range("bad offset");
  }

  constexpr std::size_t get_offset() const noexcept {
    return _off;
  }

  constexpr std::size_t get_size() const noexcept {
    return _size;
  }

  constexpr std::size_t get_remaining() const noexcept {
    return _size - _off - 1;
  }

  constexpr bool can_read(std::size_t bytes) const noexcept {
    return get_remaining() >= bytes;
  }

  constexpr bool can_write(std::size_t bytes) const noexcept {
    return get_remaining() >= bytes;
  }

  char* get() const noexcept {
    return _data.get();
  }

  void read(std::size_t bytes, char* out) const {
    if (!can_read(bytes))
      throw std::length_error("tried to read past end of stream");

    std::copy(&_data[_off], &_data[_off + bytes], out);
  }

  std::unique_ptr<char[]> read(std::size_t bytes) const {
    if (!can_read(bytes))
      throw std::length_error("tried to read past end of stream");

    std::unique_ptr<char[]> ret(new char[bytes]);
    std::copy(&_data[_off], &_data[_off + bytes], ret.get());

    return ret;
  }

  void write(std::size_t bytes, char* in) {
    if (!can_write(bytes))
      throw std::length_error("tried to write past end of stream");

    std::copy(&in[0], &in[bytes - 1], _data.get());
  }

  template <order Order, typename Out>
  Out read_as() noexcept {
    return endian::read<Order, Out>(&_data[_off]);
  }

  template <order Order, typename In>
  void write_as(In&& src) noexcept {
    endian::write<Order>(&_data[_off], std::forward<In>(src));
  }
};

std::unique_ptr<memory_buffer> read_buffer_from_file(const char* file_name) {
  std::ifstream file(file_name, std::ios::in | std::ios::binary);

  if (file) {
    file.seekg(0, std::ios::end);
    auto sz = file.tellg();
    char* mem = new char[sz];
    file.seekg(0, std::ios::beg);
    file.read(mem, sz);
    file.close();

    return std::unique_ptr<memory_buffer>(new memory_buffer(sz, mem));
  }

  throw errno;
}

void write_buffer_to_file(const memory_buffer& buf, const char* file_name) {
  std::ofstream file(file_name, std::ios::out | std::ios::binary);

  if (file) {
    file.write(buf.get(), buf.get_size());
    file.close();

    return;
  }

  throw errno;
}
}

#endif // _IMG_ENDIAN_HPP
