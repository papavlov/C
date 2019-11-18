#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include <stdio.h>
#include <string.h>

#include <assert.h>


// File info stuff (ls, du, find)
//   opendir, readdir, closedir, stat
//
// Processes (watch, shell)
//   fork, execv, execvp, waitpid
//
// Threads (starcraft, warcraft)
//   pthread_create, pthread_join, pthread_mutex_init, pthread_mutex_lock, pthread_mutex_trylock
//
// File (head, tail, wc, cat, double)
//   open, close, read, write, lseek


size_t
get_disk_usage (const char *target_filepath)
{
  DIR *dir = opendir (target_filepath);
  if (!dir)
    {
      perror ("opendir");
      return 0;
    }

  size_t target_filepath_len = strlen (target_filepath); // Get the len of the filepath string so we can later make a full path to a file.
  unsigned long total_size = 0;  // Sum all files here.

  while (1)
    {
      struct dirent *entry = readdir (dir);  // Get each file from the dir.
      if (!entry) break;   // Loop until we get all files.
      char *filename = entry->d_name;  // A filename from the current dir.
      if (strcmp (filename, "..") == 0) continue;  // If the current file is the previous dir, ignore that.

      size_t filename_len = strlen (filename);
      char filepath[target_filepath_len + 1 + filename_len + 1];  // The full filepath of the current file.
      strcpy (filepath, target_filepath);
      strcat (filepath, "/");
      strcat (filepath, filename);

      struct stat file_stat;
      int stat_error = stat (filepath, &file_stat);  // Get info about the current file (using the full path of the file, not just it's name).
      if (stat_error)
        {
          perror ("stat");
          continue;
        }

      if (S_ISDIR (file_stat.st_mode))  // From the current file stat info, get it's type, and if it's a dir, do one thing, else do another.
        {
          if (strcmp (filename, ".")) // If the current file isn't the current dir, get recursively it's total size.
            {
              total_size += get_disk_usage (filepath);
            }
          else // If we are the current dir, just get the "file size" of the dir.
            {
              total_size += file_stat.st_size;
            }
        }
      else
        {
          // If it's file, get it's size.
          total_size += file_stat.st_size;
        }
    }

  closedir (dir);

  // Print the total size of the current dir.
  printf ("%lu\t%s\n", total_size, target_filepath);

  // Return the total size of the current dir so we can sum it with other dirs recursively.
  return total_size;
}


int
main (int argc, char *argv[])
{
  if (argc >= 2)
    {
      for (int i = 1; i < argc; ++i)
        {
          get_disk_usage (argv[i]);
        }
    }
  else
    {
      get_disk_usage (".");
    }

  return 0;
}
