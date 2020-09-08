﻿

#pragma once
//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------

namespace Windows {
    namespace UI {
        namespace Xaml {
            namespace Controls {
                ref class Grid;
                ref class ToggleSwitch;
                ref class Button;
            }
        }
    }
}

namespace WinRT_First_Steps
{
    partial ref class DisplaySettings : public ::Windows::UI::Xaml::Controls::UserControl, 
        public ::Windows::UI::Xaml::Markup::IComponentConnector
    {
    public:
        void InitializeComponent();
        virtual void Connect(int connectionId, ::Platform::Object^ target);
    
    private:
        bool _contentLoaded;
    
        private: ::Windows::UI::Xaml::Controls::Grid^ HeaderBackground;
        private: ::Windows::UI::Xaml::Controls::ToggleSwitch^ EnableRevMenu;
        private: ::Windows::UI::Xaml::Controls::ToggleSwitch^ PascalSyntaxHighlighting;
        private: ::Windows::UI::Xaml::Controls::ToggleSwitch^ Boldface;
        private: ::Windows::UI::Xaml::Controls::Button^ BoldOn_Button;
        private: ::Windows::UI::Xaml::Controls::Button^ BoldOff_Button;
        private: ::Windows::UI::Xaml::Controls::Button^ RevMenuOn_Button;
        private: ::Windows::UI::Xaml::Controls::Button^ RevMenuOff_Button;
    };
}

