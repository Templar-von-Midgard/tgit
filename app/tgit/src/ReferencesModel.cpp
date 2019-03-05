#include "ReferencesModel.hpp"

#include <functional>
#include <variant>

#include <gitpp/Reference.hpp>

#include <stx/overload.hpp>

#include "ReferenceDatabase.hpp"

struct ReferencesModel::TreeItem {
  using ElementT = std::variant<std::monostate, QString, std::reference_wrapper<const Reference>>;

  TreeItem(ElementT data = {}, TreeItem* parent = nullptr) noexcept : Data(data), Parent(parent) {
  }

  TreeItem& addChild(ElementT data) noexcept {
    return *Children.emplace_back(std::make_unique<TreeItem>(data, this));
  }

  ElementT Data;
  TreeItem* Parent;
  std::vector<std::unique_ptr<TreeItem>> Children;
};

ReferencesModel::ReferencesModel(QObject* parent) : QAbstractItemModel(parent) {
  Root = std::make_unique<TreeItem>("");
  auto& localRoot = Root->addChild(tr("Local branches"));
  auto& remoteRoot = Root->addChild(tr("Remote branches"));
  auto& tagRoot = Root->addChild(tr("Tags"));
  auto& noteRoot = Root->addChild(tr("Notes"));
}

ReferencesModel::~ReferencesModel() = default;

void ReferencesModel::load(const std::vector<Reference>& references) {
  beginResetModel();
  Root = std::make_unique<TreeItem>("");
  auto& localRoot = Root->addChild(tr("Local branches"));
  auto& remoteRoot = Root->addChild(tr("Remote branches"));
  auto& tagRoot = Root->addChild(tr("Tags"));
  auto& noteRoot = Root->addChild(tr("Notes"));

  for (const auto& ref : references) {
    auto& root = std::visit(stx::overload{[&localRoot](const LocalBranch&) -> TreeItem& { return localRoot; },
                                          [&remoteRoot](const RemoteBranch&) -> TreeItem& { return remoteRoot; },
                                          [&tagRoot](const Tag&) -> TreeItem& { return tagRoot; },
                                          [&noteRoot](const Note&) -> TreeItem& { return noteRoot; }},
                            ref);
    root.addChild(std::ref(ref));
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
    return std::visit(stx::overload{[](std::monostate) { return QString{}; }, [](QString text) { return text; },
                                    [](const Reference& ref) { return shortName(ref); }},
                      itemAt(index)->Data);
  }
  if (role == DataRole) {
    if (auto refp = std::get_if<2>(&itemAt(index)->Data); refp != nullptr) {
      return QVariant::fromValue(refp->get());
    }
  }
  return {};
}

ReferencesModel::TreeItem* ReferencesModel::itemAt(const QModelIndex& index) const {
  return static_cast<TreeItem*>(index.internalPointer());
}
