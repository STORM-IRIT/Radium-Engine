#include <GuiBase/TreeModel/TreeModel.hpp>

#include <stack>
#include <Core/Log/Log.hpp>

namespace Ra
{
    namespace GuiBase
    {

         int TreeModel::rowCount(const QModelIndex& parent) const
        {
            /// The row count of an item is the number of child it has.
            if (parent.column() > 0)
            {
                return 0;
            }
            TreeItem* parentItem = getItem(parent);
            return int(parentItem->m_children.size());
        }

        TreeItem* TreeModel::getItem(const QModelIndex& index) const
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

        QVariant TreeModel::data(const QModelIndex& index, int role) const
        {
            if (index.isValid() && role == Qt::DisplayRole)
            {
                if (index.column() == 0)
                {
                    return QVariant(QString::fromStdString(getItem(index)->getName()));
                }
            }
            return QVariant();
        }

        QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
        {
            if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
            {
                return QVariant(QString::fromStdString(getHeaderString()));
            }
            else
            {
                return QVariant();
            }
        }

        QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const
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

        QModelIndex TreeModel::parent(const QModelIndex& child) const
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

        Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const
        {
            return index.isValid() && getItem(index)->isValid() && getItem(index)->isSelectable()
                   ? QAbstractItemModel::flags(index) : Qt::ItemFlags(0);
        }

        void TreeModel::rebuildModel()
        {
            // Need to call these function to invalidate the pointers
            // in the QModelIndex.
            beginResetModel();
            buildModel();
            endResetModel();
            emit modelRebuilt();
        }

        bool TreeModel::findInTree(const TreeItem* item) const
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

        void TreeModel::printModel() const
        {
#if defined CORE_DEBUG
            LOG(logDEBUG) << " Printing tree model";
            std::stack<const TreeItem*> stack;
            stack.push(m_rootItem.get());
            while (!stack.empty())
            {
                const TreeItem* current = stack.top();
                stack.pop();
                LOG(logDEBUG) << current->getName();
                for (const auto& child : current->m_children)
                {
                    stack.push(child.get());
                }
            }
#endif
        }
    }
}
