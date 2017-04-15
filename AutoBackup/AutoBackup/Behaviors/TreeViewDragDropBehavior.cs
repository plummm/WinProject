using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Telerik.Windows.DragDrop;
using System.IO;

namespace AutoBackup.Behavior
{
    class TreeViewDragDropBehavior
    {
        private TreeView _associatedObject;
        /// <summary>
        /// AssociatedObject Property
        /// </summary>
        public TreeView AssociatedObject
        {
            get
            {
                return _associatedObject;
            }
            set
            {
                _associatedObject = value;
            }
        }

        private static Dictionary<TreeView, TreeViewDragDropBehavior> instances;

        static TreeViewDragDropBehavior()
        {
            instances = new Dictionary<TreeView, TreeViewDragDropBehavior>();
        }

        public static bool GetIsEnabled(DependencyObject obj)
        {
            return (bool)obj.GetValue(IsEnabledProperty);
        }

        public static void SetIsEnabled(DependencyObject obj, bool value)
        {
            TreeViewDragDropBehavior behavior = GetAttachedBehavior(obj as TreeView);

            behavior.AssociatedObject = obj as TreeView;

            if (value)
            {
                behavior.Initialize();
            }
            else
            {
                behavior.CleanUp();
            }
            obj.SetValue(IsEnabledProperty, value);
        }

        // Using a DependencyProperty as the backing store for IsEnabled.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsEnabledProperty =
            DependencyProperty.RegisterAttached("IsEnabled", typeof(bool), typeof(TreeViewDragDropBehavior),
                new PropertyMetadata(new PropertyChangedCallback(OnIsEnabledPropertyChanged)));

        private static int lastAdded = 0;

        public static void OnIsEnabledPropertyChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs e)
        {
            SetIsEnabled(dependencyObject, (bool)e.NewValue);
        }

        private static TreeViewDragDropBehavior GetAttachedBehavior(TreeView treeView)
        {
            if (!instances.ContainsKey(treeView))
            {
                instances[treeView] = new TreeViewDragDropBehavior();
                instances[treeView].AssociatedObject = treeView;
            }

            return instances[treeView];
        }

        protected virtual void Initialize()
        {
            this.UnsubscribeFromDragDropEvents();
            this.SubscribeToDragDropEvents();
        }

        protected virtual void CleanUp()
        {
            this.UnsubscribeFromDragDropEvents();
        }

        private void SubscribeToDragDropEvents()
        {
            DragDropManager.AddDragInitializeHandler(this.AssociatedObject, OnDragInitialize);
            DragDropManager.AddGiveFeedbackHandler(this.AssociatedObject, OnGiveFeedback);
            DragDropManager.AddDropHandler(this.AssociatedObject, OnDrop);
            DragDropManager.AddDragDropCompletedHandler(this.AssociatedObject, OnDragDropCompleted);
        }

        private void UnsubscribeFromDragDropEvents()
        {
            DragDropManager.RemoveDragInitializeHandler(this.AssociatedObject, OnDragInitialize);
            DragDropManager.RemoveGiveFeedbackHandler(this.AssociatedObject, OnGiveFeedback);
            DragDropManager.RemoveDropHandler(this.AssociatedObject, OnDrop);
            DragDropManager.RemoveDragDropCompletedHandler(this.AssociatedObject, OnDragDropCompleted);

        }

        private void OnDragInitialize(object sender, DragInitializeEventArgs e)
        {
            //var draggedItem = (sender as TreeView).SelectedItem as MyObject;
            var draggedItem = (sender as TreeView).SelectedItem as DirectoryRecord;
            var directoryItem = draggedItem.Info as DirectoryInfo;

            e.AllowedEffects = DragDropEffects.All;
            var data = DragDropPayloadManager.GeneratePayload(null);
            string text = "";
            string path = "";

            if (draggedItem != null)
            {
                path = directoryItem.FullName;
                text = directoryItem.Name;
            }

            data.SetData("Text", text);
            data.SetData("DraggedData", directoryItem);
            data.SetData("Path", path);

            e.DragVisual = new DragVisual()
            {
                Content = text,
            };
            e.DragVisualOffset = e.RelativeStartPoint;
            e.Data = data;
        }

        private void OnGiveFeedback(object sender, Telerik.Windows.DragDrop.GiveFeedbackEventArgs e)
        {
            e.SetCursor(Cursors.Arrow);
            e.Handled = true;
        }

        private void OnDragDropCompleted(object sender, DragDropCompletedEventArgs e)
        {
            var draggedItem = DragDropPayloadManager.GetDataFromObject(e.Data, "DraggedData");

            if (e.Effects != DragDropEffects.None)
            {
                var collection = (sender as TreeView).ItemsSource as IList;
                //collection.Remove(draggedItem);
            }
        }

        private void OnDrop(object sender, Telerik.Windows.DragDrop.DragEventArgs e)
        {
            var text = DragDropPayloadManager.GetDataFromObject(e.Data, "Text") as string;
            var path = DragDropPayloadManager.GetDataFromObject(e.Data, "Path") as string;
            var listBox = sender as ListBox;

            if (text != null && listBox != null)
            {
                (listBox.ItemsSource as IList).Add(new MyObject() { ID = TreeViewDragDropBehavior.lastAdded++, Name = text ,Path = path});
                var back = new Backup();
                //back.CopyFile(text, sendDirectory.FullName, "");
                
            }
        }
    }
}
