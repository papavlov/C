#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>


int
print_double (const char *filepath)
{
  int fd = open (filepath, O_RDONLY);
  if (fd == -1)
    {
      perror ("open");
      return 1;
    }

  while (1)
    {
      char c[2];
      ssize_t bytes_read = read (fd, &c, 1);

      if (bytes_read == -1)
        {
          perror ("read");
          break;
        }

      if (bytes_read == 0) break;

      c[1] = c[0];
      write (1, &c, 2);
    }

  close (fd);

  return 0;
}


int
main (int argc, char *argv[])
{
  if (argc == 2)
    {
      // Run this if there's only 1 file (usually a child process).
      print_double (argv[1]);
    }
  else if (argc > 2)
    {
      // Run this only in the mother process, when there's more than 1
      // file.

      // For each file:
      for (int i = 1; i < argc; ++i)
        {
          // This line makes a new process.  The new process is an
          // exact copy of the current process (the same code and
          // everything).
          pid_t child_pid = fork ();

          // We go into this IF, if we are the new process (the
          // child).  The main process never goes into this IF.
          if (child_pid == 0)
            {
              // Changes the process to a new program one and start
              // from the beginning.
              char *child_argv[] = {argv[0], argv[i], NULL};
              execv (argv[0], child_argv);

              // We shound't ever reach this code if there are no
              // problems, becase if everything is ok, the whole
              // programs is changed to a new one and started from the
              // beginning.
              perror ("execv");
              return 1;
            }

          // The code below runs only if we are the main process (not
          // the child).

          if (child_pid == -1)
            {
              perror ("fork");
             return 1;
            }

          // The main mother process will wait here for the child
          // process to finish:
          int child_status;
          pid_t wait_error = waitpid (child_pid, &child_status, 0);
          if (wait_error == -1)
            {
              perror ("wait");
              return 1;
            }
        }
    }

  return 0;
}
