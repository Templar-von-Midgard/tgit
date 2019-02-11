#include "GraphItemDelegate.hpp"

#include <algorithm>

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

#include "GraphRow.hpp"

namespace {

constexpr qreal BranchSpacing = 5;
constexpr qreal CommitNodeWidth = 8;
constexpr qreal CommitNodeHeight = 8;

constexpr qreal EdgeWidth = 1.5;
constexpr qreal NodeWidth = 2.5;

int laneCount(const PathList& paths) noexcept;

} // namespace

GraphItemDelegate::GraphItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {
}

void GraphItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  auto o = option;
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setClipping(true);
  painter->setClipRect(option.rect);

  if (o.state & QStyle::State_Selected) {
    painter->fillRect(o.rect, o.palette.highlight());
    o.state &= ~QStyle::State_Selected;
  }

  const auto [commitIndex, paths] = index.data(Qt::DecorationRole).value<GraphRow>();
  const QRectF rect = o.rect;
  const QPointF origin = rect.topLeft();
  const qreal commitNodeY0 = origin.y() + BranchSpacing;
  const qreal commitNodeY1 = commitNodeY0 + CommitNodeHeight;
  const qreal centerY = rect.center().y();

  QVector<QPointF> lines;
  if (index.row() > 0) {
    std::vector<int> sources;
    for (auto [source, _] : paths) {
      const auto begin = sources.begin();
      const auto end = sources.end();
      if (std::find(begin, end, source) == end) {
        sources.push_back(source);
      }
    }
    if (sources.empty()) {
      sources.push_back(0);
    }
    for (auto i : sources) {
      qreal x = origin.x() + i * CommitNodeWidth + (i + 1) * BranchSpacing + CommitNodeWidth / 2.0;
      qreal y1 = commitIndex == i ? commitNodeY0 : centerY;
      QPointF sourcePoint{x, origin.y()};
      QPointF destinationPoint{x, y1};
      if (rect.contains(sourcePoint) || rect.contains(destinationPoint)) {
        lines.push_back(sourcePoint);
        lines.push_back(destinationPoint);
      }
    }
  }

  for (auto [source, destination] : paths) {
    qreal x0 = origin.x() + source * CommitNodeWidth + (source + 1) * BranchSpacing + CommitNodeWidth / 2.0;
    qreal x1 = origin.x() + destination * CommitNodeWidth + (destination + 1) * BranchSpacing + CommitNodeWidth / 2.0;
    qreal y0 = source == commitIndex ? commitNodeY1 : centerY;
    qreal y1 = rect.bottom();
    QPointF sourcePoint{x0, y0};
    QPointF destinationPoint{x1, y1};
    if (!rect.contains(sourcePoint) && !rect.contains(destinationPoint)) {
      continue;
    }
    if (source == destination) {
      lines.push_back(sourcePoint);
      lines.push_back(destinationPoint);
    } else if (source < destination) {
      QPainterPath path{sourcePoint};
      QPointF control1{x0 + BranchSpacing, y1};
      QPointF control2{x1 - BranchSpacing, y0};
      path.cubicTo(control1, control2, destinationPoint);
      painter->drawPath(path);
    } else {
      QPainterPath path{sourcePoint};
      QPointF control1{x0 - BranchSpacing, y1};
      QPointF control2{x1 + BranchSpacing, y0};
      path.cubicTo(control1, control2, destinationPoint);
      painter->drawPath(path);
    }
  }

  painter->drawLines(lines);

  const qreal x = origin.x() + commitIndex * CommitNodeWidth + (commitIndex + 1) * BranchSpacing;
  QPen nodePen = painter->pen();
  nodePen.setBrush(option.palette.foreground());
  nodePen.setWidthF(NodeWidth);
  painter->setPen(nodePen);
  painter->drawEllipse(x, commitNodeY0, CommitNodeWidth, CommitNodeHeight);

  painter->restore();
  QStyledItemDelegate::paint(painter, o, index);
}

void GraphItemDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
  QStyledItemDelegate::initStyleOption(option, index);
  option->decorationPosition = QStyleOptionViewItem::Left;
  option->decorationSize = decorationSize(option->decorationSize, index);
  option->decorationAlignment = Qt::AlignLeft;
  option->features &= ~QStyleOptionViewItem::WrapText;
}

QSize GraphItemDelegate::decorationSize(QSize previousSize, const QModelIndex& index) const {
  auto&& paths = index.data(Qt::DecorationRole).value<GraphRow>().Paths;
  auto lanes = laneCount(paths);
  auto width = lanes * CommitNodeWidth + (lanes + 1) * BranchSpacing;
  previousSize.setWidth(width);
  return previousSize;
}

namespace {

int laneCount(const PathList& paths) noexcept {
  int max = 0;
  for (const auto [source, destination] : paths) {
    if (source > max) {
      max = source;
    }
    if (destination > max) {
      max = destination;
    }
  }
  return max + 1;
}

} // namespace
