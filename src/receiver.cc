
#include "receiver.h"
#include <algorithm>

namespace Informer {

Receiver::STATE Receiver::addReporter(
  std::unique_ptr<Reporter>&& reporter) {

  const auto& ret = reporters.emplace(
      reporter->identifier(),
      std::move(reporter));
  if (!std::get<1>(ret)) {
    return ERROR;
  }

  return OK;
}

std::optional<Receiver::Report>
Receiver::retrieve(std::string reporterID) const {
  auto iter = reporters.find(reporterID);
  if (iter == std::end(reporters)) {
    return std::nullopt;
  }

  return iter->second->report();
}

Receiver::ReportsWithFailedCases
Receiver::retrieveAll() const {
  Reports reports;
  std::vector<std::string> fails;

  if (reporters.empty()) {
    return {{}, {}};
  }

  std::for_each(
    std::cbegin(reporters), std::cend(reporters),
    [&](std::pair<const std::string, std::unique_ptr<Reporter>>
       const& p) {

      const auto& report = std::get<1>(p)->report();
      if (report.has_value()) {
        reports.emplace(std::get<0>(p), report.value());
      } else {
        fails.push_back(std::get<0>(p));
      }
    });

  return std::make_tuple(reports, fails);
}

} // Informer
