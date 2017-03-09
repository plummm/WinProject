using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Office.Interop.Word;

namespace sendToPrinter
{
    public class DocPrint
    {
        
        public void Senddoc(string nDoc)
        {
            Microsoft.Office.Interop.Word.Application word = new Microsoft.Office.Interop.Word.Application { Visible = false };
            Microsoft.Office.Interop.Word.Document doc;
            object readOnly = false;
            object isVisible = false;
            string fileName= nDoc;
            word.Visible = false;
            word.DisplayAlerts = Microsoft.Office.Interop.Word.WdAlertLevel.wdAlertsNone;
            doc = word.Documents.Open(fileName, ReadOnly: readOnly, Visible: isVisible);
            doc.Activate();
            doc.PrintOut();
            doc.Close();
        }
       
    }
}
