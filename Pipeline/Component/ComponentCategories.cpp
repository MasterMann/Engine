#include "ComponentCategories.h"
#include "ComponentCategory.h"
#include "Pipeline/Component/ComponentExceptions.h"

using namespace Component;

ComponentCategories* ComponentCategories::s_Instance = NULL;
i32 ComponentCategories::s_InitCount = 0;

ComponentCategories::ComponentCategories()
{
  m_Categories.insert( M_ComponentCategories::value_type( ComponentCategoryTypes::Animation,      new ComponentCategory( ComponentCategoryTypes::Animation,     1, "Animation", "Components for animating.", "" ) ) );
  m_Categories.insert( M_ComponentCategories::value_type( ComponentCategoryTypes::Cinematic,      new ComponentCategory( ComponentCategoryTypes::Cinematic,     0, "Cinematic", "Cinematic attributes.", "" ) ) );
  m_Categories.insert( M_ComponentCategories::value_type( ComponentCategoryTypes::File,           new ComponentCategory( ComponentCategoryTypes::File,          1, "File", "Components that reference another file.", "File attributes might reference a Maya file, texture file, another entity file, etc." ) ) );
  m_Categories.insert( M_ComponentCategories::value_type( ComponentCategoryTypes::Font,           new ComponentCategory( ComponentCategoryTypes::Font,          1, "Font", "Font Components", "These attributes are used for describing the in-game font." ) ) );
  m_Categories.insert( M_ComponentCategories::value_type( ComponentCategoryTypes::Lighting,       new ComponentCategory( ComponentCategoryTypes::Lighting,      1, "Lighting", "Components for creating lit objects.", "" ) ) );
  m_Categories.insert( M_ComponentCategories::value_type( ComponentCategoryTypes::LOD,            new ComponentCategory( ComponentCategoryTypes::LOD,           1, "LOD", "Level of detail.", "Components for setting up level of detail on objects." ) ) );
  m_Categories.insert( M_ComponentCategories::value_type( ComponentCategoryTypes::Meta,           new ComponentCategory( ComponentCategoryTypes::Meta,          1, "MetaInfo", "Components with extra information for the tools.", "These attributes contain information that is not used by the game.  Instead, they contain information that can be displayed within the tools such as showing a screenshot of an entity." ) ) );
  m_Categories.insert( M_ComponentCategories::value_type( ComponentCategoryTypes::Misc,           new ComponentCategory( ComponentCategoryTypes::Misc,          1, "Misc.", "Miscellaneous attributes that don't fit in other categories.", "" ) ) );
  m_Categories.insert( M_ComponentCategories::value_type( ComponentCategoryTypes::Gameplay,       new ComponentCategory( ComponentCategoryTypes::Gameplay,      1, "Gameplay", "Components for setting up gameplay.", "" ) ) );
  m_Categories.insert( M_ComponentCategories::value_type( ComponentCategoryTypes::Configuration,  new ComponentCategory( ComponentCategoryTypes::Configuration, 1, "Configuration", "Components for setting/controlling configuration values.", "" ) ) );
  m_Categories.insert( M_ComponentCategories::value_type( ComponentCategoryTypes::PostEffects,    new ComponentCategory( ComponentCategoryTypes::PostEffects,   1, "Post Effects", "Post-processing effects.", "" ) ) );
  m_Categories.insert( M_ComponentCategories::value_type( ComponentCategoryTypes::Joint,          new ComponentCategory( ComponentCategoryTypes::Joint,         1, "Joint", "Joint data.", "" ) ) );
  // TODO: validation, make sure there are unique names, build additonal maps and sets for faster lookup!
}

ComponentCategories::~ComponentCategories()
{

}

const M_ComponentCategories& ComponentCategories::GetCategories() const
{
  return m_Categories;
}

const ComponentCategoryPtr& ComponentCategories::GetCategory( ComponentCategoryType type )
{
  M_ComponentCategories::const_iterator found = m_Categories.find( type );
  
  if ( found == m_Categories.end() )
  {
    throw Nocturnal::Exception( TXT( "There is no category with that type" ) );
  }

  return found->second;
}

void ComponentCategories::Categorize( const ComponentPtr& attribute )
{
  M_ComponentCategories::iterator found = m_Categories.find( attribute->GetCategoryType() );
  if ( found != m_Categories.end() )
  {
    ComponentCategoryPtr& category = found->second;
    category->AddComponent( attribute );
  }
  else
  {
    throw Nocturnal::Exception( TXT( "Component '%s' could not be categorized." ), attribute->GetClass()->m_ShortName.c_str() );
  }
}