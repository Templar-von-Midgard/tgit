#include "ReferencesModel.hpp"

#include <gitpp/Reference.hpp>

namespace {
QString toString(ReferenceKind kind) noexcept {
  switch (kind) {
  case ReferenceKind::LocalBranch:
    return QStringLiteral("Local branches");
  case ReferenceKind::RemoteBranch:
    return QStringLiteral("Remote branches");
  case ReferenceKind::Tag:
    return QStringLiteral("Tags");
  case ReferenceKind::Note:
    return QStringLiteral("Notes");
  default:
    return "UNKNOWN";
  }
}
} // namespace

struct ReferencesModel::TreeItem {
  QString Text;
  TreeItem* Parent = nullptr;
  std::vector<std::unique_ptr<TreeItem>> Children;
};

ReferencesModel::ReferencesModel(QObject* parent) : QAbstractItemModel(parent) {
  Root = std::make_unique<TreeItem>();
  for (auto kind : {ReferenceKind::LocalBranch, ReferenceKind::RemoteBranch, ReferenceKind::Tag, ReferenceKind::Note}) {
    auto& group = Root->Children.emplace_back(std::make_unique<TreeItem>());
    group->Parent = Root.get();
    group->Text = toString(kind);
  }
}

ReferencesModel::~ReferencesModel() = default;

void ReferencesModel::loadRepository(const gitpp::Repository& repo) {
  beginResetModel();
  References.clear();
  for (const auto& ref : gitpp::ReferenceCollection(repo)) {
    if (ref.type() == gitpp::Reference::Symbolic) {
      continue;
    }
    auto shortname = ref.shortname();
    auto type = [](const gitpp::Reference& ref) -> ReferenceKind {
      if (ref.isLocalBranch()) {
        return ReferenceKind::LocalBranch;
      } else if (ref.isRemoteBranch()) {
        return ReferenceKind::RemoteBranch;
      } else if (ref.isTag()) {
        return ReferenceKind::Tag;
      } else if (ref.isNote()) {
        return ReferenceKind::Note;
      }
      std::abort();
    }(ref);
    References.emplace(type, QString::fromUtf8(shortname.data(), shortname.size()));
  }

  Root = std::make_unique<TreeItem>();
  for (auto kind : {ReferenceKind::LocalBranch, ReferenceKind::RemoteBranch, ReferenceKind::Tag, ReferenceKind::Note}) {
    auto& group = Root->Children.emplace_back(std::make_unique<TreeItem>());
    group->Parent = Root.get();
    group->Text = toString(kind);
    auto [begin, end] = References.equal_range(kind);
    for (; begin != end; ++begin) {
      auto& item = group->Children.emplace_back(std::make_unique<TreeItem>());
      item->Parent = group.get();
      item->Text = begin->second;
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
