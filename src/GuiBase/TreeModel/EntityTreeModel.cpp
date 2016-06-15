#include <GuiBase/TreeModel/EntityTreeModel.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Component.hpp>
#include <stack>

using Ra::Engine::ItemEntry;

namespace Ra
{
    namespace GuiBase
    {

        int ItemModel::rowCount(const QModelIndex& parent) const
        {
            if (parent.column() > 0)
            {
                return 0;
            }
            TreeItem* parentItem = getItem(parent);
            return int(parentItem->m_children.size());
        }

        TreeItem* ItemModel::getItem(const QModelIndex& index) const
        {
            if (index.isValid())
            {
                TreeItem* item = static_cast<TreeItem*> (index.internalPointer());
                CORE_ASSERT(item, "Null item found");
                // This can indicate that you have invalid pointers in your model index.
                // e.g. after rebuilding the model.
                CORE_ASSERT(findInTree(item), "Item is not in tree");
                return item;
            }
            return m_rootItem.get();
        }

        void ItemModel::buildModel()
        {
            m_rootItem.reset(new TreeItem);
            m_rootItem->m_parent = nullptr;
            for (const auto& ent : m_engine->getEntityManager()->getEntities())
            {
                TreeItem* entityItem = new TreeItem;
                entityItem->m_entry = ItemEntry(ent);
                entityItem->m_parent = m_rootItem.get();
                for (const auto& comp : ent->getComponents())
                {
                    if (comp)
                    {
                        TreeItem* componentItem = new TreeItem;
                        componentItem->m_entry = ItemEntry(ent, comp.get());
                        componentItem->m_parent = entityItem;
                        for (const auto& roIdx : comp->m_renderObjects)
                        {
                            TreeItem* roItem = new TreeItem;
                            roItem->m_entry = ItemEntry(ent, comp.get(), roIdx);
                            roItem->m_parent = componentItem;
                            componentItem->m_children.emplace_back(roItem);
                        }
                        entityItem->m_children.emplace_back(componentItem);
                    }
                }
                m_rootItem->m_children.emplace_back(entityItem);
            }
        }

        QVariant ItemModel::data(const QModelIndex& index, int role) const
        {
            if (index.isValid() && role == Qt::DisplayRole)
            {
                if (index.column() == 0)
                {
                    const ItemEntry& entry = getItem(index)->m_entry;
                    return QVariant(QString::fromStdString(getEntryName(m_engine, entry)));
                }
            }
            return QVariant();
        }

        QVariant ItemModel::headerData(int section, Qt::Orientation orientation, int role) const
        {
            if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
            {
                return QVariant("Engine objects tree");
            }
            else
            {
                return QVariant();
            }
        }

        QModelIndex ItemModel::index(int row, int column, const QModelIndex& parent) const
        {
            if (!hasIndex(row, column, parent))
            {
                return QModelIndex();
            }
            // Only items from the first column may have a parent
            if (parent.isValid() && parent.column() != 0)
            {
                return QModelIndex();
            }
            // Grab the parent and make an index of the child.
            TreeItem* parentItem = getItem(parent);
            if (parentItem && row < parentItem->m_children.size())
            {
                return createIndex(row, column, parentItem->m_children[row].get());
            }
            else
            {
                return QModelIndex();
            }

        }

        QModelIndex ItemModel::parent(const QModelIndex& child) const
        {
            if (child.isValid())
            {
                TreeItem* childItem = getItem(child);
                TreeItem* parentItem = childItem->m_parent;

                // No parents of the root item are indexed.
                if (parentItem && parentItem != m_rootItem.get())
                {
                    // Figure out which row the parent is in.
                    return createIndex(parentItem->getIndexInParent(), 0, parentItem);
                }
            }
            return QModelIndex();
        }

        Qt::ItemFlags ItemModel::flags(const QModelIndex& index) const
        {
            return index.isValid() && getItem(index)->m_entry.isValid() && getItem(index)->m_entry.isSelectable()
                   ? QAbstractItemModel::flags(index) : Qt::ItemFlags(0);
        }

        const ItemEntry& ItemModel::getEntry(const QModelIndex& index) const
        {
            return getItem(index)->m_entry;
        }

        void ItemModel::rebuildModel()
        {
            // Need to call these function to invalidate the pointers
            // in the QModelIndex.
            beginResetModel();
            buildModel();
            endResetModel();
            emit modelRebuilt();
        }

        QModelIndex ItemModel::findEntryIndex(const ItemEntry& entry) const
        {
            if (entry.isValid())
            {
                std::stack<TreeItem*> stack;
                stack.push(m_rootItem.get());
                while (!stack.empty())
                {
                    TreeItem* item = stack.top();
                    stack.pop();
                    // Found item, so build the index corresponding to it.
                    if (item->m_entry == entry)
                    {
                        return createIndex(item->getIndexInParent(), 0, item);
                    }

                    // Add children to the stack
                    for (const auto& child : item->m_children)
                    {
                        stack.push(child.get());
                    }
                }
            }

            return QModelIndex();
        }

        bool ItemModel::findInTree(const TreeItem* item) const
        {
            std::stack<const TreeItem*> stack;
            stack.push(m_rootItem.get());
            while (!stack.empty())
            {
                const TreeItem* current = stack.top();
                stack.pop();
                if (current == item)
                {
                    return true;
                }
                for (const auto& child : current->m_children)
                {
                    stack.push(child.get());
                }
            }
            return false;
        }

        void ItemModel::addItem(const Engine::ItemEntry& ent)
        {
            CORE_ASSERT(ent.isValid(), "Inserting invalid entry");
            CORE_ASSERT(!findEntryIndex(ent).isValid(), "Entry already in model");
            if (!findEntryIndex(ent).isValid())
            {
                TreeItem* parentItem;
                QModelIndex parentIdx;
                if (ent.isEntityNode())
                {
                    parentItem = m_rootItem.get();
                }
                else
                {
                    ItemEntry parentEntry;
                    if (ent.isRoNode())
                    {
                        parentEntry = ItemEntry(ent.m_entity, ent.m_component);
                    }
                    else if (ent.isComponentNode())
                    {
                        parentEntry = ItemEntry(ent.m_entity);
                    }
                    parentIdx = findEntryIndex(parentEntry);
                    CORE_ASSERT(parentIdx.isValid(), "Parent does not exist");
                    parentItem = getItem( parentIdx );
                }

                CORE_ASSERT( getItem(parentIdx) == parentItem, "Model inconsistency");

                TreeItem* childItem = new TreeItem;
                childItem->m_entry = ent;
                childItem->m_parent = parentItem;
                int row = int(parentItem->m_children.size());
                beginInsertRows(parentIdx, row, row);
                parentItem->m_children.emplace_back(childItem);
                endInsertRows();
            }
        }

        void ItemModel::removeItem(const Engine::ItemEntry& ent)
        {
            QModelIndex entryIndex = findEntryIndex(ent);
            CORE_ASSERT(ent.isValid(), "Removing invalid entry");
            CORE_ASSERT( entryIndex.isValid(), "Entry not in model");
            if( entryIndex.isValid())
            {
                TreeItem* toRemove = getItem( entryIndex );
                TreeItem* parentItem = toRemove->m_parent;
                auto& childList = parentItem->m_children;
                const auto  childPos = std::find_if(childList.begin(),
                                                 childList.end(),
                                                 [toRemove](const auto& ptr) { return ptr.get() == toRemove;});

                CORE_ASSERT( childPos != childList.end(), "Child not in parent's list");
                int row = toRemove->getIndexInParent();
                CORE_ASSERT( childPos - childList.begin() == row, "Iterator consistency error");
                beginRemoveRows( parent(entryIndex), row, row );
                parentItem->m_children.erase(childPos);
                endRemoveRows();
            }
        }

        void ItemModel::printModel() const
        {
#if defined CORE_DEBUG
            LOG(logDEBUG)<< " Printing tree model";
            std::stack<const TreeItem*> stack;
            stack.push(m_rootItem.get());
            while (!stack.empty())
            {
                const TreeItem* current = stack.top();
                stack.pop();
                LOG(logDEBUG)<<Engine::getEntryName(m_engine, current->m_entry);
                for (const auto& child : current->m_children)
                {
                    stack.push(child.get());
                }
            }
#endif
        }


        int TreeItem::getIndexInParent() const
        {
            CORE_ASSERT(m_parent, "Looking for the root item's index.");
            for (uint i = 0; i < m_parent->m_children.size(); ++i)
            {
                if (m_parent->m_children[i].get() == this)
                {
                    return i;
                }
            }
            // If we reached here, it means that the item
            // was not found in its parent's child list.
            // indicating the tree is corrupted.
            CORE_ASSERT(false, " Did not find child in parent");
            return 0;
        }


    } // namespace Gui
} // namespace Ra
