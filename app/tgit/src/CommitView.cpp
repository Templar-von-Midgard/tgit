#include "CommitView.hpp"

#include <utility>

#include <git2/buffer.h>
#include <git2/commit.h>
#include <git2/diff.h>
#include <git2/tree.h>

#include <gitpp/Commit.hpp>

#include "DiffView.hpp"
#include "GitUtils.hpp"

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

DiffView CommitView::diff() const noexcept {
  return DiffView{nullptr, nullptr};
}
