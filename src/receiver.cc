
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

std::optional<Receiver::Reports>
Receiver::retrieveAll() const {
  Reports reports;

  if (reporters.empty()) {
    return std::nullopt;
  }

  std::for_each(
    std::cbegin(reporters), std::cend(reporters),
    [&](std::pair<const std::string, std::unique_ptr<Reporter>>
       const& p) {

      const auto& report = std::get<1>(p)->report();
      reports.emplace(std::get<0>(p),
                      report.has_value() ? report.value() : "" );
    });

  return reports;
}

} // Informer
