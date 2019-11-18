#include <unistd.h>
#include <sys/wait.h>

#include <assert.h>
#include <stdio.h>



int
main (int argc, char *argv[])
{
  if (argc < 2) return -1;

  char *process = argv[1];

  while (1)
    {
      // The parent process makes a child process here
      pid_t pid = fork ();
      assert (pid != -1);

      if (pid == 0)
        {
          // This code is run by the child process (not by the parent)

          // Copy all the parent arguments, except the first one.
          char *exec_args[argc];
          for (int i = 1; i < argc; ++i)
            {
              exec_args[i - 1] = argv[i];
            }
          exec_args[argc - 1] = NULL;

          // We use execvp instead of execv, because execvp will look
          // for the program in the system directories.
          int exec_error = execvp (process, exec_args);
          if (exec_error == -1)
            {
              perror (process);
              return -1;
            }
        }

      // The parent process waits for the child process to finish here
      int child_error;
      int child_pid = waitpid (pid, &child_error, 0);
      assert (child_pid != -1);
      if (child_error)
        {
          return child_error;
        }

      sleep (2);
    }

  return 0;
}
