using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;

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


    public class RestaurantTypeConverter : IValueConverter
    {
        private static Dictionary<long, string> dict = new Dictionary<long, string>();
        static RestaurantTypeConverter()
        {
            delicacyDBTableAdapters.RestaurantTypesTableAdapter data = new delicousDBManager.delicacyDBTableAdapters.RestaurantTypesTableAdapter();
            delicacyDB.RestaurantTypesDataTable table = new delicacyDB.RestaurantTypesDataTable();
            data.Fill(table);
            var r = from c in table
                    select new { K = c.TID, V = c.ReadableText };
            foreach (var item in r)
            {
                dict.Add(item.K, item.V);
            }
        }
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (targetType != typeof(string))
                throw new InvalidCastException();

            long id = (long)value;
            return dict[id];
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (targetType != typeof(long))
                throw new InvalidCastException();

            string text = (string)value;
            return dict.First(c => c.Value.Equals(text, StringComparison.CurrentCultureIgnoreCase)).Key;
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
