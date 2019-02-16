#ifndef COMMITVIEW_HPP
#define COMMITVIEW_HPP

#include <vector>

#include <QtCore/QDateTime>
#include <QtCore/QString>

namespace gitpp {
class Commit;
}

class DiffView;

struct CommitView {
  const gitpp::Commit& commit;

  QString shortId() const noexcept;
  QString summary() const noexcept;
  QString message() const noexcept;
  QDateTime creation() const noexcept;
  QString author() const noexcept;
};

#endif // COMMITVIEW_HPP
