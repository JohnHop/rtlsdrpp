#include <chrono>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <print>

#include <rtlsdrpp/rtlsdrpp.hpp>
#include <rtlsdrpp/dev_t.hpp>
#include <rtlsdrpp/utility.hpp>


template <typename Type>
void to_file(std::vector<Type> const& buffer,
             std::filesystem::path const& filename)
{
  std::ofstream ofs{filename, std::ios::binary | std::ios::out};
  auto const byte_size = buffer.size()*sizeof(Type);

  ofs.write(reinterpret_cast<char const*>(buffer.data()), byte_size);

  std::println("Written {} bytes to {}", byte_size, filename.string());
}


[[nodiscard]] constexpr
std::size_t stream_size(std::chrono::milliseconds ms,
                        unsigned const sample_rate) noexcept
{
  return ms.count() * (sample_rate / 1'000);
}


int main(void)
{
  using namespace std::chrono_literals;

  constexpr auto sample_rate = 2'048'000u;
  constexpr auto stream_time = 100ms;

  rtlsdrpp::dev_t device;
  device.set_center_frequency(97'000'000u);
  device.set_turner_gain_mode(rtlsdrpp::turner_gain_mode_e::automatic);
  device.set_sample_rate(sample_rate);
  device.set_turner_bandwidth(250'000u);

  auto const buffer_size = stream_size(stream_time, sample_rate);
  std::vector<rtlsdrpp::cu8_t> samples_cu8(buffer_size);

  int read_count;
  device.reset_buffer();
  device.read_sync(samples_cu8, read_count);

  to_file(samples_cu8, "buffer.cu8");

  std::vector<rtlsdrpp::fc32_t> samples_fc32(samples_cu8.size());
  rtlsdrpp::cu8tofc32(samples_cu8, samples_fc32);

  to_file(samples_fc32, "samples.fc32");


  return EXIT_SUCCESS;
}