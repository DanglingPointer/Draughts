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
        }

        private bool m_MousePressed = false;
        private Point m_whereMousePressed; 

        private void newGameBtn_Clicked(object sender, RoutedEventArgs e)
        {
            // set all pieces to the initial positions
        }

        private void whiteBtn_Checked(object sender, RoutedEventArgs e)
        {
            // player has chosen white side
        }

        private void blackBtn_Checked(object sender, RoutedEventArgs e)
        {
            // Player is on the dark side
        }

        private void mousePressed(object sender, MouseButtonEventArgs e)
        {
            m_MousePressed = true;
            // assign m_whereMousePressed
        }

        private void mouseReleased(object sender, MouseButtonEventArgs e)
        {
            m_MousePressed = false;
            // do the rest of the stuff
        }
    }
}
