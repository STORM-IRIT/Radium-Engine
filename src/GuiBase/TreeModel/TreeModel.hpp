#ifndef RADIUMENGINE_TREEMODEL_HPP
#define RADIUMENGINE_TREEMODEL_HPP

#include <GuiBase/RaGuiBase.hpp>

#include <vector>
#include <memory>

#include <QAbstractItemModel>
#include <QVariant>



/// This file contains a basic implementation of a tree model object with QAbstractItemModel
/// See http://doc.qt.io/qt-5/qtwidgets-itemviews-simpletreemodel-example.html
/// This could have been a template with the object type, but QObjects cannot be
/// templated, which is very sad.
/// To use this base class you must :
/// * create a derived class from TreeItem which links the model with actual objects
/// for example by storing a handle or a pointer.
/// * create a derived class from TreeModel. Add items of the derived TreeItem class
/// to the tree, and static_cast the result of getItem to the concrete class to use it.

namespace Ra
{

    namespace GuiBase
    {
        /// Base classes for element of the tree representation.
        /// It just maintains a tree structure leaving the data
        /// storage to derived classes.
        class RA_GUIBASE_API TreeItem
        {
        public:
            TreeItem() {}
            virtual ~TreeItem() {}

            // TreeItems are non-copyable
            TreeItem( const TreeItem& ) = delete;
            TreeItem& operator= ( const TreeItem& ) = delete;

            // Interface

            /// Returns the name of the represented item to be
            /// displayed in the Qt tree.
            virtual std::string getName() const = 0;

            /// Return true if  the represented object is valid.
            virtual bool isValid() const = 0;

            /// Return true if the represented object can be selected.
            virtual bool isSelectable() const = 0;

            // Other functions

            // Return the index of this object in the parents' child list.
            // Of course this won't work for the root item.
            int getIndexInParent() const;

        public:
            // Tree structure variables

            /// Parent object of item.
            TreeItem* m_parent;

            /// Children of item in the tree.
            std::vector <std::unique_ptr<TreeItem> > m_children;
        };


        /// This class implement QAbstractItem model with the TreeItem as its model
        /// Derived class must use some kind of cast to access the derived TreeItems.
        class RA_GUIBASE_API TreeModel : public QAbstractItemModel
        {
            Q_OBJECT

        public:
            TreeModel(QObject* parent = nullptr)
                    : QAbstractItemModel(parent) { }

            // QAbstractItemModel interface

            /// Returns the number of rows, i.e .the number of children of a given parent item.
            int rowCount(const QModelIndex& parent = QModelIndex()) const override;

            /// Returns the number of columns (only one in our case).
            int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 1; }

            /// Return the data corresponding to the given index : in this case a string with the item name.
            QVariant data(const QModelIndex& index, int role) const override;

            /// Return the header data of the given section.
            QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

            /// Create an index for the given row, column and parent.
            QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

            /// Get the parent of a given child item.
            QModelIndex parent(const QModelIndex& child) const override;

            /// Get the flags of the object.
            Qt::ItemFlags flags(const QModelIndex& index) const override;

            /// Call this function to update the tree model to the
            /// current content of the engine.
            void rebuildModel();

            signals:
            /// Emitted when the model has been rebuilt.
            void modelRebuilt();

        protected:

            virtual std::string getHeaderString() const = 0;

            /// Function to build the model from scratch. Call either at initialization or through rebuildModel().
            virtual void buildModel() = 0;

            /// Get the tree item corresponding to the given index.
            TreeItem* getItem(const QModelIndex& index) const;

            /// Internal functions to check if an item is in the tree.
            bool findInTree(const TreeItem* item) const;

            /// Prints elements in the model to stdout (debug only)
            void printModel() const;

        protected:
            /// Root of the tree.
            std::unique_ptr <TreeItem> m_rootItem;
        };
    }
}
#endif // RADIUMENGINE_TREEMODEL_HPP_
