using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Data;
using System.Windows.Media;

namespace delicousDBManager
{
    [ValueConversion(typeof(DataRowView), typeof(long))]
    public class GetTIDFromRestaurant : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value != null)
            {
                delicacyDB.RestaurantsRow restaurant = (delicacyDB.RestaurantsRow)value;
                delicacyDB.Relation_Restaurant_RestaurantTypeRow[] rows = restaurant.GetRelation_Restaurant_RestaurantTypeRows();
                return rows.Length == 0 ? 0 : rows[0].TID;
            }
            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return Binding.DoNothing;
        }

        #endregion
    }

    // Get Relationship between 2 users
    [ValueConversion(typeof(DataRow), typeof(UserRelationship))]
    public class RelationBetween2Converter : IMultiValueConverter
    {
        #region IMultiValueConverter Members

        public object Convert(object[] values, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (values == null || values.Length != 2 || values[0] == null || values[1] == null)
                return null;

            var source = values[0] as delicacyDB.UsersRow;
            var target = values[1] as delicacyDB.UsersRow;

            if (source != target)
            {
                var row = MainWindow.Dbset.Relation_User_User.FindByUID_HostUID_Target(source.UID, target.UID);
                if (row != null)
                {
                    int relation = row.Relation;
                    return (UserRelationship)relation;
                }
            }
            return null;
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, System.Globalization.CultureInfo culture)
        {
            return new object[0];
        }

        #endregion
    }

    // Get Relationship between 2 users
    [ValueConversion(typeof(DataRow), typeof(UserRelationship))]
    public class RelationBetweenUserRestaurantConverter : IMultiValueConverter
    {
        #region IMultiValueConverter Members

        public object Convert(object[] values, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (values == null || values.Length != 2 || values[0] == null || values[1] == null)
                return null;

            var source = values[0] as delicacyDB.UsersRow;
            var target = values[1] as delicacyDB.RestaurantsRow;

            var row = MainWindow.Dbset.Relation_User_Restaurant.FindByUIDRID(source.UID, target.RID);
            if (row != null)
            {
                int relation = row.Relation;
                return (UserRestaurantRelationship)relation;
            }
            return null;
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, System.Globalization.CultureInfo culture)
        {
            return new object[0];
        }

        #endregion
    }

    public class GetColorFromUserRelationBetween2Converter : IMultiValueConverter
    {
        #region IMultiValueConverter Members

        public object Convert(object[] values, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            var relation = new RelationBetween2Converter().Convert(values, targetType, parameter, culture);
            return new GetColorFromUserRelationConverter().Convert(relation, targetType, parameter, culture);
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, System.Globalization.CultureInfo culture)
        {
            return new object[0];
        }

        #endregion
    }

    public class GetColorFromUserRestaurantRelationBetween2Converter : IMultiValueConverter
    {
        #region IMultiValueConverter Members

        public object Convert(object[] values, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            var relation = new RelationBetweenUserRestaurantConverter().Convert(values, targetType, parameter, culture);
            return new GetColorFromUserRestaurantRelationConverter().Convert(relation, targetType, parameter, culture);
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, System.Globalization.CultureInfo culture)
        {
            return new object[0];
        }

        #endregion
    }

    [ValueConversion(typeof(UserRelationship), typeof(SolidColorBrush))]
    public class GetColorFromUserRelationConverter : IValueConverter
    {

        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            Color c = Colors.Black;

            if (value is UserRelationship)
            {
                UserRelationship relation = (UserRelationship)value;
                switch (relation)
                {
                    case UserRelationship.Friend:
                        c = Colors.LightGreen;
                        break;
                    case UserRelationship.BlackList:
                        c = Colors.DarkGray;
                        break;
                }
            }
            return new SolidColorBrush(c);
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #endregion
    }

    [ValueConversion(typeof(UserRestaurantRelationship), typeof(SolidColorBrush))]
    public class GetColorFromUserRestaurantRelationConverter : IValueConverter
    {

        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            Color c = Colors.Black;

            if (value is UserRestaurantRelationship)
            {
                UserRestaurantRelationship relation = (UserRestaurantRelationship)value;
                switch (relation)
                {
                    case UserRestaurantRelationship.Favorite:
                        c = Colors.DarkSalmon;
                        break;
                }
            }
            return new SolidColorBrush(c);
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #endregion
    }

    // Get UserName From UID
    [ValueConversion(typeof(long), typeof(string))]
    public class GetNameFromUIDConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (targetType != typeof(string))
                throw new InvalidCastException();

            long id = (long)value;
            return MainWindow.Dbset.Users.FindByUID(id).Nickname;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #endregion
    }

    // Get Restaurant Name From RID
    [ValueConversion(typeof(long), typeof(string))]
    public class GetNameFromRIDConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (targetType != typeof(string))
                throw new InvalidCastException();

            long id = (long)value;
            return MainWindow.Dbset.Restaurants.FindByRID(id).Name;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #endregion
    }

    // Get Course Name From DID
    [ValueConversion(typeof(long), typeof(string))]
    public class GetNameFromDIDConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (targetType != typeof(string))
                throw new InvalidCastException();

            delicacyDB.CoursesRow row = value is DBNull ? null : MainWindow.Dbset.Courses.FindByDID((long)value);

            return row == null ? string.Empty : row.Name;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #endregion
    }

    [ValueConversion(typeof(string), typeof(string))]
    public class RestaurantTypeConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (targetType != typeof(string))
                throw new InvalidCastException();

            long id = (long)value;
            return MainWindow.Dbset.RestaurantTypes.FindByTID(id).ReadableText;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (targetType != typeof(long))
                throw new InvalidCastException();

            string text = (string)value;
            return MainWindow.Dbset.RestaurantTypes.First(c => c.ReadableText == text).TID;
        }

        #endregion
    }

    public class ValueSubtract : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            double newlvalue = (double)value;
            double subtract = double.Parse((string)parameter);
            return newlvalue - subtract;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #endregion
    }

    public class ValueMatchConverter : IMultiValueConverter
    {
        #region IMultiValueConverter Members

        public object Convert(object[] values, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (values != null && values.Length == 2 && values[0] != null && values[1] != null)
            {
                if (values[0] == values[1])
                {
                    return parameter;
                }
            }
            return null;
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #endregion
    }


}
