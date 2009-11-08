using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Data;

namespace delicousDBManager
{
    public class GetTIDFromRestaurant : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value != null)
            {
                System.Data.DataRowView view = (System.Data.DataRowView)value;
                delicacyDB.RestaurantsRow restaurant = (delicacyDB.RestaurantsRow)view.Row;
                delicacyDB.Relation_Restaurant_RestaurantTypeRow[] rows = restaurant.GetRelation_Restaurant_RestaurantTypeRows();
                return rows.Length == 0 ? 0 : rows[0].TID;
            }
            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #endregion
    }

    // Get Relationship between 2 users
    public class RelationBetween2Converter : IMultiValueConverter
    {
        #region IMultiValueConverter Members

        public object Convert(object[] values, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (values == null || values.Length != 2 || values[0] == null || values[1] == null)
                return null;

            var source = (values[0] as DataRowView).Row as delicacyDB.UsersRow;
            var target = (values[1] as DataRowView).Row as delicacyDB.UsersRow;

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
            throw new NotImplementedException();
        }

        #endregion
    }


    // Get UserName From UID
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
}
