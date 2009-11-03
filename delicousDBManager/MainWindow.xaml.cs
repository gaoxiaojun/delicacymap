using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
//using System.Data.SQLite;

namespace delicousDBManager
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    [System.Runtime.InteropServices.ComVisible(true)]
    public class Helper
    {
        public Helper(MainWindow main)
        {
            this.main = main;
        }
        public void SetLocation(double a, double b)
        {
            main.SetLocation(a, b);
        }

        private MainWindow main;
    }
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            try
            {
                InitializeComponent();
                Map.Source = new Uri(Environment.CurrentDirectory + @"\get_location.htm", UriKind.Absolute);
                Map.ObjectForScripting = new Helper(this);
                delicacyDBTableAdapters.RestaurantsTableAdapter adapter = new delicousDBManager.delicacyDBTableAdapters.RestaurantsTableAdapter();
                delicacyDBTableAdapters.RestaurantTypesTableAdapter typeadapter = new delicousDBManager.delicacyDBTableAdapters.RestaurantTypesTableAdapter();
                adapter.Fill(restaurants);
                typeadapter.Fill(types);
                RestaurantTypes.ItemsSource = types;
                RList.ItemsSource = restaurants;
            }
            catch (Exception e)
            {

            }
        }

        private delicacyDB.RestaurantsDataTable restaurants = new delicacyDB.RestaurantsDataTable();
        private delicacyDB.RestaurantTypesDataTable types = new delicacyDB.RestaurantTypesDataTable();

        public void SetLocation(double latitude, double longitude)
        {
            //latlong.Text = latitude + ", " + longitude;
        }
    }
}
