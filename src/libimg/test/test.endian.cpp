#include <img/endian.hpp>

#include <array>
#include <iostream>
#include <fstream>
#include <iterator>

int main(int argc, char* argv[]) {
	std::array<uint8_t, 4> data;

	// To write into a buffer with a certain endianness, from system byte order
	// Functions take an iterator to operate on, or a ptr
	img::endian::write<img::order::lil>(data.begin(), 0x12345678);
	img::endian::write<img::order::big>(data.begin(), 0x12345678);

	// To read from a buffer of certain endianness, into system byte order
	// Functions take an iterator to operate on, or a ptr
	auto lilbuf = img::endian::read<img::order::lil, uint32_t>(data.cbegin());
	auto bigbuf = img::endian::read<img::order::big, uint32_t>(data.cbegin());

	std::cout << std::hex << "0x" << lilbuf << std::endl;
	std::cout << std::hex << "0x" << bigbuf << std::endl;

	// Reading and writing a binary file with endian::stream
  std::unique_ptr<img::memory_buffer> in_file;
  try {
    in_file = img::read_buffer_from_file("./jet.bmp");
  } catch (int e) {
    std::cout << "could not find file jet.bmp" << std::endl;

    return 1;
  }

	auto val1 = in_file->read_as<img::order::big, uint32_t>();
	auto val2 = in_file->read_as<img::order::big, uint32_t>();
	auto val3 = in_file->read_as<img::order::big, uint32_t>();

	std::cout << "[big endian] at byte 0-3: 0x" << std::hex << val1 << std::endl;
	std::cout << "[big endian] at byte 4-7: 0x" << std::hex << val2 << std::endl;
	std::cout << "[big endian] at byte 8-11: 0x" << std::hex << val3 << std::endl;

	in_file->seek_to(0);

	val1 = in_file->read_as<img::order::lil, uint32_t>();
	val2 = in_file->read_as<img::order::lil, uint32_t>();
	val3 = in_file->read_as<img::order::lil, uint32_t>();

	std::cout << "[lil endian] at byte 0-3: 0x" << std::hex << val1 << std::endl;
	std::cout << "[lil endian] at byte 4-7: 0x" << std::hex << val2 << std::endl;
	std::cout << "[lil endian] at byte 8-11: 0x" << std::hex << val3 << std::endl;

	img::memory_buffer buf1(4);
	img::memory_buffer buf2(4);

	buf1.write_as<img::order::lil, uint32_t>(0x12345678);
	buf2.write_as<img::order::big, uint32_t>(0x12345678);

	img::write_buffer_to_file(buf1, "lil.bin");
	img::write_buffer_to_file(buf2, "big.bin");

  return 0;
}
