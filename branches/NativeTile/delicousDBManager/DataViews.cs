using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;

namespace delicousDBManager
{
    /// <summary>
    /// Base class for all ViewModel classes displayed by TreeViewItems.  
    /// This acts as an adapter between a raw data object and a TreeViewItem.
    /// </summary>
    public class TreeViewItemViewModel : INotifyPropertyChanged
    {
        #region Data

        static readonly TreeViewItemViewModel DummyChild = new TreeViewItemViewModel();

        readonly ObservableCollection<TreeViewItemViewModel> _children;
        readonly TreeViewItemViewModel _parent;

        bool _isExpanded;
        bool _isSelected;

        #endregion // Data

        #region Constructors

        protected TreeViewItemViewModel(TreeViewItemViewModel parent, bool lazyLoadChildren)
        {
            _parent = parent;

            _children = new ObservableCollection<TreeViewItemViewModel>();

            if (lazyLoadChildren)
                _children.Add(DummyChild);
        }

        // This is used to create the DummyChild instance.
        private TreeViewItemViewModel()
        {
        }

        #endregion // Constructors

        #region Presentation Members

        #region Sorter
        #endregion

        #region Sorted Children

        #endregion

        #region Children

        /// <summary>
        /// Returns the logical child items of this object.
        /// </summary>
        public ObservableCollection<TreeViewItemViewModel> Children
        {
            get { return _children; }
        }

        #endregion // Children

        #region HasLoadedChildren

        /// <summary>
        /// Returns true if this object's Children have not yet been populated.
        /// </summary>
        public bool HasDummyChild
        {
            get { return this.Children.Count == 1 && this.Children[0] == DummyChild; }
        }

        #endregion // HasLoadedChildren

        #region IsExpanded

        /// <summary>
        /// Gets/sets whether the TreeViewItem 
        /// associated with this object is expanded.
        /// </summary>
        public bool IsExpanded
        {
            get { return _isExpanded; }
            set
            {
                if (value != _isExpanded)
                {
                    _isExpanded = value;
                    this.OnPropertyChanged("IsExpanded");
                }

                // Expand all the way up to the root.
                if (_isExpanded && _parent != null)
                    _parent.IsExpanded = true;

                // Lazy load the child items, if necessary.
                if (this.HasDummyChild)
                {
                    this.Children.Remove(DummyChild);
                    this.LoadChildren();
                }
            }
        }

        #endregion // IsExpanded

        #region Name
        public string Name
        {
            get;
            set;
        }
        #endregion

        #region IsSelected

        /// <summary>
        /// Gets/sets whether the TreeViewItem 
        /// associated with this object is selected.
        /// </summary>
        public bool IsSelected
        {
            get { return _isSelected; }
            set
            {
                if (value != _isSelected)
                {
                    _isSelected = value;
                    this.OnPropertyChanged("IsSelected");
                }
            }
        }

        #endregion // IsSelected

        #region LoadChildren

        /// <summary>
        /// Invoked when the child items need to be loaded on demand.
        /// Subclasses can override this to populate the Children collection.
        /// </summary>
        protected virtual void LoadChildren()
        {
        }

        #endregion // LoadChildren

        #region Parent

        public TreeViewItemViewModel Parent
        {
            get { return _parent; }
        }

        #endregion // Parent

        #endregion // Presentation Members

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion // INotifyPropertyChanged Members
    }

    public class RestaurantTreeViewItem : TreeViewItemViewModel
    {
        public RestaurantTreeViewItem(delicacyDB.RestaurantsRow row)
            : base(null, true)
        {
            if (row == null)
                throw new ArgumentNullException("row");

            this.Restaurant = row;

            // This is not Elegant! Figure something else out! 
            MainWindow.Dbset.Restaurants.ColumnChanged += new System.Data.DataColumnChangeEventHandler(Restaurants_ColumnChanged);

            MainWindow.Dbset.Relation_Restaurant_Course.Relation_Restaurant_CourseRowDeleting += new delicacyDB.Relation_Restaurant_CourseRowChangeEventHandler(Relation_Restaurant_Course_Relation_Restaurant_CourseRowDeleted);
            MainWindow.Dbset.Relation_Restaurant_Course.Relation_Restaurant_CourseRowChanged += new delicacyDB.Relation_Restaurant_CourseRowChangeEventHandler(Relation_Restaurant_Course_Relation_Restaurant_CourseRowChanged);
        }

        void Restaurants_ColumnChanged(object sender, System.Data.DataColumnChangeEventArgs e)
        {
            // for now, we only show name in the tree, so let's filter out some events
            if (e.Row == Restaurant && e.Row.RowState != System.Data.DataRowState.Unchanged && e.Column == MainWindow.Dbset.Restaurants.NameColumn)
            {
                OnPropertyChanged("Restaurant");
            }
        }

        void Relation_Restaurant_Course_Relation_Restaurant_CourseRowChanged(object sender, delicacyDB.Relation_Restaurant_CourseRowChangeEvent e)
        {
            if (e.Action == System.Data.DataRowAction.Add && !HasDummyChild)
            {
                if (e.Row.RID == this.Restaurant.RID)
                {
                    Children.Add(new CourseTreeViewItem(this, e.Row.CoursesRow));
                }
            }
        }

        void Relation_Restaurant_Course_Relation_Restaurant_CourseRowDeleted(object sender, delicacyDB.Relation_Restaurant_CourseRowChangeEvent e)
        {
            if (!HasDummyChild)
            {
                if (e.Row.RID == this.Restaurant.RID)
                {
                    var view = Children.First(c => ((CourseTreeViewItem)c).Course.DID == e.Row.DID);
                    if (view != null)
                    {
                        Children.Remove(view);
                    }
                }
            }
        }

        public delicacyDB.RestaurantsRow Restaurant
        {
            get;
            set;
        }

        protected override void LoadChildren()
        {
            foreach (var r in Restaurant.GetRelation_Restaurant_CourseRows())
            {
                var c = r.CoursesRow;

                // This shall not be null given foreign key is set
                if (c != null)
                {
                    Children.Add(new CourseTreeViewItem(this, c));
                }
            }
        }
    }

    public class CourseTreeViewItem : TreeViewItemViewModel
    {
        public CourseTreeViewItem(RestaurantTreeViewItem parent, delicacyDB.CoursesRow course)
            : base(parent, false)
        {
            Course = course;
        }

        public delicacyDB.CoursesRow Course
        {
            get;
            set;
        }
    }
}
