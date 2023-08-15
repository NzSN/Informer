#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <memory>
#include <optional>
#include <functional>
#include <algorithm>
#include <iostream>
#include <array>
#include "reporter.h"
#include "receiver.h"

namespace Informer {
struct ReporterForTest: public Reporter {
  ReporterForTest(std::string identifier,
                  std::function<std::optional<std::string>()> handle):
    Reporter(identifier), reportHandle(handle) {}

  std::optional<std::string> report() override {
    return reportHandle();
  };

  std::function<std::optional<std::string>()> reportHandle;
};

struct ReporterForTestFailed: public ReporterForTest {
  ReporterForTestFailed(std::string identifier,
                        std::function<std::optional<std::string>()> handle):
    ReporterForTest(identifier, handle) {}

  std::optional<std::string> report() override {
    return std::nullopt;
  }
};

struct SingleProcTest: public ::testing::Test {
  void SetUp() override {
    informer = std::make_unique<Receiver>();

    numOfReports = *rc::gen::inRange(1, 100);
    for (int i = 0; i < numOfReports; ++i) {
      std::string ident = *rc::gen::arbitrary<std::string>();
      idents.push_back(ident);

      std::unique_ptr<Reporter> r = std::make_unique<ReporterForTest>(
        ident,
        [=]() -> std::optional<std::string> {
        return *rc::gen::arbitrary<bool>() ?
          std::optional<std::string>(ident) : std::nullopt;
      });
      informer->addReporter(std::move(r));
    }
  }

  int numOfReports;
  std::vector<std::string> idents;
  std::unique_ptr<Receiver> informer;
};

RC_GTEST_FIXTURE_PROP(SingleProcTest, Basics, ()) {
  // Retrieve from a single reporter
  for (int i = 0; i < numOfReports; ++i) {
    std::optional<Receiver::Report> report = informer->retrieve(idents[i]);
    if (report.has_value()) {
      RC_ASSERT(report.value() == idents[i]);
    }
  }

  // In this case, all reporter success to report information.
  Receiver::ReportsWithFailedCases
    reports = informer->retrieveAll();
  for (auto& i: idents) {
    auto iter = std::get<0>(reports).find(i);
    bool isFound = iter != std::end(std::get<0>(reports));
    if (isFound) {
      RC_ASSERT(iter->second == i);
    } else {
      std::array<std::string, 1> identRange{i};
      RC_ASSERT(std::search(
                  std::cbegin(std::get<1>(reports)),
                  std::cend(std::get<1>(reports)),
                  std::begin(identRange),
                  std::end(identRange)) != std::cend(std::get<1>(reports)));
    }
  }
}

} // Informer
