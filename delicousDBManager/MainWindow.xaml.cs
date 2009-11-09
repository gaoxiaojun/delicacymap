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
using System.Security.Cryptography;

namespace delicousDBManager
{
    public enum UserRelationship {Friend=0, BlackList, Unspecified = -1};

    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            try
            {
                this.Resources["RelationUserUser"] = Dbset.Relation_User_User;
                InitializeComponent();
                Map.Source = new Uri(Environment.CurrentDirectory + @"\get_location.htm", UriKind.Absolute);
                Map.ObjectForScripting = new Helper(this);

                #region DataBase Initialization

                Adapters.Relation_Restaurant_RestaurantTypeTableAdapter = new delicousDBManager.delicacyDBTableAdapters.Relation_Restaurant_RestaurantTypeTableAdapter();
                Adapters.Relation_User_RestaurantTableAdapter = new delicousDBManager.delicacyDBTableAdapters.Relation_User_RestaurantTableAdapter();
                Adapters.Relation_User_UserTableAdapter = new delicousDBManager.delicacyDBTableAdapters.Relation_User_UserTableAdapter();
                Adapters.Relation_Restaurant_CourseTableAdapter = new delicousDBManager.delicacyDBTableAdapters.Relation_Restaurant_CourseTableAdapter();
                Adapters.RestaurantsTableAdapter = new delicousDBManager.delicacyDBTableAdapters.RestaurantsTableAdapter();
                Adapters.RestaurantTypesTableAdapter = new delicousDBManager.delicacyDBTableAdapters.RestaurantTypesTableAdapter();
                Adapters.UsersTableAdapter = new delicousDBManager.delicacyDBTableAdapters.UsersTableAdapter();
                Adapters.CommentsTableAdapter = new delicousDBManager.delicacyDBTableAdapters.CommentsTableAdapter();

                Adapters.Relation_Restaurant_RestaurantTypeTableAdapter.Fill(dbset.Relation_Restaurant_RestaurantType);
                Adapters.Relation_Restaurant_CourseTableAdapter.Fill(dbset.Relation_Restaurant_Course);
                Adapters.Relation_User_UserTableAdapter.Fill(dbset.Relation_User_User);
                Adapters.Relation_User_RestaurantTableAdapter.Fill(dbset.Relation_User_Restaurant);
                Adapters.RestaurantsTableAdapter.Fill(dbset.Restaurants);
                Adapters.RestaurantTypesTableAdapter.Fill(dbset.RestaurantTypes);
                Adapters.UsersTableAdapter.Fill(dbset.Users);
                Adapters.CommentsTableAdapter.Fill(dbset.Comments);

                #endregion

                RestaurantTypes.ItemsSource = dbset.RestaurantTypes;
                RList.ItemsSource = dbset.Restaurants;
                UserList.ItemsSource = dbset.Users;
                UserList_Target.ItemsSource = dbset.Users;
                CommentsList.ItemsSource = dbset.Comments;
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Error!", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private static delicacyDB dbset = new delicacyDB();
        public static delicousDBManager.delicacyDB Dbset
        {
            get { return dbset; }
        }
        private static delicacyDBTableAdapters.TableAdapterManager adapters = new delicousDBManager.delicacyDBTableAdapters.TableAdapterManager();

        private static delicacyDBTableAdapters.TableAdapterManager Adapters
        {
            get { return adapters; }
        }

        #region Tab Restaurants

        public void SetLocation(double latitude, double longitude)
        {
            if (RestaurantDetails.DataContext != null)
            {
                DataRowView v = (DataRowView)RestaurantDetails.DataContext;
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
            if (RestaurantDetails.DataContext != null)
            {
                try
                {
                    DataRowView v = (DataRowView)RestaurantDetails.DataContext;
                    delicacyDB.RestaurantsRow row = (delicacyDB.RestaurantsRow)v.Row;
                    Adapters.RestaurantsTableAdapter.Update(row);
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
                        Adapters.Relation_Restaurant_RestaurantTypeTableAdapter.Update(row0);
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

        #region Tab Courses
        #endregion

        #region Tab Users

        private MD5CryptoServiceProvider md5provider = new MD5CryptoServiceProvider();

        private string MD5(string password)
        {
            Byte[] originalBytes = ASCIIEncoding.Default.GetBytes(password);
            Byte[] encodedBytes = md5provider.ComputeHash(originalBytes);

            StringBuilder result = new StringBuilder();
            for (int i = 0; i < encodedBytes.Length; i++)
            {
                result.Append(encodedBytes[i].ToString("x2"));
            }
            return result.ToString();
        }

        private void User_NewEntry_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var row = dbset.Users.NewUsersRow();
                row.Nickname = string.Empty;
                row.Password = string.Empty;
                row.JoinTime = DateTime.Now;
                Dbset.Users.AddUsersRow(row);
                UserList.SelectedValue = row;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error!", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void SaveRelation_Click(object sender, RoutedEventArgs e)
        {
            delicacyDB.UsersRow usr = UserList.SelectedValue as delicacyDB.UsersRow;
            delicacyDB.UsersRow target = UserList_Target.SelectedValue as delicacyDB.UsersRow;
            if (usr != null && target != null && usr != target && UserRelations.SelectedItem != null)
            {
                var relationrow = dbset.Relation_User_User.FindByUID_HostUID_Target(usr.UID, target.UID);
                if (relationrow == null)
                {
                    relationrow = dbset.Relation_User_User.NewRelation_User_UserRow();
                    relationrow.UID_Host = usr.UID;
                    relationrow.UID_Target = target.UID;
                    relationrow.Relation = 0;
                    Dbset.Relation_User_User.AddRelation_User_UserRow(relationrow);
                }

                relationrow.Relation = (int)(UserRelationship)UserRelations.SelectedItem;
                Adapters.Relation_User_UserTableAdapter.Update(relationrow);
                var old = UserList.SelectedItem;
                UserList.SelectedItem = null;
                UserList.SelectedItem = old;
            }
        }

        private void User_Delete_Click(object sender, RoutedEventArgs e)
        {
            delicacyDB.UsersRow row = UserList.SelectedValue as delicacyDB.UsersRow;
            if (row != null)
            {
                row.Delete();
                Adapters.UsersTableAdapter.Update(row);
            }
        }

        private void User_Save_Click(object sender, RoutedEventArgs e)
        {
            delicacyDB.UsersRow row = null;
            if (UserDetails.DataContext != null)
            {
                try
                {
                    row = (delicacyDB.UsersRow)((DataRowView)UserDetails.DataContext).Row;
                    if (!row.HasVersion(DataRowVersion.Original) || row[dbset.Users.PasswordColumn, DataRowVersion.Current] != row[dbset.Users.PasswordColumn, DataRowVersion.Original])
                    {
                        row.Password = MD5(row.Password);
                    }
                    
                    Adapters.UsersTableAdapter.Update(dbset);
                }
                catch (System.Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error!", MessageBoxButton.OK, MessageBoxImage.Error);
                    if (row != null)
                    {
                        row.RejectChanges();
                    }
                }
            }
        }

        #endregion

        #region Tab Comments

        private void Comments_UserName_Hyperlink_Click(object sender, RoutedEventArgs e)
        {
            Hyperlink source = sender as Hyperlink;
            if (source != null)
            {
                var row = (source.DataContext as DataRowView).Row as delicacyDB.CommentsRow;
                if (row != null)
                {

                }
            }
        }

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
