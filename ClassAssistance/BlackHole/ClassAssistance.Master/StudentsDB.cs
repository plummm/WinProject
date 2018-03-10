using System;
using System.Collections.Generic;
using System.Data.SQLite;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace ClassAssistance.Master
{
    class StudentsDB
    {
        public static SQLiteConnection m_dbConnection;
        public StudentsDB()
        {
            CreateStudentsDB();
        }

        private void CreateStudentsDB()
        {
            if (File.Exists("Students.sqlite"))
            {
                OpenStudentsDB();
            }
            else
            {
                SQLiteConnection.CreateFile("Students.sqlite");
                OpenStudentsDB();
                Initialize();
                return;
            }
        }

        private void OpenStudentsDB()
        {
            m_dbConnection = new SQLiteConnection("Data Source=Students.sqlite;Version=3;");
            m_dbConnection.Open();
            
        }

        public static int QueryStuidAndPassword(string stuId, string password)
        {
            string sql = "SELECT EXISTS ( SELECT* FROM studentsInfo WHERE studentId = "
                + stuId + " AND password = '"
                + password + "')";
            SQLiteCommand command = new SQLiteCommand(sql, m_dbConnection);
            int result = Convert.ToInt32(command.ExecuteScalar());
            return result;
        }

        public void CloseStudentsDB()
        {
            m_dbConnection.Close();
        }

        private void Initialize()
        {
            string sql = "create table studentsInfo (studentId int, password varchar(20))";
            SQLiteCommand command = new SQLiteCommand(sql, m_dbConnection);
            command.ExecuteNonQuery();

            string stu1 = "insert into studentsInfo (studentId, password) values (2018060103010, 'abcdefg')";
            string stu2 = "insert into studentsInfo (studentId, password) values (2018060205021, 'qwerty123')";
            command = new SQLiteCommand(stu1, m_dbConnection);
            command.ExecuteNonQuery();
            command = new SQLiteCommand(stu2, m_dbConnection);
            command.ExecuteNonQuery();
        }
    }
}
