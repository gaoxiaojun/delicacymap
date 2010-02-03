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
using System.Windows.Shapes;
using System.IO;

namespace delicousDBManager
{
    /// <summary>
    /// Interaction logic for CommentPreview.xaml
    /// </summary>
    public partial class CommentPreview : Window
    {
        public CommentPreview(delicacyDB.CommentsRow comment)
        {
            InitializeComponent();
            this.CommentRow = comment;

            string tempfilename = System.IO.Path.GetTempFileName() + ".htm";
            using (FileStream file = new FileStream(tempfilename, FileMode.Create, FileAccess.Write))
                using (StreamWriter writer = new StreamWriter(file))
                {
                    writer.Write( "<html>" +
                                    "<body>");
                    if (!comment.IsPhotoPathNull() && !string.IsNullOrEmpty(comment.PhotoPath))
                    {
                        writer.Write("<img src='" + comment.PhotoPath + "'/>");
                    }
                    writer.Write("<p>");
                    writer.Write(comment.Comment);
                    writer.Write("</p></body></html>");
                }

            browser.Source = new Uri(tempfilename, UriKind.Absolute);
        }

        private delicacyDB.CommentsRow CommentRow;
    }
}
