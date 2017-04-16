using System;
using System.Linq;
using Telerik.Windows.Controls;
using System.Collections.Generic;

namespace AutoBackup.ViewModels
{
public class DropIndicationDetails : ViewModelBase
{
    private List<ProductViewModel> currentDraggedItem;
    private DropPosition currentDropPosition;
    private object currentDraggedOverItem;

        public object CurrentDraggedOverItem
    {

        get
        {
            return currentDraggedOverItem;
        }
        set
        {
            if (this.currentDraggedOverItem != value)
            {
                currentDraggedOverItem = value;
                OnPropertyChanged("CurrentDraggedOverItem");
            }
        }
    }

    public int DropIndex { get; set; }

    public DropPosition CurrentDropPosition
    {
        get
        {
            return this.currentDropPosition;
        }
        set
        {
            if (this.currentDropPosition != value)
            {
                this.currentDropPosition = value;
                OnPropertyChanged("CurrentDropPosition");
            }
        }
    }

    public List<ProductViewModel> CurrentDraggedItem
    {
        get
        {
            return this.currentDraggedItem;
        }
        set
        {
            if (this.currentDraggedItem != value)
            {
                this.currentDraggedItem = value;
                OnPropertyChanged("CurrentDraggedItem");
            }
        }
    }
}
}
