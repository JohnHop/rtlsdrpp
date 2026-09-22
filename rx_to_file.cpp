#include <chrono>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <ios>
#include <complex>

#include <rtlsdrpp/rtlsdrpp.hpp>
#include <rtlsdrpp/utility.hpp>


template <typename Type>
void to_file(std::vector<Type> const& buffer, 
             std::filesystem::path const& filename)
{
  std::ofstream ofs{filename, std::ios_base::binary | std::ios_base::out};

  ofs.write(
    reinterpret_cast<char const*>(buffer.data()), 
    buffer.size()*sizeof(Type)
  );
}


[[nodiscard]] constexpr
std::size_t stream_size(std::chrono::milliseconds ms, 
                        unsigned const sample_rate) noexcept
{
  return ms.count() * (sample_rate / 1'000) * 2;
}


int main(int argc, char* argv[])
{
  using fc32_t = std::complex<float>;

  auto constexpr sample_rate = 2'048'000u;
  
  rtlsdrpp::dev_t device;
  device.set_center_frequency(97'000'000u);
  device.set_turner_gain_mode(rtlsdrpp::turner_gain_mode_e::automatic);
  device.set_sample_rate(sample_rate);
  device.set_turner_bandwidth(250'000u);

  auto const buffer_size = 
    stream_size(std::chrono::milliseconds{100}, sample_rate);
  std::vector<uint8_t> buffer(buffer_size);

  int read_count;
  device.reset_buffer();
  device.read_sync(buffer, read_count);

  to_file(buffer, "buffer.cu8");

  std::vector<fc32_t> samples(buffer.size() / 2);
  rtlsdrpp::cu8tofc32(buffer, samples);

  to_file(samples, "samples.fc32");
  
  return EXIT_SUCCESS;
}