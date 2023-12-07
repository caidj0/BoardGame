// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "Board.h"
#include "winrt/Windows.UI.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

Board* myBoard;

namespace winrt::BoardGame::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        ExtendsContentIntoTitleBar(true);
        SetTitleBar(AppTitle());
        createBoard(2, 2, 2,false);
    }

    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
    
}


void winrt::BoardGame::implementation::MainWindow::AppBarButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
{
    ContentDialog dialog;
    dialog.XamlRoot(Content().XamlRoot());
    dialog.Title(winrt::box_value(L"设置"));
    dialog.PrimaryButtonText(L"确定");
    dialog.CloseButtonText(L"取消");

    dialog.Content(SettingPage());
    dialog.DefaultButton(ContentDialogButton::Primary);
    dialog.PrimaryButtonClick([&](winrt::Microsoft::UI::Xaml::Controls::ContentDialog cd, winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs args)
        {
            createBoard(_wtoi(cd.Content().as<SettingPage>().FindName(L"Length_Input").as<TextBox>().Text().c_str()),
                _wtoi(cd.Content().as<SettingPage>().FindName(L"Weight_Input").as<TextBox>().Text().c_str()),
                _wtoi(cd.Content().as<SettingPage>().FindName(L"Color_Input").as<TextBox>().Text().c_str()),
                cd.Content().as<SettingPage>().FindName(L"FlipOneCheck").as<CheckBox>().IsChecked().GetBoolean());
        });
    
    dialog.ShowAsync();
}

void winrt::BoardGame::implementation::MainWindow::createBoard(int m, int n, int color,bool flipOneCheck)
{
    if (myBoard != nullptr) delete myBoard;
    myBoard = new Board(m, n, color);
    myBoard->spread();

    RowButtons().Children().Clear();
    ColumnButtons().Children().Clear();
    ColumnButtons().RowDefinitions().Clear();
    RowButtons().ColumnDefinitions().Clear();

    for (int i = 0; i < n; i++)
        ColumnButtons().ColumnDefinitions().Append(Controls::ColumnDefinition());

    for (int i = 0; i < m; i++)
        RowButtons().RowDefinitions().Append(Controls::RowDefinition());

    for (int i = 0; i < m; i++)
    {
        Button bt;
        bt.Height(50);
        bt.Width(50);
        Grid::SetRow(bt, i);
        Grid::SetColumn(bt, 0);
        bt.Click([&,i](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&){
            myBoard->flipRow(i);
            drawBoard(flipOneCheck);
            });
        RowButtons().Children().Append(bt);
    }

    for (int i = 0; i < n; i++)
    {
        Button bt;
        bt.Height(50);
        bt.Width(50);
        Grid::SetRow(bt, 0);
        Grid::SetColumn(bt, i);
        bt.Click([&,i](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&) {
            myBoard->flipColumn(i);
            drawBoard(flipOneCheck);
            });
        ColumnButtons().Children().Append(bt);
    }

    
    drawBoard(flipOneCheck);
}

Windows::UI::Color ConvertHSVToRGB(double hue, double saturation, double value)
{
    // 转换 HSV 到 RGB
    double chroma = value * saturation;
    double hPrime = std::fmod(hue / 60.0, 6.0);
    double x = chroma * (1 - std::fabs(std::fmod(hPrime, 2) - 1));
    double r, g, b;

    if (hPrime >= 0 && hPrime < 1)
    {
        r = chroma;
        g = x;
        b = 0;
    }
    else if (hPrime >= 1 && hPrime < 2)
    {
        r = x;
        g = chroma;
        b = 0;
    }
    else if (hPrime >= 2 && hPrime < 3)
    {
        r = 0;
        g = chroma;
        b = x;
    }
    else if (hPrime >= 3 && hPrime < 4)
    {
        r = 0;
        g = x;
        b = chroma;
    }
    else if (hPrime >= 4 && hPrime < 5)
    {
        r = x;
        g = 0;
        b = chroma;
    }
    else
    {
        r = chroma;
        g = 0;
        b = x;
    }

    double m = value - chroma;

    // 将 RGB 范围从 [0, 1] 映射到 [0, 255]
    r = (r + m) * 255.0;
    g = (g + m) * 255.0;
    b = (b + m) * 255.0;

    // 将 r、g、b 转换为整数
    int red = static_cast<int>(r);
    int green = static_cast<int>(g);
    int blue = static_cast<int>(b);

    // 创建 Windows::UI::Color 对象
    return Windows::UI::ColorHelper::FromArgb(255,red,green,blue);
}

void winrt::BoardGame::implementation::MainWindow::drawBoard(bool flipOneCheck)
{
    MainButtons().Children().Clear();
    MainButtons().RowDefinitions().Clear();
    MainButtons().ColumnDefinitions().Clear();

    for (int i = 0; i < myBoard->m; i++)
        MainButtons().RowDefinitions().Append(Controls::RowDefinition());

    for (int i = 0; i < myBoard->n; i++)
        MainButtons().ColumnDefinitions().Append(Controls::ColumnDefinition());

    for (int i = 0; i < myBoard->m; i++)
    {
        for (int j = 0; j < myBoard->n; j++)
        {
            Button bt;
            bt.Height(50);
            bt.Width(50);
            if (flipOneCheck)
            {
                bt.Click([&, i, j](winrt::Windows::Foundation::IInspectable const& handler, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
                    {
                        myBoard->flipOne(i, j);
                        handler.as<Button>().Background(Media::SolidColorBrush(ConvertHSVToRGB(360 / myBoard->color * myBoard->board[i][j], 0.65, 1)));
                    });
            }
            bt.Background(Media::SolidColorBrush(ConvertHSVToRGB(360 / myBoard->color * myBoard->board[i][j],0.65,1)));
            Grid::SetRow(bt, i);
            Grid::SetColumn(bt, j);
            MainButtons().Children().Append(bt);
        }
    }
}
