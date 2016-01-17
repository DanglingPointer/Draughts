using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Checkers;

namespace WinCheckers
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            m_Game = new Gameplay();
        }

        private bool m_MousePressed = false;
        private int m_DragStartRow;
        private int m_DragStartCol;

        private bool m_SideChosen = false;
        private Gameplay m_Game;

        private void newGameBtn_Clicked(object sender, RoutedEventArgs e)
        {
            m_Game = new Gameplay();
            m_SideChosen = false;
            ShowPieces();
        }

        private void whiteBtn_Checked(object sender, RoutedEventArgs e)
        {
            if (!m_SideChosen)
            {
                m_Game.WhitePlayer = true;
                m_SideChosen = true;
            }
        }

        private void blackBtn_Checked(object sender, RoutedEventArgs e)
        {
            if (!m_SideChosen)
            {
                m_Game.WhitePlayer = false;
                m_SideChosen = true;
            }
        }

        private void mousePressed(object sender, MouseButtonEventArgs e)
        {
            if (m_SideChosen)
            {
                m_MousePressed = true;
                // assign m_whereMousePressed
            }
        }

        private void mouseReleased(object sender, MouseButtonEventArgs e)
        {
            if (m_SideChosen)
            {
                m_MousePressed = false;
                // do the rest of the stuff
            }
        }

        private void ShowPieces()
        {
            for (int grow = 0; grow < 8; ++grow)
            {
                int row = 7 - grow;
                for (int gcol = 0; gcol < 8; ++gcol)
                {
                    int col = gcol;
                    if ((m_Game[grow, gcol] & Piece.White) == Piece.White)
                    {
                        if ((m_Game[grow, gcol] & Piece.King) == Piece.King) // White king
                        {
                            foreach (UIElement child in mygrid.Children)
                            {
                                if (Grid.GetRow(child) == row && Grid.GetColumn(child) == col && child is Ellipse)
                                {
                                    Ellipse myellipse = child as Ellipse;
                                    if (myellipse.Fill is RadialGradientBrush) // King piece
                                    {
                                        RadialGradientBrush brush = myellipse.Fill as RadialGradientBrush;
                                        if (brush.GradientStops[1].Color == Colors.White)
                                            child.Opacity = 1;
                                        else
                                            child.Opacity = 0;
                                    }
                                    else // Man piece
                                        child.Opacity = 0;
                                }
                            }
                        }
                        else // White man
                        {
                            foreach (UIElement child in mygrid.Children)
                            {
                                if (Grid.GetRow(child) == row && Grid.GetColumn(child) == col && child is Ellipse)
                                {
                                    Ellipse myellipse = child as Ellipse;
                                    if (myellipse.Fill is SolidColorBrush) // Man piece
                                    {
                                        SolidColorBrush scb = myellipse.Fill as SolidColorBrush;
                                        if (scb.Color == Colors.White)
                                            child.Opacity = 1;
                                        else
                                            child.Opacity = 0;
                                    }
                                    else // King piece
                                        child.Opacity = 0;
                                }
                            }
                        }
                    }
                    else if ((m_Game[grow, gcol] & Piece.Black) == Piece.Black)
                    {
                        if ((m_Game[grow, gcol] & Piece.King) == Piece.King) // black king
                        {
                            foreach (UIElement child in mygrid.Children)
                            {
                                if (Grid.GetRow(child) == row && Grid.GetColumn(child) == col && child is Ellipse)
                                {
                                    Ellipse myellipse = child as Ellipse;
                                    if (myellipse.Fill is RadialGradientBrush) // King piece
                                    {
                                        RadialGradientBrush brush = myellipse.Fill as RadialGradientBrush;
                                        if (brush.GradientStops[1].Color == Colors.Black)
                                            child.Opacity = 1;
                                        else
                                            child.Opacity = 0;
                                    }
                                    else // Man piece
                                        child.Opacity = 0;
                                }
                            }

                        }
                        else // Black man
                        {
                            foreach (UIElement child in mygrid.Children)
                            {
                                if (Grid.GetRow(child) == row && Grid.GetColumn(child) == col && child is Ellipse)
                                {
                                    Ellipse myellipse = child as Ellipse;
                                    if (myellipse.Fill is SolidColorBrush) // Man piece
                                    {
                                        SolidColorBrush scb = myellipse.Fill as SolidColorBrush;
                                        if (scb.Color == Colors.Black)
                                            child.Opacity = 1;
                                        else
                                            child.Opacity = 0;
                                    }
                                    else // King piece
                                        child.Opacity = 0;
                                }
                            }
                        }
                    }
                }
            }

        }

    }
}
