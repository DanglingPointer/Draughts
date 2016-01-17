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
        private Point m_DragStart;

        private bool m_SideChosen = false;
        private Gameplay m_Game;

        private bool m_ShowBoxWhenChecked = true;

        private void newGameBtn_Clicked(object sender, RoutedEventArgs e)
        {
            m_Game = new Gameplay();
            m_SideChosen = false;
            WhiteRadButn.IsChecked = false;
            BlackRadButn.IsChecked = false;
            ShowPieces();
        }

        private void whiteBtn_Checked(object sender, RoutedEventArgs e)
        {
            if (!m_SideChosen)
            {
                m_Game.WhitePlayer = true;
                m_SideChosen = true;
            }
            else
            {
                if (m_ShowBoxWhenChecked)
                {
                    MessageBox.Show("Click the New Game button before choosing side!");
                    m_ShowBoxWhenChecked = false;
                }
                else
                {
                    m_ShowBoxWhenChecked = true;
                }
                if (m_Game.WhitePlayer)
                {
                    WhiteRadButn.IsChecked = true;
                    BlackRadButn.IsChecked = false;
                }
                else
                {
                    WhiteRadButn.IsChecked = false;
                    BlackRadButn.IsChecked = true;
                }
            }
        }

        private void blackBtn_Checked(object sender, RoutedEventArgs e)
        {
            if (!m_SideChosen)
            {
                m_Game.WhitePlayer = false;
                m_SideChosen = true;

                m_Game.AITurn();
                ShowPieces();
            }
            else
            {
                if (m_ShowBoxWhenChecked)
                {
                    MessageBox.Show("Click the New Game button before choosing side!");
                    m_ShowBoxWhenChecked = false;
                }
                else
                {
                    m_ShowBoxWhenChecked = true;
                }
                if (m_Game.WhitePlayer)
                {
                    WhiteRadButn.IsChecked = true;
                    BlackRadButn.IsChecked = false;
                }
                else
                {
                    WhiteRadButn.IsChecked = false;
                    BlackRadButn.IsChecked = true;
                }
            }
        }

        private void mousePressed(object sender, MouseButtonEventArgs e)
        {
            if (m_SideChosen)
            {
                m_MousePressed = true;
                m_DragStart = e.GetPosition(mygrid);
            }
        }

        private void mouseReleased(object sender, MouseButtonEventArgs e)
        {
            if (!m_SideChosen)
            {
                MessageBox.Show("Choose side first!");
            }
            else if (m_MousePressed)
            {
                Direction dirn;
                Point dragEnd = e.GetPosition(mygrid);
                if (dragEnd.X > m_DragStart.X)
                    dirn = (dragEnd.Y > m_DragStart.Y) ? Direction.RightDown : Direction.RightUp;
                else
                    dirn = (dragEnd.Y > m_DragStart.Y) ? Direction.LeftDown : Direction.LeftUp;
                if (m_Game.PlayerTurn(7 - Convert.ToInt32(m_DragStart.Y) / 55, Convert.ToInt32(m_DragStart.X) / 55, dirn))
                {
                    // ShowPieces(); // obsolete
                    m_Game.AITurn();
                    ShowPieces();
                }
                else
                    MessageBox.Show("Invalid move!");
                m_MousePressed = false;
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
                                        RadialGradientBrush brush = (RadialGradientBrush)myellipse.Fill;
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
                                        SolidColorBrush scb = (SolidColorBrush)myellipse.Fill;
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
                                        RadialGradientBrush brush = (RadialGradientBrush)myellipse.Fill;
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
                                        SolidColorBrush scb = (SolidColorBrush)myellipse.Fill;
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
                    else // Empty
                    {
                        foreach (UIElement child in mygrid.Children)
                        {
                            if (Grid.GetRow(child) == row && Grid.GetColumn(child) == col && child is Ellipse)
                                child.Opacity = 0;
                        }
                    }
                }
            }
        }

    }
}
