#ifndef RADIUMENGINE_SELECTION_MANAGER_HPP_
#define RADIUMENGINE_SELECTION_MANAGER_HPP_

#include <QItemSelectionModel>

#include <Engine/ItemModel/ItemEntry.hpp>
#include <GuiBase/TreeModel/EntityTreeModel.hpp>

namespace Ra
{
    namespace Engine
    {
        class Entity;
        class Component;
    }
}

namespace Ra
{
    namespace GuiBase
    {

        class SelectionManager : public QItemSelectionModel
        {
        Q_OBJECT
        public:
            SelectionManager(ItemModel* model, QObject* parent = nullptr);

            // The following functions adapt QItemSelectionModel functions to
            // operate with ItemEntries instead of model indices.

            /// Returns true if the selection contains the given item.
            bool isSelected(const Engine::ItemEntry& ent) const;

            /// Return the set of selected entries. @see selectedIndexes()
            std::vector<Engine::ItemEntry> selectedEntries() const;

            /// Return the current selected item, or an invalid entry if there is no
            /// current item. @seeCurrentIndex;
            const Engine::ItemEntry& currentItem() const;

            /// Select an item through an item entry. @see QItemSelectionModel::Select
            void select( const Engine::ItemEntry& ent,  QItemSelectionModel::SelectionFlags command );

            /// Select an item through an item entry. @see QItemSelectionModel::Select
            void select( const QModelIndex& idx, QItemSelectionModel::SelectionFlags command ) override
            {
                Engine::ItemEntry ent = itemModel()->getEntry(idx);
                QItemSelectionModel::select( idx, command );
            }

            void select( const QItemSelection& selection, QItemSelectionModel::SelectionFlags command) override
            {
                std::vector<Engine::ItemEntry> entries;
                for ( const auto& idx : selection.indexes())
                {
                    entries.push_back(itemModel()->getEntry(idx));
                }

                QItemSelectionModel::select(selection , command );
            }

            /// Set an item as current through an item entry. @see QItemSelectionModen::setCurrent
            void setCurrentEntry( const Engine::ItemEntry& ent, QItemSelectionModel::SelectionFlags command);

        protected slots:
            void onModelRebuilt();
        protected slots:
            void printSelection() const;

        protected:

            const ItemModel* itemModel () const  { return static_cast<const ItemModel*>( model());}

        };

    }
}


#endif //  RADIUMENGINE_SELECTION_MANAGER_HPP_
