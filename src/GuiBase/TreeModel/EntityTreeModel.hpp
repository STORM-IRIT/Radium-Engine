#ifndef RADIUMENGINE_ENTITYTREEMODEL_HPP
#define RADIUMENGINE_ENTITYTREEMODEL_HPP

#include <vector>
#include <memory>

#include <QAbstractItemModel>
#include <QVariant>

#include <Engine/ItemModel/ItemEntry.hpp>


namespace Ra
{
    namespace GuiBase
    {
        /// Element of the tree representation of items.
        struct TreeItem
        {
            Engine::ItemEntry m_entry;
            TreeItem* m_parent;
            std::vector<std::unique_ptr<TreeItem>> m_children;

            // Return the index of this object in the parents' child list.
            // Of course this won't work for the root item
            int getIndexInParent() const;
        };

        /// Implementation of QAbstractItemModel to show the engine objects
        /// as a tree in the main GUI.
        /// See http://doc.qt.io/qt-5/qtwidgets-itemviews-simpletreemodel-example.html
        ///  The root elelement conventionally has no data but is the parent of all entity
        /// items which parent their components ,etc.
        class ItemModel : public QAbstractItemModel
        {
            Q_OBJECT
        public:

            ItemModel ( const Engine::RadiumEngine* engine, QObject* parent = nullptr)
                    : QAbstractItemModel( parent )
                    , m_engine( engine )
            {
                buildModel();
            }

            // QAbstractItemModel interface

            /// Return the data corresponding to the given index : in this case a string with the item name.
            QVariant data( const QModelIndex& index, int role ) const override;

            /// Return the header data of the given section.
            QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

            /// Returns the number of rows, i.e .the number of children of a given parent item.
            int rowCount( const QModelIndex& parent = QModelIndex() ) const override;

            /// Returns the number of columns.
            int columnCount( const QModelIndex& parent = QModelIndex()) const override { return 1;}

            /// Create an index for the given row, column and parent.
            QModelIndex index ( int row, int column, const QModelIndex& parent = QModelIndex()) const override;

            /// Get the parent of a given child item.
            QModelIndex parent( const QModelIndex& child ) const override;

            /// Get the flags of the object.
            Qt::ItemFlags flags( const QModelIndex &index ) const override;


            // Other functions

            /// Returns the entry corresponding to a given index or an invalid entry
            /// if the index doesn't match any entry.
            const Engine::ItemEntry& getEntry( const QModelIndex& index )const;

            /// Returns the index corresponding to the given entry if it exists in the
            /// model, or an invalid index if not found.
            QModelIndex findEntryIndex( const Engine::ItemEntry& entry ) const;

        public slots:

            /// Call this function to update the tree model to the
            /// current content of the engine.
            void rebuildModel();

            void addItem( const Engine::ItemEntry& ent );

            void removeItem( const Engine::ItemEntry& ent );

       signals:
            void modelRebuilt();

        protected:
            /// Internal function to build the tree.
            void buildModel();

            /// Internal functions to check if an item is in the tree.
            bool findInTree( const TreeItem* item ) const;

            /// Get the tree item corresponding to the given index.
            TreeItem* getItem( const QModelIndex& index ) const;

            /// Prints elements in the model (debug only)
            void printModel() const;

        protected:
            /// Engine instance.
            const Engine::RadiumEngine* m_engine;

            /// Root of the tree.
            std::unique_ptr<TreeItem> m_rootItem;
        };


    } // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_ENTITYTREEMODEL_HPP
