#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <stdio.h>


void write_num (unsigned long long num)
{
  if (num)
    {
      char s[64];
      size_t i = 64;
      while (num)
        {
          s[--i] = num % 10 + '0';
          num /= 10;
        }

      size_t len = 64 - i;
      write (1, s + i, len);
    }
  else
    {
      write (1, "0", 1);
    }
}


size_t my_strlen (const char *s)
{
  size_t len = 0;
  while (s[len]) ++len;
  return len;
}


int
main (int argc, char *argv[])
{
  for (int i = 1; i < argc; ++i)
    {
      char *filepath = argv[i];

      struct stat stats;
      int stat_error = lstat (filepath, &stats);
      if (stat_error)
        {
          perror ("stat");
          continue;
        }

      char c = '?';
      if      (S_ISREG (stats.st_mode)) c = '-';
      else if (S_ISDIR (stats.st_mode)) c = 'd';
      else if (S_ISLNK (stats.st_mode)) c = 'l';
      else if (S_ISCHR (stats.st_mode)) c = 'c';
      else if (S_ISBLK (stats.st_mode)) c = 'b';
      else if (S_ISFIFO (stats.st_mode)) c = 'f';
      else if (S_ISSOCK (stats.st_mode)) c = 's';

      char permissions[9];
      if (stats.st_mode & S_IRUSR) permissions[0] = 'r';
      else                         permissions[0] = '-';
      if (stats.st_mode & S_IWUSR) permissions[1] = 'w';
      else                         permissions[1] = '-';
      if (stats.st_mode & S_IXUSR) permissions[2] = 'x';
      else                         permissions[2] = '-';

      if (stats.st_mode & S_IRGRP) permissions[3] = 'r';
      else                         permissions[3] = '-';
      if (stats.st_mode & S_IWGRP) permissions[4] = 'w';
      else                         permissions[4] = '-';
      if (stats.st_mode & S_IXGRP) permissions[5] = 'x';
      else                         permissions[5] = '-';

      if (stats.st_mode & S_IROTH) permissions[6] = 'r';
      else                         permissions[6] = '-';
      if (stats.st_mode & S_IWOTH) permissions[7] = 'w';
      else                         permissions[7] = '-';
      if (stats.st_mode & S_IXOTH) permissions[8] = 'x';
      else                         permissions[8] = '-';

      struct passwd *user_fields = getpwuid (stats.st_uid);
      if (!user_fields)
        {
          perror ("getpwuid");
          continue;
        }
      char *user_name = user_fields->pw_name;

      struct group *group_fields = getgrgid (stats.st_gid);
      if (!group_fields)
        {
          perror ("getgrnam");
          continue;
        }
      char *group_name = group_fields->gr_name;

      write (1, &c, 1);
      write (1, permissions, 9);
      write (1, " ", 1);
      write (1, user_name, my_strlen (user_name));
      write (1, " ", 1);
      write (1, group_name, my_strlen (group_name));
      write (1, " ", 1);
      write_num (stats.st_size);
      write (1, " ", 1);
      write (1, filepath, my_strlen (filepath));
      write (1, "\n", 1);
    }
}
