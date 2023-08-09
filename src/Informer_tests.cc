#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <memory>
#include <optional>
#include <functional>

class Receiver;
class Reporter;

namespace Informer {
struct ReporterForTest: public Reporter {
  ReporterForTest(std::string identifierArg,
                  std::function<std::optional<std::string>()> handle):
    identifier(identifierArg), reportHandle(handle) {}

  std::optional<std::string> report() const {
    return reportHandle();
  };

  std::string identifier;
  std::function<std::optional<std::string>()> reportHandle;
};

struct SingleProcTest: public ::testing::Test {
  void SetUp() override {
    informer = std::make_unique<Receiver>();
    reporters.emplace_back("ident1", [](const Reporter& s) {
      return "information from ident1";
    });
    reporters.emplace_back("ident2", [](const Reporter& s) {
      return "information from ident2";
    });

    informer->register("ident1", reporters[0].get());
    informer->register("ident2", reporters[1].get());
  }

  std::unique_ptr<Receiver> informer;
  using Reporters = std::vector<std::unique_ptr<Reporter>>;
  Reporters reporters;
};

RC_GTEST_FIXTURE_PROP(SingleProcTest, Basics, ()) {
  // Retrieve from a single reporter
  std::string info = informer->retrieve("ident1");
  info = informer->retrieve("ident2");

  std::vector<std::string> informer->retrieves();
}

#ifdef __linux__
struct MultiProcTest: public ::testing::Test {
  void SetUp() override {
    informer = std::make_unique<Receiver>();
  }

  std::unique_ptr<Receiver> informer;
  using Reporters = std::vector<std::unique_ptr<Reporter>>;
  Reporters reporters;
};
#endif // __linux__

} // Informer
