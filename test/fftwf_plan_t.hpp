#if !defined(FFTW_PLAN_HPP)
#define FFTW_PLAN_HPP


#include <mutex>
#include <memory>

#include <fftw3.h>


/**
 * RAII wrapper around a `fftwf_plan_s` from `fftw3` library.
 */
class fftwf_plan_t
{
public:

  fftwf_plan_t(std::size_t const fft_len) noexcept
  {
    fftwf_plan_s* fftwf_plan;

    {
      std::scoped_lock lock{fft_mutex};

      fftwf_plan =
        fftwf_plan_dft_1d(
          fft_len,
          nullptr,
          nullptr,
          FFTW_FORWARD,
          FFTW_ESTIMATE// or FFTW_MEASURE
        );
    }

    fftwf_plan_.reset(fftwf_plan);
  };

  /// @brief returns a pointer to the managed object.
  auto get() { return fftwf_plan_.get(); }

private:

  struct deleter {
    void operator()(fftwf_plan_s* p) { fftwf_destroy_plan(p); }
  };

  static inline std::mutex fft_mutex;  // because `fftw_plan_dft_1d` is not thread-safe

  std::unique_ptr<fftwf_plan_s,deleter> fftwf_plan_;
};


#endif  // FFTW_PLAN_HPP