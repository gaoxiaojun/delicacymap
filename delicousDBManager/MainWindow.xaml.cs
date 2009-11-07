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
using System.Data;

namespace delicousDBManager
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            try
            {
                InitializeComponent();
                Map.Source = new Uri(Environment.CurrentDirectory + @"\get_location.htm", UriKind.Absolute);
                Map.ObjectForScripting = new Helper(this);
                this.Adapters_Restaurants = new delicousDBManager.delicacyDBTableAdapters.RestaurantsTableAdapter();
                this.Adapters_RestaurantTypes = new delicousDBManager.delicacyDBTableAdapters.RestaurantTypesTableAdapter();
                this.Adapters_Relation_Restaurant_RestaurantType = new delicousDBManager.delicacyDBTableAdapters.Relation_Restaurant_RestaurantTypeTableAdapter();
                Adapters_Relation_Restaurant_RestaurantType.Fill(dbset.Relation_Restaurant_RestaurantType);
                Adapters_Restaurants.Fill(dbset.Restaurants);
                Adapters_RestaurantTypes.Fill(dbset.RestaurantTypes);
                RestaurantTypes.ItemsSource = dbset.RestaurantTypes;
                RList.ItemsSource = dbset.Restaurants;
            }
            catch (Exception e)
            {

            }
        }

        private delicacyDB dbset = new delicacyDB();

        #region Adapters

        public delicacyDBTableAdapters.RestaurantsTableAdapter Adapters_Restaurants
        {
            get;
            set;
        }

        public delicacyDBTableAdapters.RestaurantTypesTableAdapter Adapters_RestaurantTypes
        {
            get;
            set;
        }

        public delicacyDBTableAdapters.Relation_Restaurant_CourseTableAdapter Adapters_Relation_Restaurant_Course
        {
            get;
            set;
        }

        public delicacyDBTableAdapters.Relation_Restaurant_RestaurantTypeTableAdapter Adapters_Relation_Restaurant_RestaurantType
        {
            get;
            set;
        }

        #endregion


        #region Tab Restaurants

        public void SetLocation(double latitude, double longitude)
        {
            if (Details.DataContext != null)
            {
                DataRowView v = (DataRowView)Details.DataContext;
                delicacyDB.RestaurantsRow row = (delicacyDB.RestaurantsRow)v.Row;
                row.Latitude = latitude;
                row.Longtitude = longitude;
            }
        }

        private void Restaurant_NewEntry_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var row = dbset.Restaurants.NewRestaurantsRow();
                row.Name = string.Empty;
                row.Latitude = 0;
                row.Longtitude = 0;
                row.AverageExpense = 0;
                row.Rating = 0;
                dbset.Restaurants.AddRestaurantsRow(row);
                RList.SelectedValue = row;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error!", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void Restaurant_Save_Click(object sender, RoutedEventArgs e)
        {
            if (Details.DataContext != null)
            {
                try
                {
                    DataRowView v = (DataRowView)Details.DataContext;
                    delicacyDB.RestaurantsRow row = (delicacyDB.RestaurantsRow)v.Row;
                    Adapters_Restaurants.Update(row);
                    if (RestaurantTypes.SelectedValue != null)
                    {
                        var RtypeRows = row.GetRelation_Restaurant_RestaurantTypeRows();
                        delicacyDB.Relation_Restaurant_RestaurantTypeRow row0 = null;
                        if (RtypeRows.Length != 0)
                        {
                            row0 = RtypeRows[0];
                            row0.RID = row.RID;
                            row0.TID = (long)RestaurantTypes.SelectedValue;
                        }
                        else
                        {
                            row0 = dbset.Relation_Restaurant_RestaurantType.AddRelation_Restaurant_RestaurantTypeRow(row,
                                ((delicacyDB.RestaurantTypesRow)((DataRowView)RestaurantTypes.SelectedItem).Row));
                        }
                        Adapters_Relation_Restaurant_RestaurantType.Update(row0);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error!", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
        }

        private void RList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (RList.SelectedItem != null)
            {
                delicacyDB.RestaurantsRow row = (delicacyDB.RestaurantsRow)RList.SelectedValue;
                if (row.Latitude != 0)
                {
                    try
                    {
                        Map.InvokeScript("SetMapCenter", row.Latitude, row.Longtitude);
                    }
                    catch (System.Exception ex)
                    {
                    	
                    }
                }
            }
        }

        #endregion

        #region Tab Course
        #endregion
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
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
}
