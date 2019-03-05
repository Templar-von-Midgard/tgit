#include "ReferencesModel.hpp"

#include <gitpp/Reference.hpp>

#include "ReferenceDatabase.hpp"

namespace {
QString toString(Reference::T kind) noexcept {
  switch (kind) {
  case Reference::LocalBranch:
    return QStringLiteral("Local branches");
  case Reference::RemoteBranch:
    return QStringLiteral("Remote branches");
  case Reference::Tag:
    return QStringLiteral("Tags");
  case Reference::Note:
    return QStringLiteral("Notes");
  default:
    return "UNKNOWN";
  }
}
} // namespace

struct ReferencesModel::TreeItem {
  TreeItem(const QString& text, TreeItem* parent = nullptr) noexcept : Text(text), Parent(parent) {
  }

  TreeItem& addChild(const QString& text) noexcept {
    return *Children.emplace_back(std::make_unique<TreeItem>(text, this));
  }

  QString Text;
  TreeItem* Parent;
  std::vector<std::unique_ptr<TreeItem>> Children;
};

ReferencesModel::ReferencesModel(QObject* parent) : QAbstractItemModel(parent) {
  Root = std::make_unique<TreeItem>("");
  auto& localRoot = Root->addChild(toString(Reference::LocalBranch));
  auto& remoteRoot = Root->addChild(toString(Reference::RemoteBranch));
  auto& tagRoot = Root->addChild(toString(Reference::Tag));
  auto& noteRoot = Root->addChild(toString(Reference::Note));
}

ReferencesModel::~ReferencesModel() = default;

void ReferencesModel::load(const std::vector<Reference>& references) {
  beginResetModel();
  Root = std::make_unique<TreeItem>("");
  auto& localRoot = Root->addChild(toString(Reference::LocalBranch));
  auto& remoteRoot = Root->addChild(toString(Reference::RemoteBranch));
  auto& tagRoot = Root->addChild(toString(Reference::Tag));
  auto& noteRoot = Root->addChild(toString(Reference::Note));

  for (const auto& ref : references) {
    switch (ref.Type) {
    case Reference::LocalBranch:
      localRoot.addChild(ref.ShortName);
      break;
    case Reference::RemoteBranch:
      remoteRoot.addChild(ref.ShortName);
      break;
    case Reference::Tag:
      tagRoot.addChild(ref.ShortName);
      break;
    case Reference::Note:
      noteRoot.addChild(ref.ShortName);
      break;
    default:
      break;
    }
  }
  endResetModel();
}

QModelIndex ReferencesModel::index(int row, int column, const QModelIndex& parent) const {
  if (!hasIndex(row, column, parent)) {
    return {};
  }
  auto parentItem = parent.isValid() ? itemAt(parent) : Root.get();
  return createIndex(row, column, parentItem->Children[row].get());
}

QModelIndex ReferencesModel::parent(const QModelIndex& child) const {
  if (!child.isValid()) {
    return {};
  }
  auto parentItem = itemAt(child)->Parent;
  if (parentItem == Root.get()) {
    return {};
  }
  if (parentItem->Parent == nullptr) {
    return createIndex(0, 0, parentItem);
  }
  for (std::size_t i = 0, size = parentItem->Parent->Children.size(); i < size; ++i) {
    if (parentItem == parentItem->Parent->Children[i].get()) {
      return createIndex(i, 0, parentItem);
    }
  }
  std::abort();
}

int ReferencesModel::rowCount(const QModelIndex& parent) const {
  auto parentItem = parent.isValid() ? itemAt(parent) : Root.get();
  return parentItem->Children.size();
}

int ReferencesModel::columnCount(const QModelIndex& parent) const {
  return 1;
}

QVariant ReferencesModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::DisplayRole) {
    return itemAt(index)->Text;
  }
  return {};
}

ReferencesModel::TreeItem* ReferencesModel::itemAt(const QModelIndex& index) const {
  return static_cast<TreeItem*>(index.internalPointer());
}
