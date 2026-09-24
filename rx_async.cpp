#include <print>

#include "rtlsdrpp/rtlsdrpp.hpp"
#include "rtlsdrpp/dev_t.hpp"


template <typename Context>
void to_stdout(std::span<rtlsdrpp::cu8_t> buffer, Context& ctx)
{
  static int count;

  std::println("fatto {}!", count++);
}


int main(void)
{
  constexpr auto sample_rate = 2'048'000u;
  auto const ctx = nullptr;
  
  rtlsdrpp::dev_t device;
  device.set_center_frequency(97'000'000u);
  device.set_turner_gain_mode(rtlsdrpp::turner_gain_mode_e::automatic);
  device.set_sample_rate(sample_rate);
  device.set_turner_bandwidth(250'000u);

  device.reset_buffer();
  device.read_async(to_stdout, ctx);


  return EXIT_SUCCESS;
}