/*#include "Precompile.h"*/
#include "EntityInstancePanel.h"
#include "EntitySet.h"

#include "Foundation/Inspect/Controls/ValueControl.h"
#include "Foundation/Inspect/Controls/ButtonControl.h"

#include "Core/Asset/AssetInit.h"
#include "Foundation/Component/ComponentHandle.h"
#include "Core/Asset/AssetClass.h"
#include "Foundation/Log.h"
#include "Platform/Process.h"
#include "Volume.h"

#include "Light.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Asset;
using namespace Helium::Core;

EntityPanel::EntityPanel(PropertiesGenerator* generator, const OS_SceneNodeDumbPtr& selection)
: InstancePanel (generator, selection)
, m_EntityPath( NULL )
{
    a_Name.Set( TXT( "EntityInstance" ) );

    m_Generator->PushContainer();
    {
        static const tstring helpText = TXT( "This sets the path on disk of the entity instance." );
        m_Generator->AddLabel( TXT( "Class Path" ) )->a_HelpText.Set( helpText );

        m_EntityPath = m_Generator->AddValue<Core::EntityInstance, tstring>( m_Selection, &Core::EntityInstance::GetEntityAssetPath, &Core::EntityInstance::SetEntityAssetPath );
        m_EntityPath->a_HelpText.Set( helpText );
        m_EntityPath->e_ControlChanging.AddMethod( this, &EntityPanel::OnEntityAssetChanging );
        m_EntityPath->e_ControlChanged.AddMethod( this, &EntityPanel::OnEntityAssetChanged );

        Inspect::FileDialogButton* fileButton = m_Generator->AddFileDialogButton<Core::EntityInstance, tstring>( m_Selection, &Core::EntityInstance::GetEntityAssetPath, &Core::EntityInstance::SetEntityAssetPath );
        fileButton->e_ControlChanging.AddMethod( this, &EntityPanel::OnEntityAssetChanging );
        fileButton->e_ControlChanged.AddMethod( this, &EntityPanel::OnEntityAssetChanged );
        fileButton->a_HelpText.Set( TXT( "Clicking this button will allow you to select a different entity from the disk for this instance." ) );

        tstring filter;
        Reflect::GetClass<Asset::Entity>()->GetProperty( Asset::AssetProperties::FileFilter, filter );

        if ( !filter.empty() )
        {
            fileButton->a_Filter.Set( filter );
        }
        else
        {
            // There's a problem, better disable the button
            fileButton->a_IsEnabled.Set( false );
        }

        m_EntityPath->SetProperty( TXT( "FileFilter" ), filter );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Class Buttons" ) );

        Inspect::Button* refreshButton = m_Generator->AddButton( Inspect::ButtonClickedSignature::Delegate( this, &EntityPanel::OnEntityAssetRefresh ) );
        refreshButton->a_Icon.Set( TXT( "actions/view-refresh" ) );
        refreshButton->a_HelpText.Set( TXT( "Refresh" ) );

        Inspect::Button* lunaButton = m_Generator->AddButton( Inspect::ButtonClickedSignature::Delegate( this, &EntityPanel::OnEntityAssetEditAsset ) );
        lunaButton->a_Icon.Set( TXT( "asset_editor" ) );
        lunaButton->a_HelpText.Set( TXT( "Edit this entity class in Editor's Asset Editor" ) );

        Inspect::Button* mayaButton = m_Generator->AddButton( Inspect::ButtonClickedSignature::Delegate( this, &EntityPanel::OnEntityAssetEditArt ) );
        mayaButton->a_IsEnabled.Set( m_Selection.Size() == 1 );
        mayaButton->a_Icon.Set( TXT( "maya" ) );
        mayaButton->a_HelpText.Set( TXT( "Edit this entity class's art in Maya" ) );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        static const tstring helpText = TXT( "This determines if a pointer should be drawn in the 3d view where this entity is placed." );
        m_Generator->AddLabel( TXT( "Show Pointer" ) )->a_HelpText.Set( helpText );
        m_Generator->AddCheckBox<Core::EntityInstance, bool>( m_Selection, &Core::EntityInstance::IsPointerVisible, &Core::EntityInstance::SetPointerVisible, false )->a_HelpText.Set( helpText );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        static const tstring helpText = TXT( "This determines if the bounding box for the entity should be drawn in the 3d view where this entity is placed." );
        m_Generator->AddLabel( TXT( "Show Bounds" ) )->a_HelpText.Set( helpText );
        m_Generator->AddCheckBox<Core::EntityInstance, bool>( m_Selection, &Core::EntityInstance::IsBoundsVisible, &Core::EntityInstance::SetBoundsVisible, false )->a_HelpText.Set( helpText );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        static const tstring helpText = TXT( "This determines if the entity's geometry should be drawn in the 3d view." );
        m_Generator->AddLabel( TXT( "Show Geometry" ) )->a_HelpText.Set( helpText );
        m_Generator->AddCheckBox<Core::EntityInstance, bool>( m_Selection, &Core::EntityInstance::IsGeometryVisible,  &Core::EntityInstance::SetGeometryVisible, false )->a_HelpText.Set( helpText );
    }
    m_Generator->Pop();
}

///////////////////////////////////////////////////////////////////////////////
// Validation function for the controls that change the EntityInstance Class field.
// Returns true if the new value for the EntityInstance Class field can be resolved to
// a file TUID.
//
void EntityPanel::OnEntityAssetChanging( const Inspect::ControlChangingArgs& args )
{
    bool allow = false;

    tstring newValue;
    Reflect::Serializer::GetValue(args.m_NewValue, newValue);

    Helium::Path path( newValue );
    if ( path.Exists() )
    {
        // Make sure the file has a reflect extension
        std::set< tstring > extensions;
        Reflect::Archive::GetExtensions( extensions );
        if ( extensions.find( path.Extension() ) != extensions.end() )
        {
            allow = true;
        }
    }

    args.m_Veto = !allow;
}

void EntityPanel::OnEntityAssetChanged( const Inspect::ControlChangedArgs& args )
{
}

void EntityPanel::OnEntityAssetRefresh( const Inspect::ButtonClickedArgs& args )
{
    Core::Scene* scene = NULL;

    // when we refresh, reload the common class set information in case
    // we did something like reexport an art class, while luna is still opened
    std::set< EntitySet* > reloadQueue;     // entities we want to reload

    std::set< tstring > files;

    OS_SceneNodeDumbPtr::Iterator selectionIter = m_Selection.Begin();
    OS_SceneNodeDumbPtr::Iterator selectionEnd = m_Selection.End();
    for (; selectionIter != selectionEnd; ++selectionIter )
    {
        Core::EntityInstance* entity = Reflect::ObjectCast< Core::EntityInstance >( *selectionIter );

        if ( !scene )
        {
            Core::SceneNode* node = Reflect::ObjectCast< Core::SceneNode >( *selectionIter );
            scene = node->GetOwner();
        }

        if( entity->GetClassSet( ) )
        {
            reloadQueue.insert( entity->GetClassSet( ) );
        }

        if (entity->IsGeometryVisible())
        {
            for ( int i=0; i<GeometryModes::Count; i++ )
            {
                Core::Scene* nestedScene = entity->GetNestedScene(entity->GetOwner()->GetViewport()->GetGeometryMode());

                if (nestedScene)
                {
                    if ( files.insert( nestedScene->GetPath().Get() ).second )
                    {
                        // Only reload the scene if we haven't already done so during this iteration.
                        nestedScene->Reload();
                    }
                }
            }
        }
    }

    std::set< EntitySet* >::iterator itr = reloadQueue.begin();
    std::set< EntitySet* >::iterator end = reloadQueue.end();

    while( itr != end )
    {
        EntitySet* entClassSet = ObjectCast< EntitySet >( *itr );

        if( entClassSet )
        {
            entClassSet->LoadAssetClass( );
        }

        itr++;
    }

    if (scene)
    {
        scene->Execute(false);
    }
}

void EntityPanel::OnEntityAssetEditAsset( const Inspect::ButtonClickedArgs& args )
{
    std::set< tstring > files;
    OS_SceneNodeDumbPtr::Iterator selectionIter = m_Selection.Begin();
    OS_SceneNodeDumbPtr::Iterator selectionEnd = m_Selection.End();
    for ( ; selectionIter != selectionEnd; ++selectionIter )
    {
        Core::EntityInstance* entity = Reflect::ObjectCast< Core::EntityInstance >( *selectionIter );
        if ( entity )
        {
            tstring fileToEdit = entity->GetEntityAssetPath();
            if ( !fileToEdit.empty() )
            {
                files.insert( fileToEdit );
            }
        }
    }

    std::set< tstring >::const_iterator fileItr = files.begin();
    std::set< tstring >::const_iterator fileEnd = files.end();
    for ( ; fileItr != fileEnd; ++fileItr )
    {
#pragma TODO( "Open the file for edit" )
        HELIUM_BREAK();
    }
}

void EntityPanel::OnEntityAssetEditArt( const Inspect::ButtonClickedArgs& args )
{
}
