#ifndef RADIUMENGINE_SELECTION_MANAGER_HPP_
#define RADIUMENGINE_SELECTION_MANAGER_HPP_
#include <GuiBase/RaGuiBase.hpp>

#include <Engine/ItemModel/ItemEntry.hpp>
#include <GuiBase/TreeModel/EntityTreeModel.hpp>
#include <QItemSelectionModel>

namespace Ra {
namespace Engine {
class Entity;
class Component;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace GuiBase {

/**
 * The SelectionManager class manages objet selection in Radium.
 * It adapts QItemSelectionModel to operate with ItemEntries instead of
 * model indices.
 */
class RA_GUIBASE_API SelectionManager : public QItemSelectionModel {
    Q_OBJECT
  public:
    SelectionManager( ItemModel* model, QObject* parent = nullptr );

    /**
     * Returns true if the selection contains the given item.
     */
    bool isSelected( const Engine::ItemEntry& ent ) const;

    /**
     * Return the set of selected entries.
     * \see QItemSelectionModel::selectedIndexes().
     */
    std::vector<Engine::ItemEntry> selectedEntries() const;

    /**
     * Return the current selected item, or an invalid entry if there is no
     * current item.
     * \see QItemSelectionModel::CurrentIndex().
     */
    const Engine::ItemEntry& currentItem() const;

    /**
     * Select an item through an item entry.
     * \see QItemSelectionModel::Select().
     */
    virtual void select( const Engine::ItemEntry& ent,
                         QItemSelectionModel::SelectionFlags command );

    /**
     * Set an item as current through an item entry.
     * \see QItemSelectionModel::setCurrent().
     */
    void setCurrentEntry( const Engine::ItemEntry& ent,
                          QItemSelectionModel::SelectionFlags command );

  protected slots:
    /**
     * Clear the selection when the model is rebuilt.
     */
    void onModelRebuilt();

    /**
     * Display stat info to the Debug output.
     */
    void printSelection() const;

  protected:
    /**
     * Return the ItemModel.
     */
    const ItemModel* itemModel() const { return static_cast<const ItemModel*>( model() ); }
};

} // namespace GuiBase
} // namespace Ra

#endif //  RADIUMENGINE_SELECTION_MANAGER_HPP_
