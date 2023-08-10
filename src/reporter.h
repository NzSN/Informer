
#ifndef REPORTER_H
#define REPORTER_H

#include <optional>
#include <functional>
#include <string>

namespace Informer {

class Reporter {
public:
  Reporter(std::string identifier):
    identifier_(identifier) {}
  virtual ~Reporter() = default;

  // Leave the way how report datas are generated and
  // how transfer such datas to implementation of Reporter.
  virtual std::optional<std::string> report() = 0;
  std::string identifier() const {
    return identifier_;
  }
private:
  std::string identifier_;
};

} // Informer

#endif /* REPORTER_H */
