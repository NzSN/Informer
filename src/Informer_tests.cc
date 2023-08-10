#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <memory>
#include <optional>
#include <functional>

#include <iostream>
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

struct SingleProcTest: public ::testing::Test {
  void SetUp() override {
    informer = std::make_unique<Receiver>();

    numOfReports = *rc::gen::inRange(1, 100);
    for (int i = 0; i < numOfReports; ++i) {
      std::string ident = *rc::gen::arbitrary<std::string>();
      idents.push_back(ident);

      std::unique_ptr<Reporter> r = std::make_unique<ReporterForTest>(ident, [=]() {
        return ident;
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
    RC_ASSERT(report.has_value());
    RC_ASSERT(report.value() == idents[i]);
  }

  std::optional<Receiver::Reports> reports = informer->retrieveAll();
  RC_ASSERT(reports.has_value());
  for (int i = 0; i < numOfReports; ++i) {
    auto iter = reports.value().find(idents[i]);
    bool isFound = iter != std::end(reports.value());
    RC_ASSERT(isFound);
    RC_ASSERT(iter->second == idents[i]);
  }
}

} // Informer
