#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int tail_file (int fd, int max_lines)
{
  // We go to the end of the file, and we get how many bytes until the
  // end, and in this way we can get how bit is the file.
  off_t file_offset_end = lseek (fd, 0, SEEK_END);
  // We check for errors.
  if (file_offset_end == -1)
    {
      perror ("lseek");
      return 1;
    }

  // We rewind back the file, so we can read from it later.
  off_t file_offset_begin = lseek (fd, 0, SEEK_SET);
  // We check for errors.
  if (file_offset_begin == -1)
    {
      perror ("lseek");
      return 1;
    }

  size_t file_size = file_offset_end;
  // We make an array (also called a buffer), where we can store the
  // file contents.
  char   file_buffer[file_size];
  ssize_t bytes_read = read (fd, file_buffer, file_size);
  // Check for read errors.
  if (bytes_read == -1)
    {
      perror ("read");
      return 1;
    }

  // If the file is 0 bytes long (there's nothing in the file), we
  // don't print anything, but there's no error (it's ok).

  // If there's only one byte in the file, we just print it and that's
  // all.
  if (file_size == 1)
    {
      write (1, file_buffer, 1);
      return 0;
    }
  // If there are more than 1 bytes in the file.
  else if (file_size > 1)
    {
      // We set in index called "begin", which starts from the end of
      // the file (file_size - 1) and goes backwards, until max_lines (10) are counted
      // or we reach the beginning of the file.
      int begin = file_size - 1;

      // If the file ends with a new line, then we skip over it
      // (ignore it, don't count it).
      if (file_buffer[begin] == '\n') begin--;

      int lines_count = 0;
      // Loop until we go to the beginning of the file.
      while (begin > 0)
        {
          // If we see a new line, we increment our line counter.
          if (file_buffer[begin] == '\n')
            {
              lines_count++;
              // If we have the max lines we need (10), we stop, and
              // move our begin index 1 position forward, so we don't
              // print the first line.
              if (lines_count == max_lines)
                {
                  begin++;
                  break;
                }
            }

          begin--;
        }

      // We write everything from the "begin" index, until the end of
      // the file.
      size_t bytes_util_end = file_size - begin;
      write (1, &file_buffer[begin], bytes_util_end);
    }

  // We close the file.
  int close_error = close (fd);
  if (close_error == -1)
    {
      perror ("close");
      return 1;
    }

  return 0;
}


// We print all characters, unitl we reach a null terminaltor
// character.  We do this, because we aren't allowed to use <stdio.h>
// functions, like printf.
void print (const char *s)
{
  for (size_t i = 0; s[i] != '\0'; i++)
    {
      write (1, &s[i], 1);
    }
}

int main (int argc, char *argv[])
{
  // If only one filepath in terminal.
  if (argc == 2)
    {
      char *filepath = argv[1];
      int fd = open (filepath, O_RDONLY);
      if (fd == -1)
        {
          // Print error if file can't be opened, using our own function.
          print (argv[0]);
          print (": cannot open '");
          print (filepath);
          print ("' for reading: ");
          print (strerror (errno));
          print ("\n");
          // Stop the program with an error 1
          return 1;
        }

      // Print the last lines and return the error code from the function.
      return tail_file (fd, 10);
    }
  // If more than one filepath in terminal
  if (argc > 2)
    {
      // A the end of this code we'll return this variable, specifying
      // if the program finished without an error or not.
      int error_code = 0;

      // A FOR loop for all filepaths provided in the terminal.  We
      // start with arg_index=1, because the first argument in the
      // terminal is the name of the program.
      for (int arg_index = 1; arg_index < argc; ++arg_index)
        {
          // We open the file.
          char *filepath = argv[arg_index];
          int fd = open (filepath, O_RDONLY);
          if (fd == -1)
            {
              // Print error if file can't be opened, using our own function.
              print (argv[0]);
              print (": cannot open '");
              print (filepath);
              print ("' for reading: ");
              print (strerror (errno));
              print ("\n");
              // We set the error_code variable to 1 if we have an error.
              error_code = 1;
            }
          else
            {
              // If the file isn't the fist one, we print a new line
              // before we print the file.
              if (arg_index != 1) print ("\n");

              // If the file openned correctly, we print it's name and some symbols around it.
              print ("==> ");
              print (filepath);
              print (" <==\n");

              // We print the last 10 lines of the file.
              int file_error_code = tail_file (fd, 10);

              // If the function returned an error, we save this error
              // in our error_code variable.
              if (file_error_code) error_code = file_error_code;
            }
        }

      return error_code;
    }
  // If no arguments in terminal
  else
    {
      return 1;
    }

  return 0;
}
