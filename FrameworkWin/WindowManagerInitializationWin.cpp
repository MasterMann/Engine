//----------------------------------------------------------------------------------------------------------------------
// WindowManagerInitializationWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkWinPch.h"
#include "FrameworkWin/WindowManagerInitializationWin.h"

#include "WinWindowing/WinWindowManager.h"

namespace Lunar
{
    /// Constructor.
    ///
    /// @param[in] hInstance  Handle to the application instance.
    /// @param[in] nCmdShow   Flags specifying how the application window should be shown (passed in from WinMain()).
    WindowManagerInitializationWin::WindowManagerInitializationWin( HINSTANCE hInstance, int nCmdShow )
        : m_hInstance( hInstance )
        , m_nCmdShow( nCmdShow )
    {
        HELIUM_ASSERT( hInstance );
    }

    /// @copydoc WindowManager::Initialize()
    bool WindowManagerInitializationWin::Initialize()
    {
        WinWindowManager* pWindowManager = WinWindowManager::CreateStaticInstance();
        HELIUM_ASSERT( pWindowManager );
        if( !pWindowManager )
        {
            return false;
        }

        if( !pWindowManager->Initialize( m_hInstance, m_nCmdShow ) )
        {
            WindowManager::DestroyStaticInstance();

            return false;
        }

        return true;
    }
}