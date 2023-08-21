
#ifndef RECEIVER_H
#define RECEIVER_H

#include <utility>
#include <string>
#include <memory>
#include <map>
#include "reporter.h"

namespace Informer {

// Receiver is responsible for pulling information
// from Reporter (that may resied in local or remote process).
template<typename T>
class Receiver {
public:

  enum STATE {
    OK,
    ERROR,
  };

  using Report = T;
  using Reports = std::map<std::string, T>;
  using ReportsWithFailedCases =
    std::tuple<Reports, std::vector<std::string>>;

  STATE addReporter(std::unique_ptr<Reporter<T>>&& reporter);

  std::optional<Report> retrieve(std::string reporterID) const;

  ReportsWithFailedCases retrieveAll() const;
private:
  std::map<std::string, std::unique_ptr<Reporter<T>>> reporters;
};

} // Informer


#endif /* RECEIVER_H */
