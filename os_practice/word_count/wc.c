#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>



// int
// is_ascii_word_char (char c)
// {
//   return ((c >= 'A' && c <= 'Z') ||
//           (c >= 'a' && c <= 'z') ||
//           (c >= '0' && c <= '9') ||
//           c == '_');
// }


void
print (const char *s)
{
  for (; *s; ++s) write (1, s, 1);
}


int
is_ascii_space_char (char c)
{
  return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\f' || c == '\v');
}


void
print_size_t (size_t num)
{
  if (num == 0)
    {
      write (1, "0", 1);
      return;
    }

  // char buffer[64];
  // int  begin = sizeof (buffer);

  // while (num)
  //   {
  //     char digit = (num % 10) + '0';
  //     buffer[--begin] = digit;
  //     num /= 10;
  //   }

  // write (1, buffer + begin, sizeof (buffer) - begin);

  char buffer[64];
  int len = 0;
  while (num)
    {
      char digit = (num % 10) + '0';
      buffer[len++] = digit;
      num /= 10;
    }

  while (len)
    {
      --len;
      write (1, &buffer[len], 1);
    }
}


void
count_file_lines (int file, size_t *lines_count_r, size_t *words_count_r, size_t *file_size_r)
{
  // Use lseek to get file size.  The 0 is an offset from
  // the end of the file.
  long seek_offset;
  seek_offset = lseek (file, 0, SEEK_END);
  if (seek_offset == -1)
    {
      perror ("lseek");
      return;
    }

  size_t file_size = seek_offset;

  // We go back to the begining of the file, so when we do the read ()
  // function, it starts to read from there.  The 0 is an offset from
  // the beginning of the file.
  seek_offset = lseek (file, 0, SEEK_SET);
  if (seek_offset == -1)
    {
      perror ("lseek");
      return;
    }

  // Allocate memory for the file contents.
  char *buffer = (char *) malloc (file_size);
  if (!buffer)
    {
      perror ("malloc");
      return;
    }

  // Read the whole file.
  ssize_t bytes_read = read (file, buffer, file_size);
  if (bytes_read < 0)
    {
      perror ("read");
      free (buffer);
      return;
    }

  if ((size_t) bytes_read != file_size)
    {
      print ("Warning: We didn't read the whole file.\n");
    }

  size_t words_count = 0;
  size_t lines_count = 0;
  int in_word = 0;  // We use this var as a boolean.
  for (size_t i = 0; i < file_size; ++i)
    {
      char c = buffer[i];

      if (is_ascii_space_char (c))
        {
          // We come here if the character is a space.
          if (c == '\n') ++lines_count;
          // If the character is a space, that mean we aren't in a
          // word, so we set our in_word var to 0 (false):
          in_word = 0;
        }
      else
        {
          // We come here if the charater is a letter (or another
          // symbol).
          if (!in_word)
            {
              // If the previous charater wasn't a letter (or another
              // symbol), the in_word var will be 0 (false), which
              // also means that a new word just began:
              ++words_count;
              in_word = 1;
            }
        }
    }

  free (buffer);

  // We put data in the variables outside of the function (in main()),
  // using pointers:
  *lines_count_r = lines_count;
  *words_count_r = words_count;
  *file_size_r = file_size;
}


int
main (int argc, char *argv[])
{
  size_t total_lines_count = 0;
  size_t total_words_count = 0;
  size_t total_file_size = 0;

  for (int arg_index = 1; arg_index < argc; ++arg_index)
    {
      char *filepath = argv[arg_index];
      int file = open (filepath, O_RDONLY);
      if (file < 0)
        {
          print (argv[0]);
          print (": ");
          print (filepath);
          print (": ");
          print (strerror (errno));
          print ("\n");
        }
      else
        {
          size_t lines_count;
          size_t words_count;
          size_t file_size;

          count_file_lines (file, &lines_count, &words_count, &file_size);
          close (file);

          total_lines_count += lines_count;
          total_words_count += words_count;
          total_file_size   += file_size;

          print (" ");
          print_size_t (lines_count);
          print ("  ");
          print_size_t (words_count);
          print (" ");
          print_size_t (file_size);
          print (" ");
          print (filepath);
          print ("\n");
        }
    }

  if (argc > 2)
    {
      print (" ");
      print_size_t (total_lines_count);
      print ("  ");
      print_size_t (total_words_count);
      print (" ");
      print_size_t (total_file_size);
      print (" ");
      print ("total");
      print ("\n");
    }

  return 0;
}
