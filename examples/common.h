#include <algorithm>
#include <ranges>
#include <sstream>
#if !defined(JOHNHOP_RTLSDRPP_TEST_COMMON_H)
#define JOHNHOP_RTLSDRPP_TEST_COMMON_H


/**
 * Common definitions used across test source files.
 */

#include <vector>
#include <filesystem>
#include <fstream>
#include <print>


/**
 * Print a buffer's samples to a file.
 */
template <typename Type>
void to_file(std::vector<Type> const& buffer,
             std::filesystem::path const& filename)
{
  std::ofstream ofs{filename, std::ios::binary | std::ios::out};
  auto const byte_size = buffer.size()*sizeof(Type);

  ofs.write(reinterpret_cast<char const*>(buffer.data()), byte_size);

  std::println("Written {} bytes to {}", byte_size, filename.string());
}


/**
 * Print a range of values.
 */
template <std::ranges::input_range Range>
std::string to_string(Range const& range)
{
  std::ostringstream oss;

  std::ranges::for_each(range, [&oss](auto const& e) { oss << e << " "; });

  auto str = oss.str();
  str.pop_back(); // remove last space

  return str;
}


#endif  // JOHNHOP_RTLSDRPP_TEST_COMMON_H