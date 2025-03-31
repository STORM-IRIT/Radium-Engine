#pragma once

#include <Core/Utils/Index.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Scene/ItemEntry.hpp>
#include <Gui/RaGui.hpp>
#include <Gui/TreeModel/TreeModel.hpp>
#include <string>

namespace Ra {
namespace Gui {

class RA_GUI_API EngineTreeItem : public TreeItem
{
  public:
    std::string getName() const override {
        return getEntryName( Engine::RadiumEngine::getInstance(), m_entry );
    }

    bool isValid() const override { return m_entry.isValid(); }

    bool isSelectable() const override { return m_entry.isSelectable(); }

  public:
    Engine::Scene::ItemEntry m_entry;
};

/// Implementation of QAbstractItemModel to show the engine objects
/// as a tree in the main GUI.
class RA_GUI_API ItemModel : public TreeModel
{
    Q_OBJECT
  public:
    explicit ItemModel( const Engine::RadiumEngine* engine, QObject* parent = nullptr ) :
        TreeModel( parent ), m_engine( engine ) {
        buildModel();
        connect( this, &TreeModel::dataChanged, this, &ItemModel::onDataChanged );
    }

    // Other functions

    /// Returns the entry corresponding to a given index or an invalid entry
    /// if the index doesn't match any entry.
    const Engine::Scene::ItemEntry& getEntry( const QModelIndex& index ) const;

    /// Returns the index corresponding to the given entry if it exists in the
    /// model, or an invalid index if not found.
    QModelIndex findEntryIndex( const Engine::Scene::ItemEntry& entry ) const;

  public slots:

    void addItem( const Engine::Scene::ItemEntry& ent );

    void removeItem( const Engine::Scene::ItemEntry& ent );

  private slots:

    void onDataChanged( const QModelIndex& topLeft,
                        const QModelIndex& bottomRight,
                        const QVector<int>& roles );

  signals:

    void visibilityROChanged( Ra::Core::Utils::Index roIndex, bool visible );

  protected:
    /// Internal function to build the tree.
    void buildModel() override;

    std::string getHeaderString() const override { return "Engine Objects Tree"; }

  protected:
    /// Engine instance.
    const Engine::RadiumEngine* m_engine;
};

} // namespace Gui
} // namespace Ra
