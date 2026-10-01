#include "Waterfall.hpp"

#include <algorithm>
#include <complex>
#include <fftw3.h>


Waterfall::Waterfall(std::size_t fft_len,
                     float ref_db,
                     float range_db) noexcept
: fft_len_{fft_len},
  fftw_plan_{fft_len},
  ref_db_{ref_db},
  range_db_{range_db}
{
  fftwf_result_ =
    static_cast<fftwf_complex*>(fftwf_malloc(sizeof(fftwf_complex)*fft_len_));
}


void Waterfall::fft(std::span<fc32_t> input,
                    std::span<float> output) noexcept
{
  fftwf_execute_dft(
    fftw_plan_.get(),
    reinterpret_cast<fftwf_complex*>(input.data()),
    fftwf_result_
  );

  std::ranges::transform(
    fftwf_result_,
    fftwf_result_ + fft_len_,
    output.begin(),
    [](auto&& c) {
      return 10.0f * log10f(std::norm(reinterpret_cast<fc32_t&>(c))) - 1e-20f;
    }
  );
}


std::array<float,3> Waterfall::rgb(float db) noexcept
{
  float const t = std::clamp((ref_db_ - db) / range_db_, 0.f, 1.f);

  return{
    std::clamp(1.6f * t - 0.1f, 0.f, 1.f),
    std::clamp(1.9f * t - 1.1f, 0.f, 1.f),
    std::clamp(1.f - 1.4f * t, 0.f, 0.5f * t + 0.025f)
  };
}