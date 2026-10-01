#if !defined(WATERFALL_HPP)
#define WATERFALL_HPP


#include <cstddef>
#include <complex>
#include <vector>

#include "fftwf_plan_t.hpp"


class Waterfall
{
  using fc32_t = std::complex<float>;

public:

  Waterfall(std::size_t fft_len,
            float ref_db = -30.f,
            float range_db = 80.f) noexcept;

  void fft(std::span<fc32_t> input, std::span<float> output) noexcept;


private:

  std::array<float,3> rgb(float db) noexcept;

  std::size_t fft_len_;
  fftwf_plan_t fftw_plan_;
  float ref_db_;
  float range_db_;

  fftwf_complex* fftwf_result_;
};


#endif  // WATERFALL_HPP