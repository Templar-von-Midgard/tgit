#include "ReferenceItemDelegate.hpp"

#include <algorithm>

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtSvg/QSvgRenderer>

#include "ReferencesModel.hpp"

namespace {

constexpr qreal StatusPadding = 2.5;

QFont statusFont(QFont mainFont) {
  mainFont.setPointSizeF(mainFont.pointSizeF() / 1.8);
  return mainFont;
}

} // namespace

ReferenceItemDelegate::ReferenceItemDelegate(QObject* parent) noexcept
    : QStyledItemDelegate(parent), ArrowUpIcon(new QSvgRenderer(QStringLiteral(":/images/half-arrow-up"), this)),
      ArrowUpIconAspectRatio(ArrowUpIcon->viewBoxF().width() / ArrowUpIcon->viewBoxF().height()),
      ArrowDownIcon(new QSvgRenderer(QStringLiteral(":/images/half-arrow-down"), this)),
      ArrowDownIconAspectRatio(ArrowDownIcon->viewBoxF().width() / ArrowDownIcon->viewBoxF().height()) {
}

void ReferenceItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const {
  auto o = option;
  initStyleOption(&o, index);

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);

  if (o.features & QStyleOptionViewItem::HasDecoration) {
    auto baseRect = o.rect;
    baseRect.setWidth(o.decorationSize.width());
    auto localBranch = std::get<LocalBranch>(index.data(ReferencesModel::DataRole).value<Reference>());
    if (localBranch.Upstream.isEmpty()) {
      painter->drawText(baseRect, Qt::AlignCenter, "?");
    } else if (localBranch.AheadBy == 0 && localBranch.BehindBy == 0) {
      painter->drawText(baseRect, Qt::AlignCenter, "=");
    } else {
      painter->setFont(statusFont(o.font));
      QRectF arrowUpRect = baseRect.adjusted(StatusPadding, StatusPadding, 0, -StatusPadding);
      arrowUpRect.setWidth(arrowUpRect.height() * ArrowUpIconAspectRatio);

      QRectF arrowDownRect = baseRect.adjusted(StatusPadding, StatusPadding, 0, -StatusPadding);
      arrowDownRect.setWidth(arrowDownRect.height() * ArrowDownIconAspectRatio);

      auto topRect = baseRect.adjusted(std::max(arrowUpRect.width(), arrowDownRect.width()) + StatusPadding, 0, 0, 0);
      topRect.setHeight(baseRect.height() / 2);
      auto bottomRect = topRect;
      bottomRect.setTop(topRect.bottom());
      bottomRect.setHeight(baseRect.height() / 2);

      if (localBranch.AheadBy > 0) {
        painter->drawText(topRect, Qt::AlignCenter, QString::number(localBranch.AheadBy));
        ArrowUpIcon->render(painter, arrowUpRect);
      }
      if (localBranch.BehindBy > 0) {
        painter->drawText(bottomRect, Qt::AlignCenter, QString::number(localBranch.BehindBy));
        ArrowDownIcon->render(painter, arrowDownRect);
      }
    }
  }

  painter->restore();
  QStyledItemDelegate::paint(painter, option, index);
}

void ReferenceItemDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
  QStyledItemDelegate::initStyleOption(option, index);
  auto data = index.data(ReferencesModel::DataRole);
  if (data.isValid() && std::holds_alternative<LocalBranch>(data.value<Reference>())) {
    option->features |= QStyleOptionViewItem::HasDecoration;
    int width = 0;
    const auto& localBranch = std::get<LocalBranch>(data.value<Reference>());
    if (localBranch.Upstream.isEmpty()) {
      width = option->fontMetrics.width('?');
    } else if (localBranch.AheadBy == 0 && localBranch.BehindBy == 0) {
      width = option->fontMetrics.width('=');
    } else {
      auto font = statusFont(option->font);
      QFontMetrics fm(font);
      width = std::max(fm.width(QString::number(localBranch.AheadBy)), fm.width(QString::number(localBranch.BehindBy)));
      width += option->decorationSize.height() * std::max(ArrowUpIconAspectRatio, ArrowDownIconAspectRatio);
    }

    option->decorationSize = QSize(width + StatusPadding * 2, option->rect.height());
  }
}
