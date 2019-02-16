#include "CommitView.hpp"

#include <utility>

#include <gitpp/Commit.hpp>

QString CommitView::shortId() const noexcept {
  return QString::fromStdString(commit.shortId());
}

QString CommitView::summary() const noexcept {
  return QString::fromStdString(commit.summary());
}

QString CommitView::message() const noexcept {
  return QString::fromStdString(commit.message());
}

QDateTime CommitView::creation() const noexcept {
  return QDateTime::fromSecsSinceEpoch(commit.author().When.Time);
}

QString CommitView::author() const noexcept {
  return QString::fromStdString(commit.author().Name);
}
