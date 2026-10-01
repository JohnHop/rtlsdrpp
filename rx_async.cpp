#include <print>

#include "rtlsdrpp/rtlsdrpp.hpp"
#include "rtlsdrpp/dev_t.hpp"
#include "rtlsdrpp/utility.hpp"

#include "Waterfall.hpp"


template <typename Context>
void to_stdout(std::span<rtlsdrpp::cu8_t> buffer_cu8, Context& ctx)
{
  static std::vector<rtlsdrpp::fc32_t> buffer_fc32(buffer_cu8.size());
  static std::vector<float> fft(buffer_fc32.size());

  auto& waterfall = reinterpret_cast<Waterfall&>(ctx);

  rtlsdrpp::cu8tofc32(buffer_cu8, buffer_fc32);

  waterfall.fft(buffer_fc32, fft);
}


int main(void)
{
  constexpr auto sample_rate = 2'048'000u;
  constexpr auto default_buffer_size = 262'144uz / 2; // per cu8

  rtlsdrpp::dev_t device;
  device.set_center_frequency(97'000'000u);
  device.set_turner_gain_mode(rtlsdrpp::turner_gain_mode_e::automatic);
  device.set_sample_rate(sample_rate);
  device.set_turner_bandwidth(250'000u);
  device.reset_buffer();

  Waterfall waterfall{default_buffer_size};
  device.read_async(to_stdout, waterfall, default_buffer_size);


  return EXIT_SUCCESS;
}