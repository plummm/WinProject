using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AutoBackup
{
    class Backup
    {
        public void CopyFile(string sourcePath, string targetPath, string fileName)
        {
            // Use Path class to manipulate file and directory paths.
            string sourceFile = System.IO.Path.Combine(sourcePath, fileName);
            string destFile = System.IO.Path.Combine(targetPath, fileName);

            // To copy a folder's contents to a new location:
            // Create a new target folder, if necessary.
            if (!System.IO.Directory.Exists(sourceFile))
            {
                if (!System.IO.Directory.Exists(targetPath))
                {
                    System.IO.Directory.CreateDirectory(targetPath);
                }

                // To copy a file to another location and 
                // overwrite the destination file if it already exists.
                System.IO.File.Copy(sourceFile, destFile, true);
            }
            else
            { 

                // To copy all the files in one directory to another directory.
                // Get the files in the source folder. (To recursively iterate through
                // all subfolders under the current directory, see
                // "How to: Iterate Through a Directory Tree.")
                // Note: Check for target path was performed previously
                //       in this code example.
                if (System.IO.Directory.Exists(sourcePath))
                {
                    if (!System.IO.Directory.Exists(destFile))
                        System.IO.Directory.CreateDirectory(destFile);

                    string[] files = System.IO.Directory.GetFiles(sourceFile);
                    string[] directories = System.IO.Directory.GetDirectories(sourceFile);

                    // Copy the files and overwrite destination files if they already exist.
                    foreach (string s in files)
                    {
                        // Use static Path methods to extract only the file name from the path.
                        fileName = System.IO.Path.GetFileName(s);
                        string targetFile = System.IO.Path.Combine(destFile, fileName);
                        try
                        {
                            // Will not overwrite if the destination file already exists.
                            System.IO.File.Copy(s, targetFile, true);
                        }

                        // Catch exception if the file was already copied.
                        catch (System.IO.IOException copyError)
                        {
                            
                        }
                    }

                    foreach (string s in directories)
                    {
                        CopyFile(sourceFile, destFile, System.IO.Path.GetFileName(s));
                        
                    }
                }

            }
        }
    }
}
