#ifndef GRAPHITEMDELEGATE_HPP
#define GRAPHITEMDELEGATE_HPP

#include <QtWidgets/QStyledItemDelegate>

class GraphItemDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  explicit GraphItemDelegate(QObject* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

protected:
  void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;

private:
  QSize decorationSize(const QStyleOptionViewItem& option, const QModelIndex& index) const;

  QBrush LocalBranchBackground;
  QBrush RemoteBranchBackground;
  QBrush TagBackground;
  QBrush NoteBackground;
};

#endif // GRAPHITEMDELEGATE_HPP
