/**
 * Stream samples to a file.
 * Inspired by the uhd `rx_samples_to_file.cpp` example program.
 */


#include <chrono>
#include <concepts>
#include <cstdlib>
#include <iostream>
#include <string>
#include <csignal>

#include <rtlsdrpp/rtlsdrpp.hpp>
#include <rtlsdrpp/dev_t.hpp>
#include <rtlsdrpp/utility.hpp>

#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/any.hpp>

#include "./common.h"


namespace po = boost::program_options;


using cu8_vector_t = std::vector<rtlsdrpp::cu8_t>;


// unfortunately we nee to use some global variables
rtlsdrpp::dev_t* device_ptr;
std::vector<cu8_vector_t>* memory_ptr; // buffers in async read will be store here
po::variables_map vm; // contains the program options


/// @brief Used to parse the `--duration` option.
struct DurationOption
{
  std::variant<std::chrono::milliseconds,std::size_t> value;

  /// @brief Returns the number of samples derived from this option.
  std::size_t nsamples() const
  {
    return std::visit(
      [](auto arg)
      {
        using Type = std::decay_t<decltype(arg)>;

        if constexpr (std::same_as<Type,std::chrono::milliseconds>) {
          return rtlsdrpp::nsamples(arg);
        } else {
          return arg;
        }
      },
      value
    );
  }
};


/// @brief Custom validator for `DurationOption`.
void validate(boost::any& v,
              std::vector<std::string> const& values,
              DurationOption*,
              int)
{
  po::check_first_occurrence(v);
  std::string const& s =
    po::validators::get_single_string(values);

  if (s.ends_with("ms")) {
    auto const value = std::stoul(s.substr(0, s.size()-2));
    v = boost::any{DurationOption{std::chrono::milliseconds{value}}};
  }
  else if (s.ends_with("s")) {
    auto const value = std::stoul(s.substr(0, s.size()-1));
    v = boost::any{DurationOption{std::chrono::seconds{value}}};
  }
  else
  {
    auto const value = std::stoul(s);
    v = boost::any{DurationOption{std::size_t{value}}};
  }
}


/// @brief Write buffer to file according to the sample type.
template <typename Type>
void write_to_file(std::vector<Type> const& buffer,
                   std::string const& type,
                   std::string const& filename)
{
  if (type == "cu8") {
    to_file(buffer, filename+".cu8");
  }
  else
  {
    std::vector<rtlsdrpp::fc32_t> samples_fc32(buffer.size());
    rtlsdrpp::cu8tofc32(buffer, samples_fc32);

    to_file(samples_fc32, filename+".fc32");
  }
}


/// @brief Signal handler when the user press ctrl+c: stop the async stream.
void sigint_handler(int)
{
  if (device_ptr) {
    device_ptr->cancel_async();
  } else {
    throw std::runtime_error{"sigint_handler(int): `device_ptr` is not valid"};
  }

  if (!memory_ptr) {
    throw std::runtime_error{"sigint_handler(int): `memory_ptr` is not valid"};
  }

  std::println("Accumulating...");

  std::vector<rtlsdrpp::cu8_t> sum;
  std::ranges::for_each(
    *memory_ptr,
    [&sum](auto const& buffer) {
      sum.insert(sum.end(), buffer.begin(), buffer.end());
    }
  );

  write_to_file(sum, vm.at("type").as<std::string>(), "samples");
}


/// @brief Callback invoked by `rtlsdrpp::read_async`: write a single async read into a temporary memory.
template <typename Context>
void to_memory(std::span<rtlsdrpp::cu8_t> buffer, Context& ctx)
{
  auto* memory_ptr = reinterpret_cast<std::vector<cu8_vector_t>*>(&ctx);

  auto& out = memory_ptr->emplace_back(buffer.size());
  std::ranges::copy(buffer, out.begin());
}


void read_sync(rtlsdrpp::dev_t& device)
{
  auto const nsamples = vm.at("duration").as<DurationOption>().nsamples();
  std::vector<rtlsdrpp::cu8_t> samples_cu8(nsamples);

  std::println("Reading synchronously {} samples...", nsamples);

  int read_count;
  device.read_sync(samples_cu8, read_count);

  write_to_file(
    samples_cu8,
    vm.at("type").as<std::string>(),
    vm.at("filename").as<std::string>()
  );
}


void read_async(rtlsdrpp::dev_t& device)
{
  // catch ctrl+c
  struct sigaction sa;
  sa.sa_handler = sigint_handler;
  sigaction(SIGINT, &sa, nullptr);

  device_ptr = &device;

  std::vector<cu8_vector_t> memory;
  memory_ptr = &memory;

  std::println("Reading asynchronously (press ctrl+c to stop)...");

  device.read_async(to_memory, memory);
}


int main(int argc, char* argv[])
{
  //* parse command line options / / / / / / / / / / / / / / / / / / / / / / / /

  po::options_description desc{"Allowed options"};
  desc.add_options()
    ("help", "Produce this help message")
    (
      "filename",
      po::value<std::string>()->default_value("samples"),
      "Name of the output binary file (format type will be appended)."
    )
    (
      "type,t",
      po::value<std::string>()->default_value("cu8"),
      "Data format for the output file: cu8 (default, over-the-wire format), "
      "fc32 (32-bit float).\n"
      "Note that choosing fc32 does not improve precision."
    )
    (
      "duration,d",
      po::value<DurationOption>(),
      "Streaming duration in second (s), millisecond (ms) or number of samples."
      "\nIf not set, data will be continuously received."
    )
    (
      "freq,f",
      po::value<double>()->required(),
      "Center frequency."
    )
    (
      "rate,r",
      po::value<double>()->default_value(2048e3),
      "Sample rate (default 2048e3)."
    )
    (
      "bw,b",
      po::value<double>()->default_value(250e3),
      "Bandwidth (default: 250e3)."
    )
  ;

  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    desc.print(std::cout);
    std::exit(EXIT_SUCCESS);
  }

  //* program starts here / / / / / / / / / / / / / / / / / / / / / / / / / / /

  rtlsdrpp::dev_t device;
  device.set_center_frequency(vm.at("freq").as<double>());
  device.set_turner_gain_mode(rtlsdrpp::turner_gain_mode_e::automatic);
  device.set_sample_rate(vm.at("rate").as<double>());
  device.set_turner_bandwidth(vm.at("bw").as<double>());
  device.reset_buffer();

  if (vm.count("duration")) {
    read_sync(device);
  } else {  // no duration
    read_async(device);
  }

  return EXIT_SUCCESS;
}