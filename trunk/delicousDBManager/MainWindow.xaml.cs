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
using System.Collections.ObjectModel;

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

                #region DataBase Initialization

                Adapters.Relation_Restaurant_RestaurantTypeTableAdapter = new delicousDBManager.delicacyDBTableAdapters.Relation_Restaurant_RestaurantTypeTableAdapter();
                Adapters.Relation_User_RestaurantTableAdapter = new delicousDBManager.delicacyDBTableAdapters.Relation_User_RestaurantTableAdapter();
                Adapters.Relation_User_UserTableAdapter = new delicousDBManager.delicacyDBTableAdapters.Relation_User_UserTableAdapter();
                Adapters.Relation_Restaurant_CourseTableAdapter = new delicousDBManager.delicacyDBTableAdapters.Relation_Restaurant_CourseTableAdapter();
                Adapters.Relation_Restaurant_CourseTableAdapter = new delicousDBManager.delicacyDBTableAdapters.Relation_Restaurant_CourseTableAdapter();
                Adapters.RestaurantsTableAdapter = new delicousDBManager.delicacyDBTableAdapters.RestaurantsTableAdapter();
                Adapters.RestaurantTypesTableAdapter = new delicousDBManager.delicacyDBTableAdapters.RestaurantTypesTableAdapter();
                Adapters.UsersTableAdapter = new delicousDBManager.delicacyDBTableAdapters.UsersTableAdapter();
                Adapters.CommentsTableAdapter = new delicousDBManager.delicacyDBTableAdapters.CommentsTableAdapter();
                Adapters.CoursesTableAdapter = new delicousDBManager.delicacyDBTableAdapters.CoursesTableAdapter();

                FillTables();


                #endregion

                RestaurantTypes.ItemsSource = Dbset.RestaurantTypes;
                RList.ItemsSource = Dbset.Restaurants;
                UserList.ItemsSource = Dbset.Users;
                UserList_Target.ItemsSource = Dbset.Users;
                User_RestaurantList.ItemsSource = dbset.Restaurants;
                CommentsList.ItemsSource = Dbset.Comments;
                CourseList.ItemsSource = Dbset.Courses;
                RestaurantCourseTree.ItemsSource = new ObservableCollection<RestaurantTreeViewItem>(
                    from c in Dbset.Restaurants
                    select new RestaurantTreeViewItem(c));
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Error!", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void FillTables()
        {
            Adapters.Relation_Restaurant_RestaurantTypeTableAdapter.Fill(Dbset.Relation_Restaurant_RestaurantType);
            Adapters.Relation_Restaurant_CourseTableAdapter.Fill(Dbset.Relation_Restaurant_Course);
            Adapters.Relation_User_UserTableAdapter.Fill(Dbset.Relation_User_User);
            Adapters.Relation_User_RestaurantTableAdapter.Fill(Dbset.Relation_User_Restaurant);
            Adapters.Relation_Restaurant_CourseTableAdapter.Fill(Dbset.Relation_Restaurant_Course);
            Adapters.RestaurantsTableAdapter.Fill(Dbset.Restaurants);
            Adapters.RestaurantTypesTableAdapter.Fill(Dbset.RestaurantTypes);
            Adapters.UsersTableAdapter.Fill(Dbset.Users);
            Adapters.CommentsTableAdapter.Fill(Dbset.Comments);
            Adapters.CoursesTableAdapter.Fill(Dbset.Courses);
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

        private void RevertChange(DataRowView view)
        {
            if (view != null && view.Row.RowState == DataRowState.Modified)
            {
                view.Row.RejectChanges();

                // Workaround to update List Content
                foreach (DataColumn c in view.Row.Table.Columns)
                {
                    view.Row[c] = view.Row[c];
                }
                view.Row.RejectChanges();
            }
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

        private void Restaurant_DeleteEntry_Click(object sender, RoutedEventArgs e)
        {
            if (RestaurantDetails.DataContext != null)
            {
                try
                {
                    DataRowView v = (DataRowView)RestaurantDetails.DataContext;
                    delicacyDB.RestaurantsRow row = (delicacyDB.RestaurantsRow)v.Row;
                    row.Delete();
                    Adapters.RestaurantsTableAdapter.Update(row);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error!", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
        }

        private void Restaurant_Save_Click(object sender, RoutedEventArgs e)
        {
            if (RestaurantDetails.DataContext != null)
            {
                try
                {
                    // Update Restaurant info
                    DataRowView v = (DataRowView)RestaurantDetails.DataContext;
                    delicacyDB.RestaurantsRow row = (delicacyDB.RestaurantsRow)v.Row;
                    Adapters.RestaurantsTableAdapter.Update(row);

                    // Update Restaurant type
                    if (RestaurantTypes.SelectedItem != null || !string.IsNullOrEmpty(RestaurantTypes.Text))
                    {
                        delicacyDB.RestaurantTypesRow typerow = null;

                        // New type is used. insert it into RestaurantTypes table
                        if (RestaurantTypes.SelectedItem == null)
                        {
                            typerow = Dbset.RestaurantTypes.AddRestaurantTypesRow(RestaurantTypes.Text);
                            Adapters.RestaurantTypesTableAdapter.Update(typerow);
                        }
                        else
                            typerow = (delicacyDB.RestaurantTypesRow)((DataRowView)RestaurantTypes.SelectedItem).Row;

                        var RtypeRows = row.GetRelation_Restaurant_RestaurantTypeRows();
                        delicacyDB.Relation_Restaurant_RestaurantTypeRow row0 = null;
                        if (RtypeRows.Length != 0)
                        {
                            row0 = RtypeRows[0];
                            row0.RID = row.RID;
                            row0.RestaurantTypesRow = typerow;
                        }
                        else
                        {
                            row0 = dbset.Relation_Restaurant_RestaurantType.AddRelation_Restaurant_RestaurantTypeRow(row, typerow);
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

        private Binding backupRTypeBinding = null;

        private void RList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (RList.SelectedItem != null)
            {
                // This is a known limitation of OneWay binding. the binding is automatically removed after an user
                // manually specified a value. To work around this, we have to use TwoWay binding or save the binding 
                // and restore it later

                var oldbinding = BindingOperations.GetBinding(RestaurantTypes, ComboBox.SelectedValueProperty);
                if (oldbinding != null)
                {
                    backupRTypeBinding = oldbinding;
                }
                else if (backupRTypeBinding != null)
                {
                    RestaurantTypes.SetBinding(ComboBox.SelectedValueProperty, backupRTypeBinding);
                }
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

        private void RestaurantDetails_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            RevertChange(e.OldValue as DataRowView);
        }

        #endregion

        #region Tab Courses

        private void Course_AddToRestaurant_Click(object sender, RoutedEventArgs e)
        {
            if (CourseList.SelectedItem != null && RestaurantCourseTree.SelectedItem != null)
            {
                delicacyDB.RestaurantsRow restaurant = null;
                if (RestaurantCourseTree.SelectedItem is RestaurantTreeViewItem)
                {
                    restaurant = ((RestaurantTreeViewItem)RestaurantCourseTree.SelectedItem).Restaurant;
                }
                else if (RestaurantCourseTree.SelectedItem is CourseTreeViewItem)
                {
                    restaurant = ((RestaurantTreeViewItem)((CourseTreeViewItem)RestaurantCourseTree.SelectedItem).Parent).Restaurant;
                }

                if (restaurant != null)
                {
                    delicacyDB.CoursesRow c = (delicacyDB.CoursesRow)CourseList.SelectedValue;
                    var relationrow = Dbset.Relation_Restaurant_Course.NewRelation_Restaurant_CourseRow();
                    relationrow.CoursesRow = c;
                    relationrow.RestaurantsRow = restaurant;
                    Dbset.Relation_Restaurant_Course.AddRelation_Restaurant_CourseRow(relationrow);
                    Adapters.Relation_Restaurant_CourseTableAdapter.Update(relationrow);
                }
            }
        }

        private void Course_RemoveFromRestaurant_Click(object sender, RoutedEventArgs e)
        {
            if (RestaurantCourseTree.SelectedItem != null && RestaurantCourseTree.SelectedItem is CourseTreeViewItem)
            {
                CourseTreeViewItem courseview = (CourseTreeViewItem)RestaurantCourseTree.SelectedItem;
                var restaurant = ((RestaurantTreeViewItem)courseview.Parent).Restaurant;
                var course = courseview.Course;

                if (restaurant != null && course != null)
                {
                    var row = Dbset.Relation_Restaurant_Course.FindByRIDDID(restaurant.RID, course.DID);
                    if (row != null)
                    {
                        row.Delete();
                        Adapters.Relation_Restaurant_CourseTableAdapter.Update(row);
                    }
                }
            }
        }

        private void Course_Add_Click(object sender, RoutedEventArgs e)
        {
            var crow = Dbset.Courses.NewCoursesRow();
            crow.Name = string.Empty;
            Dbset.Courses.AddCoursesRow(crow);
            CourseList.SelectedValue = crow;
        }

        private void Course_Save_Click(object sender, RoutedEventArgs e)
        {
            DataRowView v = CourseDetail.DataContext as DataRowView;
            if (v != null)
            {
                var crow = (delicacyDB.CoursesRow)v.Row;
                Adapters.CoursesTableAdapter.Update(crow);
            }
        }

        private void Course_Delete_Click(object sender, RoutedEventArgs e)
        {
            DataRowView v = CourseDetail.DataContext as DataRowView;
            if (v != null)
            {
                v.Row.Delete();
                Adapters.CoursesTableAdapter.Update(v.Row);
            }
        }

        private void CourseDetail_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            DataRowView view = e.OldValue as DataRowView;
            RevertChange(view);
        }

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

        private void RemoveRelation_Click(object sender, RoutedEventArgs e)
        {
            delicacyDB.UsersRow usr = UserList.SelectedValue as delicacyDB.UsersRow;
            delicacyDB.UsersRow target = UserList_Target.SelectedValue as delicacyDB.UsersRow;
            if (usr != null && target != null && usr != target)
            {
                var relationrow = dbset.Relation_User_User.FindByUID_HostUID_Target(usr.UID, target.UID);
                if (relationrow != null)
                {
                    relationrow.Delete();
                    Adapters.Relation_User_UserTableAdapter.Update(relationrow);

                    var old = UserList.SelectedItem;
                    UserList.SelectedItem = null;
                    UserList.SelectedItem = old;
                }
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

                    Adapters.UsersTableAdapter.Update(row);
                }
                catch (System.Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error!", MessageBoxButton.OK, MessageBoxImage.Error);
                    if (row != null)
                    {
                        RevertChange(UserDetails.DataContext as DataRowView);
                    }
                }
            }
        }

        private void UserDetails_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            RevertChange(e.OldValue as DataRowView);
        }

        private void SetUserRestaurantRelation(object sender, RoutedEventArgs e)
        {
            var userrow = UserList.SelectedValue as delicacyDB.UsersRow;
            var restaurantrow = User_RestaurantList.SelectedValue as delicacyDB.RestaurantsRow;

            if (userrow != null && restaurantrow != null && UserRestaurantRelations.SelectedItem != null)
            {
                var relationrow = Dbset.Relation_User_Restaurant.FindByUIDRID(userrow.UID, restaurantrow.RID);
                if (relationrow == null)
                {
                    relationrow = Dbset.Relation_User_Restaurant.NewRelation_User_RestaurantRow();
                    relationrow.RestaurantsRow = restaurantrow;
                    relationrow.UsersRow = userrow;
                    relationrow.Relation = 0;
                    Dbset.Relation_User_Restaurant.AddRelation_User_RestaurantRow(relationrow);
                }
                relationrow.Relation = (int)(UserRelationship)UserRestaurantRelations.SelectedItem;
                Adapters.Relation_User_RestaurantTableAdapter.Update(relationrow);

                // workaround
                var old = UserList.SelectedItem;
                UserList.SelectedItem = null;
                UserList.SelectedItem = old;
            }
        }

        private void RemoveUserRestaurantRelation(object sender, RoutedEventArgs e)
        {
            var userrow = UserList.SelectedValue as delicacyDB.UsersRow;
            var restaurantrow = User_RestaurantList.SelectedValue as delicacyDB.RestaurantsRow;

            if (userrow != null && restaurantrow != null)
            {
                var relationrow = Dbset.Relation_User_Restaurant.FindByUIDRID(userrow.UID, restaurantrow.RID);
                if (relationrow != null)
                {
                    relationrow.Delete();
                    Adapters.Relation_User_RestaurantTableAdapter.Update(relationrow);

                    // workaround
                    var old = UserList.SelectedItem;
                    UserList.SelectedItem = null;
                    UserList.SelectedItem = old;
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
                    UserList.SelectedValue = Dbset.Users.FindByUID(row.UID);
                    Tabs.SelectedItem = TabUser;
                }
            }
        }

        private void Comments_Restaurant_Hyperlink_Click(object sender, RoutedEventArgs e)
        {
            Hyperlink source = sender as Hyperlink;
            if (source != null)
            {
                var row = (source.DataContext as DataRowView).Row as delicacyDB.CommentsRow;
                if (row != null)
                {
                    RList.SelectedValue = Dbset.Restaurants.FindByRID(row.RID);
                    Tabs.SelectedItem = TabRestaurant;
                }
            }
        }

        private void Comments_Course_Hyperlink_Click(object sender, RoutedEventArgs e)
        {
            Hyperlink source = sender as Hyperlink;
            if (source != null)
            {
                var row = (source.DataContext as DataRowView).Row as delicacyDB.CommentsRow;
                if (row != null)
                {
                    CourseList.SelectedValue = Dbset.Courses.FindByDID(row.DID);
                    Tabs.SelectedItem = TabCourse;
                }
            }
        }

        private void CommentDetails_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            RevertChange(e.OldValue as DataRowView);
        }

        private void Comment_New_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var row = Dbset.Comments.NewCommentsRow();
                row.UID = row.RID = row.DID = 0;
                row.Comment = string.Empty;
                row.AddTime = DateTime.Now;
                row.TimeZone = 8;
                Dbset.Comments.AddCommentsRow(row);
                CommentsList.SelectedValue = row;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error!", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void Comment_Delete_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                delicacyDB.CommentsRow row = CommentsList.SelectedValue as delicacyDB.CommentsRow;
                if (row != null)
                {
                    row.Delete();
                    Adapters.CommentsTableAdapter.Update(row);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error!", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void Comment_Preview_Click(object sender, RoutedEventArgs e)
        {
            if (CommentDetail.DataContext != null)
            {
                var previewwindow = new CommentPreview(CommentDetail.DataContext as delicacyDB.CommentsRow);
                previewwindow.Show();
            }
        }

        private void Comment_Save_Click(object sender, RoutedEventArgs e)
        {
            delicacyDB.CommentsRow row = null;
            if (CommentDetail.DataContext != null)
            {
                try
                {
                    row = (delicacyDB.CommentsRow)((DataRowView)CommentDetail.DataContext).Row;
                    Adapters.CommentsTableAdapter.Update(row);
                }
                catch (System.Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error!", MessageBoxButton.OK, MessageBoxImage.Error);
                    if (row != null)
                    {
                        RevertChange(CommentDetail.DataContext as DataRowView);
                    }
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
