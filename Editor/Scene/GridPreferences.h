#pragma once

#include "Foundation/Math/Color3.h"
#include "Foundation/Reflect/Element.h"
#include "Editor/Settings.h"

namespace Helium
{
    namespace Editor
    {
        namespace GridUnits
        {
            enum GridUnit
            {
                Meters,
                Centimeters,
            };

            static void GridUnitEnumerateEnumeration( Reflect::Enumeration* info )
            {
                info->AddElement(Meters, TXT( "Meters" ) );
                info->AddElement(Centimeters, TXT( "Centimeters" ) );
            }
        }
        typedef GridUnits::GridUnit GridUnit;

        class GridPreferences : public Reflect::ConcreteInheritor< GridPreferences, Settings >
        {
        public:
            GridPreferences( const tstring& version = TXT( "" ),
                GridUnit units = GridUnits::Meters,
                u32 width = 12,
                u32 length = 12,
                f32 majorStep = 5.0f,
                f32 minorStep = 1.0f,
                Math::Color3 axisColor = Math::Color3( Math::Vector3( 0.0f, 0.0f, 0.0f ) ),
                Math::Color3 majorColor = Math::Color3( Math::Vector3( 0.5f, 0.5f, 0.5f ) ),
                Math::Color3 minorColor = Math::Color3( Math::Vector3( 0.5f, 0.5f, 0.5f ) ) );
            ~GridPreferences();

            virtual void PostDeserialize();

            u32 GetWidth();
            u32 GetLength();
            f32 GetMajorStep();
            f32 GetMinorStep();
            const Math::Color3& GetAxisColor();
            const Math::Color3& GetMajorColor();
            const Math::Color3& GetMinorColor();

        private:
            void OnChanged( const Reflect::ElementChangeArgs& args );

            static f32 GetConversionFactor( GridUnit units );
            static f32 ConvertUnits( f32 sourceValue, GridUnit sourceUnits, GridUnit destinationUnits );

        private:
            GridUnit m_Units;
            GridUnit m_PreviousUnits;
            u32 m_Width;
            u32 m_Length;
            f32 m_MajorStep;
            f32 m_MinorStep;
            Math::Color3 m_AxisColor;
            Math::Color3 m_MajorColor;
            Math::Color3 m_MinorColor;

        public:
            static void EnumerateClass( Reflect::Compositor< GridPreferences >& comp )
            {
                comp.AddEnumerationField( &GridPreferences::m_Units, "m_Units" );
                comp.AddField( &GridPreferences::m_Width, "m_Width" );
                comp.AddField( &GridPreferences::m_Length, "m_Length" );
                comp.AddField( &GridPreferences::m_MajorStep, "m_MajorStep" );
                comp.AddField( &GridPreferences::m_MinorStep, "m_MinorStep" );
                comp.AddField( &GridPreferences::m_AxisColor, "m_AxisColor" );
                comp.AddField( &GridPreferences::m_MajorColor, "m_MajorColor" );
                comp.AddField( &GridPreferences::m_MinorColor, "m_MinorColor" );
            }
        };

        typedef Helium::SmartPtr<GridPreferences> GridPreferencesPtr;
    }
}