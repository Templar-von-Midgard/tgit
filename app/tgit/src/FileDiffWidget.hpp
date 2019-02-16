#ifndef FILEDIFFWIDGET_HPP
#define FILEDIFFWIDGET_HPP

#include <cmath>
#include <memory>
#include <tuple>
#include <vector>

#include <QtWidgets/QSplitter>

namespace gitpp {
struct DeltaDetails;
}

namespace Ui {
class FileDiffWidget;
}

struct Region {
  int First;
  int Last;

  constexpr bool contains(int value) const noexcept {
    return First <= value && value <= Last;
  }

  constexpr void extend(int value) noexcept {
    if (value < First) {
      First = value;
    }
    if (Last < value) {
      Last = value;
    }
  }

  constexpr int offsetOf(int value) const noexcept {
    return value - First;
  }

  constexpr int length() const noexcept {
    return Last - First + 1;
  }
};

struct Mapping {
  enum Kind { Context, Addition, Deletion } Kind;
  Region Left;
  Region Right;

  int leftToRight(int value) const noexcept {
    double offset = Left.offsetOf(value);
    auto ratio = offset / Left.length();
    return std::round(Right.First + Right.length() * ratio);
  }
  int rightToLeft(int value) const noexcept {
    double offset = Right.offsetOf(value);
    auto ratio = offset / Right.length();
    return std::round(Left.First + Left.length() * ratio);
  }
};

class FileDiffWidget : public QSplitter {
  Q_OBJECT
public:
  explicit FileDiffWidget(QWidget* parent = nullptr);
  ~FileDiffWidget();

  void setFile(const gitpp::DeltaDetails& file, QString leftContents, QString rightContents);

private:
  void highlightLines(const gitpp::DeltaDetails& file);

  std::unique_ptr<Ui::FileDiffWidget> Ui;

  std::vector<Mapping> LineMapping;
  bool ScrollLocked = false;
};

#endif // FILEDIFFWIDGET_HPP
