﻿

//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------
#include "pch.h"
#include "MainPage.xaml.h"




void ::WinRT_First_Steps::MainPage::InitializeComponent()
{
    if (_contentLoaded)
        return;

    _contentLoaded = true;

    // Call LoadComponent on ms-appx:///MainPage.xaml
    ::Windows::UI::Xaml::Application::LoadComponent(this, ref new ::Windows::Foundation::Uri(L"ms-appx:///MainPage.xaml"), ::Windows::UI::Xaml::Controls::Primitives::ComponentResourceLocation::Application);

    // Get the AppBar named 'appBar'
    appBar = safe_cast<::Windows::UI::Xaml::Controls::AppBar^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"appBar"));
    // Get the Button named 'ResolutionPopup_Button'
    ResolutionPopup_Button = safe_cast<::Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"ResolutionPopup_Button"));
    // Get the Button named 'colorPopup_Button'
    colorPopup_Button = safe_cast<::Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"colorPopup_Button"));
    // Get the Popup named 'resolutionPopup'
    resolutionPopup = safe_cast<::Windows::UI::Xaml::Controls::Primitives::Popup^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"resolutionPopup"));
    // Get the Popup named 'colorPopup'
    colorPopup = safe_cast<::Windows::UI::Xaml::Controls::Primitives::Popup^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"colorPopup"));
    // Get the StackPanel named 'colorPopupPanel'
    colorPopupPanel = safe_cast<::Windows::UI::Xaml::Controls::StackPanel^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"colorPopupPanel"));
    // Get the StackPanel named 'resolutionPopupPanel'
    resolutionPopupPanel = safe_cast<::Windows::UI::Xaml::Controls::StackPanel^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"resolutionPopupPanel"));
}

void ::WinRT_First_Steps::MainPage::Connect(int connectionId, Platform::Object^ target)
{
    switch (connectionId)
    {
    case 1:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::SuspendTestButton_Click);
        break;
    case 2:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::ResumeTestButton_Click);
        break;
    case 3:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::ResolutionPopup_Click);
        break;
    case 4:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::colorPopup_Click);
        break;
    case 5:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::SoftReset_Click);
        break;
    case 6:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::Undo_Click);
        break;
    case 7:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::colorPopup_Amber);
        break;
    case 8:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::colorPopup_Green);
        break;
    case 9:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::colorPopup_Paperwhite);
        break;
    case 10:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::ResolutionPopup_Big);
        break;
    case 11:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::ResolutionPopup_Medium);
        break;
    case 12:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::ResolutionPopup_Small);
        break;
    case 13:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::WinRT_First_Steps::MainPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&MainPage::ResolutionPopup_24x80);
        break;
    }
    (void)connectionId; // Unused parameter
    (void)target; // Unused parameter
    _contentLoaded = true;
}

