#ifndef RADIUMENGINE_ENTITYTREEMODEL_HPP
#define RADIUMENGINE_ENTITYTREEMODEL_HPP

#include <GuiBase/RaGuiBase.hpp>

#include <Engine/ItemModel/ItemEntry.hpp>
#include <Engine/RadiumEngine.hpp>

#include <GuiBase/TreeModel/TreeModel.hpp>
namespace Ra
{
    namespace GuiBase
    {

        class RA_GUIBASE_API EngineTreeItem : public TreeItem
        {
        public:

            std::string getName() const override
            {
                return getEntryName( Engine::RadiumEngine::getInstance(), m_entry);
            }

            bool isValid() const override
            {
                return m_entry.isValid();
            }

            bool isSelectable() const override
            {
                return m_entry.isSelectable();
            }
        public:
            Engine::ItemEntry m_entry;
        };



        /// Implementation of QAbstractItemModel to show the engine objects
        /// as a tree in the main GUI.
        class RA_GUIBASE_API ItemModel : public TreeModel
        {
            Q_OBJECT
        public:

            ItemModel ( const Engine::RadiumEngine* engine, QObject* parent = nullptr)
                    : TreeModel( parent )
                    , m_engine( engine )
            {
                buildModel();
            }

            // Other functions

            /// Returns the entry corresponding to a given index or an invalid entry
            /// if the index doesn't match any entry.
            const Engine::ItemEntry& getEntry( const QModelIndex& index )const;

            /// Returns the index corresponding to the given entry if it exists in the
            /// model, or an invalid index if not found.
            QModelIndex findEntryIndex( const Engine::ItemEntry& entry ) const;

        public slots:

            void addItem( const Engine::ItemEntry& ent );

            void removeItem( const Engine::ItemEntry& ent );


        protected:
            /// Internal function to build the tree.
            void buildModel() override;

            std::string getHeaderString() const override{ return "Engine Objects Tree"; }

        protected:
            /// Engine instance.
            const Engine::RadiumEngine* m_engine;

        };


    } // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_ENTITYTREEMODEL_HPP
