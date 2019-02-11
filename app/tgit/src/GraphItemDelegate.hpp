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
  QSize decorationSize(QSize previousSize, const QModelIndex& index) const;
};

#endif // GRAPHITEMDELEGATE_HPP
