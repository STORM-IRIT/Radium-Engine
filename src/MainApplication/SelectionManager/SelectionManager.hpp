#ifndef RADIUMENGINE_SELECTION_MANAGER_HPP_
#define RADIUMENGINE_SELECTION_MANAGER_HPP_

#include <QItemSelectionModel>

#include <MainApplication/ItemModel/ItemEntry.hpp>
#include <MainApplication/Gui/EntityTreeModel.hpp>


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
    namespace Gui
    {

        class SelectionManager : public QItemSelectionModel
        {
        Q_OBJECT
        public:
            SelectionManager(ItemModel* model, QObject* parent = nullptr);

            // The following functions adapt QItemSelectionModel functions to
            // operate with ItemEntries instead of model indices.

            /// Returns true if the selection contains the given item.
            bool isSelected(const ItemEntry& ent) const;

            /// Return the set of selected entries. @see selectedIndexes()
            std::vector<ItemEntry> selectedEntries() const;

            /// Return the current selected item, or an invalid entry if there is no
            /// current item. @seeCurrentIndex;
            ItemEntry currentItem() const;

            void select( const ItemEntry& ent,  QItemSelectionModel::SelectionFlags command );

            void setCurrentEntry( const ItemEntry& ent, QItemSelectionModel::SelectionFlags command);


        protected slots:
            void printSelection() const;

        protected:

            const ItemModel* itemModel () const  { return static_cast<const ItemModel*>( model());}

        };

    }
}


#endif //  RADIUMENGINE_SELECTION_MANAGER_HPP_
