#ifndef RADIUMENGINE_TREEMODEL_HPP
#define RADIUMENGINE_TREEMODEL_HPP

#include <GuiBase/RaGuiBase.hpp>

#include <memory>
#include <vector>

#include <QAbstractItemModel>
#include <QVariant>

/**
 * This file contains a basic implementation of a tree model object with QAbstractItemModel.
 * \see http://doc.qt.io/qt-5/qtwidgets-itemviews-simpletreemodel-example.html
 *
 * This could have been a template with the object type, but QObjects cannot be
 * templated, which is very sad.
 * To use this base class you must :
 * - create a derived class from TreeItem which links the model with actual objects
 * for example by storing a handle or a pointer.
 * - create a derived class from TreeModel. Add items of the derived TreeItem class
 * to the tree, and static_cast the result of getItem to the concrete class to use it.
 */

namespace Ra {

namespace GuiBase {
/**
 * Base class for elements of the tree representation.
 * It just maintains a tree structure leaving the data storage to derived classes.
 */
class RA_GUIBASE_API TreeItem {
  public:
    TreeItem() {}

    virtual ~TreeItem() {}

    /**
     * Copy operator is forbidden.
     */
    TreeItem( const TreeItem& ) = delete;

    /**
     * Assignment operator is forbidden.
     */
    TreeItem& operator=( const TreeItem& ) = delete;

    /**
     * Return the name of the represented item to be displayed in the Qt tree.
     */
    virtual std::string getName() const = 0;

    /**
     * Return true if the represented object is valid, false otherwise.
     */
    virtual bool isValid() const = 0;

    /**
     * Return true if the represented object can be selected, false otherwise.
     */
    virtual bool isSelectable() const = 0;

    /**
     * Return the index of this object in the parents' child list.
     * \note This won't work for the root item.
     */
    int getIndexInParent() const;

  public:
    /// Parent object of item.
    TreeItem* m_parent;

    /// Children of item in the tree.
    std::vector<std::unique_ptr<TreeItem>> m_children;
};

/**
 * This class implement QAbstractItem model with the TreeItem as its model.
 * Derived class must use some kind of cast to access the derived TreeItems.
 */
class RA_GUIBASE_API TreeModel : public QAbstractItemModel {
    Q_OBJECT

  public:
    TreeModel( QObject* parent = nullptr ) : QAbstractItemModel( parent ) {}

    /// \name QAbstractItemModel interface
    /// \{

    int rowCount( const QModelIndex& parent = QModelIndex() ) const override;

    int columnCount( const QModelIndex& parent = QModelIndex() ) const override { return 1; }

    QVariant data( const QModelIndex& index, int role ) const override;

    QVariant headerData( int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole ) const override;

    QModelIndex index( int row, int column,
                       const QModelIndex& parent = QModelIndex() ) const override;

    QModelIndex parent( const QModelIndex& child ) const override;

    Qt::ItemFlags flags( const QModelIndex& index ) const override;
    /// \}

    /**
     * Update the tree model to match the current content of the Engine.
     */
    void rebuildModel();

  signals:
    /**
     * Emitted when the model has been rebuilt.
     */
    void modelRebuilt();

  protected:
    /**
     * Return the Header string.
     */
    virtual std::string getHeaderString() const = 0;

    /**
     * Build the model from scratch.
     * Called either at initialization or through rebuildModel().
     */
    virtual void buildModel() = 0;

    /**
     * Return the tree item corresponding to the given index.
     */
    TreeItem* getItem( const QModelIndex& index ) const;

    /**
     * Check if the given TreeItem is in the tree.
     */
    bool findInTree( const TreeItem* item ) const;

    /**
     * Prints elements in the model to stdout (in debug mode only).
     */
    void printModel() const;

  protected:
    /// Root of the tree.
    std::unique_ptr<TreeItem> m_rootItem;
};
} // namespace GuiBase
} // namespace Ra
#endif // RADIUMENGINE_TREEMODEL_HPP_
