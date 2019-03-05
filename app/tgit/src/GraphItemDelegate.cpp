#include "GraphItemDelegate.hpp"

#include <algorithm>

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

#include <stx/algorithm/contains.hpp>
#include <stx/overload.hpp>

#include "GraphRow.hpp"
#include "HistoryModelAdaptor.hpp"
#include "Reference.hpp"

namespace {

constexpr qreal BranchSpacing = 5;
constexpr qreal CommitNodeWidth = 8;
constexpr qreal CommitNodeHeight = 8;

constexpr qreal EdgeWidth = 1.5;
constexpr qreal NodeWidth = 2.5;

constexpr qreal ReferenceMargin = 2.5;
constexpr qreal ReferencePadding = 5.0;

GraphRow graph(const QModelIndex& index) noexcept {
  return index.data(HistoryModelAdaptor::GraphRole).value<GraphRow>();
}

std::vector<Reference> references(const QModelIndex& index) noexcept {
  return index.data(HistoryModelAdaptor::ReferencesRole).value<std::vector<Reference>>();
}

int laneCount(const PathList& paths) noexcept;

QSize graphSizeHint(QSize previousSize, const GraphRow& graphRow) noexcept;
QSize referencesSizeHint(const QStyleOptionViewItem& option, const std::vector<Reference>& refs) noexcept;

QSize paintGraph(QPainter& painter, const QStyleOptionViewItem& option, const QModelIndex& index) noexcept;
void paintReference(QPainter& painter, QRectF rect, const Reference& ref, const QBrush& background) noexcept;

QFont referenceFont(const QStyleOptionViewItem& option) {
  return QFont(option.font.family(), 8);
}

} // namespace

GraphItemDelegate::GraphItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {
  LocalBranchBackground = Qt::yellow;
  RemoteBranchBackground = Qt::red;
  TagBackground = Qt::gray;
  NoteBackground = Qt::white;
}

void GraphItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  auto o = option;
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setClipping(true);
  painter->setClipRect(o.rect);

  if (o.state & QStyle::State_Selected) {
    painter->fillRect(o.rect, o.palette.highlight());
    o.state &= ~QStyle::State_Selected;
  }

  auto graphSize = paintGraph(*painter, o, index);

  int referencesX = o.rect.x() + graphSize.width() + ReferencePadding;

  painter->setFont(referenceFont(o));
  for (auto&& ref : references(index)) {
    int width = painter->fontMetrics().width(shortName(ref)) + 2 * ReferenceMargin;
    const auto& bg = std::visit(stx::overload{[this](const LocalBranch&) { return LocalBranchBackground; },
                                              [this](const RemoteBranch&) { return RemoteBranchBackground; },
                                              [this](const Tag&) { return TagBackground; },
                                              [this](const Note&) { return NoteBackground; }},
                                ref);
    paintReference(*painter, QRectF(referencesX, o.rect.top(), width, o.rect.height()), ref, bg);
    referencesX += width + ReferencePadding;
  }

  painter->restore();
  QStyledItemDelegate::paint(painter, o, index);
}

void GraphItemDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
  QStyledItemDelegate::initStyleOption(option, index);
  option->decorationPosition = QStyleOptionViewItem::Left;
  option->decorationSize = decorationSize(*option, index);
  option->decorationAlignment = Qt::AlignLeft;
  option->features &= ~QStyleOptionViewItem::WrapText;
  option->features |= QStyleOptionViewItem::HasDecoration;
}

QSize GraphItemDelegate::decorationSize(const QStyleOptionViewItem& option, const QModelIndex& index) const {
  auto graphSize = graphSizeHint(option.decorationSize, graph(index));
  auto refsSize = referencesSizeHint(option, references(index));
  return {graphSize.width() + refsSize.width(), std::max(graphSize.height(), refsSize.height())};
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

QSize graphSizeHint(QSize previousSize, const GraphRow& graphRow) noexcept {
  auto&& paths = graphRow.Paths;
  auto lanes = laneCount(paths);
  auto width = lanes * CommitNodeWidth + (lanes + 1) * BranchSpacing;
  previousSize.setWidth(width);
  return previousSize;
}

QSize referencesSizeHint(const QStyleOptionViewItem& option, const std::vector<Reference>& refs) noexcept {
  auto previousSize = option.decorationSize;
  auto font = referenceFont(option);
  QFontMetrics fm(font);
  int width = std::accumulate(refs.begin(), refs.end(), 0, [&fm](int acc, const Reference& ref) {
    return acc + fm.width(shortName(ref)) + ReferenceMargin * 2 + ReferencePadding;
  });
  return {width, previousSize.height()};
}

QSize paintGraph(QPainter& painter, const QStyleOptionViewItem& option, const QModelIndex& index) noexcept {
  painter.save();

  auto pen = painter.pen();
  pen.setWidthF(EdgeWidth);
  painter.setPen(pen);

  const auto graphRow = graph(index);
  const auto [commitIndex, paths] = graphRow;
  const QRectF rect = option.rect;
  const QPointF origin = rect.topLeft();
  const qreal commitNodeY0 = origin.y() + BranchSpacing;
  const qreal commitNodeY1 = commitNodeY0 + CommitNodeHeight;
  const qreal centerY = rect.center().y();

  QVector<QPointF> lines;
  if (index.row() > 0) {
    std::vector<int> sources;
    for (auto [source, _] : paths) {
      if (!stx::contains(sources, source)) {
        sources.push_back(source);
      }
    }
    if (!stx::contains(sources, commitIndex)) {
      sources.push_back(commitIndex);
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
      painter.drawPath(path);
    } else {
      QPainterPath path{sourcePoint};
      QPointF control1{x0 - BranchSpacing, y1};
      QPointF control2{x1 + BranchSpacing, y0};
      path.cubicTo(control1, control2, destinationPoint);
      painter.drawPath(path);
    }
  }

  painter.drawLines(lines);

  const qreal x = origin.x() + commitIndex * CommitNodeWidth + (commitIndex + 1) * BranchSpacing;
  QPen nodePen = painter.pen();
  nodePen.setBrush(option.palette.foreground());
  nodePen.setWidthF(NodeWidth);
  painter.setPen(nodePen);
  painter.drawEllipse(x, commitNodeY0, CommitNodeWidth, CommitNodeHeight);

  painter.restore();
  return graphSizeHint(option.decorationSize, graphRow);
}

void paintReference(QPainter& painter, QRectF rect, const Reference& ref, const QBrush& background) noexcept {
  QRectF boundingRect = painter.fontMetrics().boundingRect(rect.toRect(), Qt::AlignCenter, shortName(ref));
  QPainterPath path;
  path.addRoundedRect(boundingRect.adjusted(-ReferenceMargin, -ReferenceMargin, ReferenceMargin, ReferenceMargin),
                      ReferenceMargin, ReferenceMargin);
  painter.fillPath(path, background);
  painter.drawPath(path);
  painter.drawText(boundingRect, Qt::AlignCenter, shortName(ref));
}

} // namespace
