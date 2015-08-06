/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <MainApplication/Gui/EntityTreeModel.hpp>

#include <Core/String/StringUtils.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/Component.hpp>
#include <MainApplication/Gui/EntityTreeItem.hpp>

namespace Ra
{

    // FIXME(Charly): Remove data from constructor
    Gui::EntityTreeModel::EntityTreeModel ( const QStringList& headers, QObject* parent )
        : QAbstractItemModel ( parent )
    {
        QVector<EntityTreeItem::ItemData> rootData;
        foreach ( QString header, headers )
        {
            EntityTreeItem::ItemData data;
            data.data = QVariant ( header );
            rootData << data;
        }

        m_rootItem = new EntityTreeItem ( rootData );
    }

    Gui::EntityTreeModel::~EntityTreeModel()
    {
        delete m_rootItem;
    }

    int Gui::EntityTreeModel::columnCount ( const QModelIndex& parent ) const
    {
        return m_rootItem->getColumnCount();
    }

    QVariant Gui::EntityTreeModel::data ( const QModelIndex& index, int role ) const
    {
        if ( !index.isValid() )
        {
            return QVariant();
        }

        if ( role != Qt::DisplayRole && role != Qt::EditRole )
        {
            return QVariant();
        }

        EntityTreeItem* item = getItem ( index );

        return item->getData ( index.column() ).data;
    }

    Qt::ItemFlags Gui::EntityTreeModel::flags ( const QModelIndex& index ) const
    {
        if ( !index.isValid() )
        {
            return 0;
        }

        return Qt::ItemIsEditable | QAbstractItemModel::flags ( index );
    }

    Gui::EntityTreeItem* Gui::EntityTreeModel::getItem ( const QModelIndex& index ) const
    {
        if ( index.isValid() )
        {
            EntityTreeItem* item = static_cast<EntityTreeItem*> ( index.internalPointer() );
            if ( item )
            {
                return item;
            }
        }

        return m_rootItem;
    }

    QVariant Gui::EntityTreeModel::headerData ( int section, Qt::Orientation orientation, int role ) const
    {
        if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        {
            return m_rootItem->getData ( section ).data;
        }

        return QVariant();
    }

    QModelIndex Gui::EntityTreeModel::index ( int row, int column, const QModelIndex& parent ) const
    {
        if ( parent.isValid() && parent.column() != 0 )
        {
            return QModelIndex();
        }

        EntityTreeItem* parentItem = getItem ( parent );

        EntityTreeItem* childItem = parentItem->getChild ( row );
        if ( childItem )
        {
            return createIndex ( row, column, childItem );
        }
        else
        {
            return QModelIndex();
        }
    }

    bool Gui::EntityTreeModel::insertColumns ( int position, int columns, const QModelIndex& parent )
    {
        bool success;

        beginInsertColumns ( parent, position, position + columns - 1 );
        success = m_rootItem->insertColumns ( position, columns );
        endInsertColumns();

        return success;
    }

    bool Gui::EntityTreeModel::insertRows ( int position, int rows, const QModelIndex& parent )
    {
        EntityTreeItem* parentItem = getItem ( parent );
        bool success;

        beginInsertRows ( parent, position, position + rows - 1 );
        success = parentItem->insertChildren ( position, rows, m_rootItem->getColumnCount() );
        endInsertRows();

        return success;
    }

    QModelIndex Gui::EntityTreeModel::parent ( const QModelIndex& child ) const
    {
        if ( !child.isValid() )
        {
            return QModelIndex();
        }

        EntityTreeItem* childItem = getItem ( child );
        EntityTreeItem* parentItem = childItem->getParentItem();

        if ( parentItem == m_rootItem )
        {
            return QModelIndex();
        }

        return createIndex ( parentItem->getChildCount(), 0, parentItem );
    }

    bool Gui::EntityTreeModel::removeColumns ( int position, int columns, const QModelIndex& parent )
    {
        bool success;

        beginRemoveColumns ( parent, position, position + columns - 1 );
        success = m_rootItem->removeColumns ( position, columns );
        endRemoveColumns();

        if ( m_rootItem->getColumnCount() == 0 )
        {
            removeRows ( 0, rowCount() );
        }

        return success;
    }

    bool Gui::EntityTreeModel::removeRows ( int position, int rows, const QModelIndex& parent )
    {
        EntityTreeItem* parentItem = getItem ( parent );
        bool success = true;

        beginRemoveRows ( parent, position, position + rows - 1 );
        success = parentItem->removeChildren ( position, rows );
        endRemoveRows();

        return success;
    }

    int Gui::EntityTreeModel::rowCount ( const QModelIndex& parent ) const
    {
        EntityTreeItem* parentItem = getItem ( parent );
        return parentItem->getChildCount();
    }

    bool Gui::EntityTreeModel::setData ( const QModelIndex& index, const QVariant& value, int role )
    {
        if ( role != Qt::EditRole )
        {
            return false;
        }

        EntityTreeItem* item = getItem ( index );
        EntityTreeItem::ItemData data = item->getData ( index.column() );
        data.data = value;
        bool result = item->setData ( index.column(), data );

        if ( result )
        {
            emit dataChanged ( index, index );
        }

        return result;
    }

    bool Gui::EntityTreeModel::setHeaderData ( int section, Qt::Orientation orientation, const QVariant& value, int role )
    {
        if ( role != Qt::EditRole || orientation != Qt::Horizontal )
        {
            return false;
        }

        EntityTreeItem::ItemData data;
        data.data = value;
        bool result = m_rootItem->setData ( section, data );

        if ( result )
        {
            emit headerDataChanged ( orientation, section, section );
        }

        return result;
    }

    void Gui::EntityTreeModel::entitiesUpdated ( const std::vector<Engine::Entity*>& entities )
    {
        for ( const auto& ent : entities )
        {
            uint row = rowCount();
            insertRows ( row, 1 );
            EntityTreeItem* item = getItem ( index ( row, 0 ) );

            EntityTreeItem::ItemData data;
            data.data = QString::fromStdString ( ent->getName() );
            data.entity = ent;
            data.isEntityNode = true;

            item->setData ( 0, data );

            insertComponents ( ent, item );
        }
    }

    void Gui::EntityTreeModel::insertComponents ( Engine::Entity* entity, EntityTreeItem* parent )
    {
        std::vector<Engine::Component*> components = entity->getComponents();

        for ( const auto comp : components )
        {
            std::string name = comp->getName();
            EntityTreeItem* item;

            QVector<EntityTreeItem::ItemData> vec;
            EntityTreeItem::ItemData data;
            data.data = QString::fromStdString ( name );
            data.component = comp;
            data.isComponentNode = true;

            vec << data;

            item = new EntityTreeItem ( vec, parent );
            parent->appendChild ( item );
        }
    }

    void Gui::EntityTreeModel::handleRename ( const QModelIndex& topLeft,
                                              const QModelIndex& bottomRight,
                                              const QVector<int>& )
    {
        EntityTreeItem* item = getItem ( topLeft );
        EntityTreeItem::ItemData data = item->getData ( 0 );

        if ( data.isEntityNode )
        {
            if ( data.entity )
            {
                data.entity->rename ( data.data.toString().toStdString() );
            }
        }

        if ( data.isComponentNode )
        {
            // do stuff
        }
    }

    namespace
    {
        bool firstTimeSelection = true;
        QModelIndex lastSelected;
    }

    void Gui::EntityTreeModel::handleSelect ( const QModelIndex& index )
    {
        EntityTreeItem::ItemData data;

        if ( !firstTimeSelection )
        {
            data = getItem ( lastSelected )->getData ( 0 );
            if ( data.isEntityNode )
            {
                data.entity->setSelected ( false );
            }

            if ( data.isComponentNode )
            {
                data.component->setSelected ( false );
            }
        }
        else
        {
            firstTimeSelection = false;
        }

        data = getItem ( index )->getData ( 0 );
        if ( data.isEntityNode )
        {
            data.entity->setSelected ( true );
        }

        if ( data.isComponentNode )
        {
            data.component->setSelected ( true );
        }

        lastSelected = index;
    }

} // namespace Ra
