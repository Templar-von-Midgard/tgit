#ifndef REFERENCEITEMDELEGATE
#define REFERENCEITEMDELEGATE

#include <QtWidgets/QStyledItemDelegate>

class QSvgRenderer;

class ReferenceItemDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  explicit ReferenceItemDelegate(QObject* parent = nullptr) noexcept;

  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

protected:
  void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;

private:
  QSvgRenderer* ArrowUpIcon;
  qreal ArrowUpIconAspectRatio;
  QSvgRenderer* ArrowDownIcon;
  qreal ArrowDownIconAspectRatio;
};

#endif
