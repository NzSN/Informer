
#ifndef RECEIVER_INL_H
#define RECEIVER_INL_H

#include "receiver.h"
#include <algorithm>

namespace Informer {

template<typename T>
typename Receiver<T>::STATE Receiver<T>::addReporter(
  std::unique_ptr<Reporter<T>>&& reporter) {

  const auto& ret = reporters.emplace(
      reporter->identifier(),
      std::move(reporter));
  if (!std::get<1>(ret)) {
    return ERROR;
  }

  return OK;
}

template<typename T>
std::optional<typename Receiver<T>::Report>
Receiver<T>::retrieve(std::string reporterID) const {
  auto iter = reporters.find(reporterID);
  if (iter == std::end(reporters)) {
    return std::nullopt;
  }

  return iter->second->report();
}

template<typename T>
typename Receiver<T>::ReportsWithFailedCases
Receiver<T>::retrieveAll() const {
  Reports reports;
  std::vector<std::string> fails;

  if (reporters.empty()) {
    return {{}, {}};
  }

  std::for_each(
    std::cbegin(reporters), std::cend(reporters),
    [&](std::pair<const std::string, std::unique_ptr<Reporter<T>>>
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

#endif /* RECEIVER-INL_H */
