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
                DataRowView view = (DataRowView)value;
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

    public class GetColorFromRelationBetween2Converter : IMultiValueConverter
    {
        #region IMultiValueConverter Members

        public object Convert(object[] values, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            var relation = new RelationBetween2Converter().Convert(values, targetType, parameter, culture);
            return new GetColorFromRelationConverter().Convert(relation, targetType, parameter, culture);
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, System.Globalization.CultureInfo culture)
        {
            return new object[0];
        }

        #endregion
    }

    [ValueConversion(typeof(UserRelationship), typeof(SolidColorBrush))]
    public class GetColorFromRelationConverter : IValueConverter
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
                        c = Colors.LightSlateGray;
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
}
