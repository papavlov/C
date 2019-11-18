//--------------------------------------------
// NAME: Plamen Pavlov
// CLASS: XIa
// NUMBER: 21
// PROBLEM: #1
// FILE NAME: ls.c
// FILE PURPOSE:
// 
//---------------------------------------------

// Нужно е за C99
#define MAX_ENTRIES 60000
#define FILE_NAME_SIZE 1024

#define S_IFMT 0160000 /* файл */
#define S_IFDIR 0040000 /* директория */

#define S_IFREG 0100000	/* регулярен файл */
#define S_IFDIR 0040000	/* директория */ 
#define S_IFBLK 0060000	/* блоково устройство */
#define S_IFCHR 0020000 /* символно устройство */
#define S_IFIFO 0010000	/* FIFO */
#define S_IFLNK 0120000  /* simwolna prepratka */
#define S_IFSOCK 0140000 /* сокет */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <getopt.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

struct fs_data_entry {
	char *full_file_name;
	char *file_name;
	int is_dir;
	char *file_type;
	char *rusr;
	char *wusr;
	char *xusr;
	char *rgrp;
	char *wgrp;
	char *xgrp;
	char *roth;
	char *woth;
	char *xoth;
	int nlink;
	int st_uid;
	int st_gid;
	int st_size;
	int st_mtime;
};

int dirs_index = 0;
struct dir_struct {
	int is_dir;
	char *full_file_name;
	char *pwd;
	int fsde_index;
	struct fs_data_entry fsde_array[4096];
};
struct dir_struct dirs_array[2048];

//--------------------------------------------
// FUNCTION: usage()
// Изписва в stdout помощна информация за
// употребата на тази програма
// PARAMETERS:
// program_name - Името на програмата
//----------------------------------------------
void usage(char *program_name) {
	printf("Помощна информация: %s [-a] [-l] [-R] [-?] [FILE]\n",
		program_name);
}


//--------------------------------------------
// FUNCTION: ishidden()
// Сравнява файлът, дали е скрит
// PARAMETERS:
// char *my_file,
//----------------------------------------------
int ishidden(const char *my_file)
{
	if (my_file[0] == '.') {
		return 1;
	} else {
		return 0;
	}
}
//--------------------------------------------
// FUNCTION: ispwd()
// Сравнява файлът, дали е  .
// PARAMETERS:
// char *my_file,
//----------------------------------------------
int ispwd(const char *my_file)
{
	if (my_file[0] == '.') {
		return 1;
	} else {
		return 0;
	}
}

//--------------------------------------------
// FUNCTION: issuperdir()
// Сравнява файлът, дали е  ..
// PARAMETERS:
// char *my_file,
//----------------------------------------------
int issuperdir(const char *my_file)
{
	if (my_file[0] == '.' && my_file[1] == '.') {
		return 1;
	} else {
		return 0;
	}
}

//--------------------------------------------
// FUNCTION: getusername()
// Взема потребителско име от /etc/passwd по uid
// PARAMETERS:
// int uid,
//----------------------------------------------
char *getusername(int uid) {
  	struct passwd *pw;
  	pw = getpwuid(uid);
  	if (pw) {
 		return pw->pw_name;
   	}
 	fprintf (stderr,"getusername(): cannot find username for UID %u\n",
		  (unsigned) uid);
  	return NULL;
}

//--------------------------------------------
// FUNCTION: getgroupname()
// Взема име на група по gid от /etc/group
// PARAMETERS:
// int uid,
//----------------------------------------------
char *getgroupname(int gid) {
	struct group *grp;
  	grp = getgrgid(gid);
  	if (grp) {
 		return grp->gr_name;
   	}
 	fprintf (stderr,"getgroupname(): cannot find group name for GID %u\n",
		  (unsigned) gid);
  	return NULL;
}

//--------------------------------------------
// FUNCTION: filetype()
// Проверява тип на файл
// PARAMETERS:
// char *my_file,
//----------------------------------------------
int filetype(char *my_file) {
	// http://man7.org/linux/man-pages/man2/stat.2.html
	struct stat st;
	if ( stat(my_file, &st) == 0 ) {
		switch (st.st_mode & S_IFMT) {
			case S_IFREG:
				//printf("regular file\n");
				return 1;
			case S_IFDIR:
				//printf("directory\n");
				return 2;
			case S_IFBLK:
				//printf("block device\n");
				return 3;
			case S_IFCHR:
				//printf("character device\n");
				return 4;
			case S_IFIFO:
				//printf("FIFO/pipe\n");
				return 5;
			case S_IFLNK:
				//printf("symlink\n");
				return 6;
			case S_IFSOCK:
				//printf("socket\n");
				return 7;
			default:
				//printf("unknown?\n");
				return -1;
		}
	} else {
		fprintf(stderr, "ls: cannot access %s: No such file or directory\n",
			my_file);
		return -2;			
	}
	return -3;
}

//--------------------------------------------
// FUNCTION: getmystat()
// Взема статуса на файл
// PARAMETERS:
// char *directory,
// char *filename,
// int list_details,
// struct dir_struct *ds,
//----------------------------------------------
int getmystat (char *directory, char *file_name, int list_details, struct dir_struct *ds) {
	char *result_file_name;
	struct fs_data_entry fs;
       	// TODO '//' Повтарящи се наклонени черти
	if (file_name[0] != '\0') {
		result_file_name = malloc((strlen(directory) + strlen(file_name) + 1 + 1)); 
		strcpy(result_file_name, directory);
		strcat(result_file_name, "/");
		strcat(result_file_name, file_name);
	} else {
		result_file_name = malloc((strlen(directory) + 1)); 
		strcpy(result_file_name, directory);
	}
	struct stat st;
	if ( stat(result_file_name, &st) == 0 ) {
		// S_IFMT е битмаска за файловия тип
		mode_t stat_v;
		// AND NOT взема предвид само правата на файловете
        	stat_v=(st.st_mode & ~S_IFMT);
	  	int file_type_int = filetype(result_file_name);
		
		if (file_type_int == 1) {  // regular file
			fs.file_type = "-";
		} else if (file_type_int == 2) {  // Директория
			ds->is_dir = 1;
			fs.file_type = "d";
			fs.is_dir = 1;
		} else if (file_type_int == 3) {
			fs.file_type = "b";
		} else if (file_type_int == 4) {
			fs.file_type = "c";
		} else if (file_type_int == 5) {
			fs.file_type = "p";
		} else if (file_type_int == 6) {
			fs.file_type = "I";
		} else if (file_type_int == 7) {
			fs.file_type = "s";
		} else {
			fs.file_type = "?";
		}
		if (list_details == 1) {
			if (stat_v & S_IRUSR) {
				fs.rusr = "r";
			} else {
				fs.rusr = "-";
			}
        		if (stat_v & S_IWUSR) {
				fs.wusr = "w";
			} else { 
				fs.wusr = "-";
			}
        		if (stat_v & S_IXUSR) {
				fs.xusr = "x";
			} else {
				fs.xusr = "-";
			}
        		if (stat_v & S_IRGRP) {
				fs.rgrp = "r";
			} else {
				fs.rgrp = "-";
			}
        		if (stat_v & S_IWGRP) {
				fs.wgrp = "w";
			} else {
				fs.wgrp = "-";
			}
        		if (stat_v & S_IXGRP) {
				fs.xgrp = "x";
			} else {
				fs.xgrp = "-";
			}
        		if (stat_v & S_IROTH) {
				fs.roth = "r";
			} else {
				fs.roth = "-";
			}
        		if (stat_v & S_IWOTH) {
				fs.woth = "w";
			} else {
				fs.woth = "-";
			}
        		if (stat_v & S_IXOTH) {
				fs.xoth = "x";
			} else {
				fs.xoth = "-";
			}
			
			fs.nlink = st.st_nlink;
			fs.st_uid = st.st_uid;
			fs.st_gid = st.st_gid;
			fs.st_size = st.st_size;

			fs.st_mtime = st.st_mtime;
		}
		if (file_name[0] == '\0') {
			fs.file_name = result_file_name;
		} else {
			fs.file_name = file_name;
		}
		fs.full_file_name = result_file_name;
		ds->fsde_array[ds->fsde_index++] = fs;
		//fsde_array[fsde_index++] = fs;
		//printf("\t Файл: %s\n", result_file_name);
	}

	return 0;
}

//--------------------------------------------
// FUNCTION: listmydir()
// Показва съдържанието на зададената директория
// PARAMETERS:
// char *directory,
// int list_hidden,
// int list_details,
// int list_recursive,
//----------------------------------------------
int listmydir(char *directory, int list_hidden, int list_details, int list_recursive) {
	struct dir_struct ds;
	ds.fsde_index = 0;
	ds.full_file_name = directory;

	DIR *dir = opendir(directory);
	struct dirent *entry;
	//size_t len = strlen(directory);

	if (!(dir = opendir(directory))) {
        	fprintf(stderr, "ls: cannot open directory %s: %s\n",
                	directory, strerror(errno));
        	return -1;
    	}
	//printf("%s:\n", directory);
	while ((entry = readdir(dir))) {
	  char *result_file_name = malloc((strlen(directory) + strlen(entry->d_name) + 1 + 1)); 
	  strcpy(result_file_name, directory);
	  strcat(result_file_name, "/");
	  strcat(result_file_name, entry->d_name);
	  
	  int file_type_int = filetype(result_file_name);
	  if (file_type_int == 2 && list_recursive == 1) {  // Е директория
		ds.is_dir = 1;
		if (ishidden(entry->d_name) && list_hidden == 1) {
			if (!issuperdir(entry->d_name) && !ispwd(entry->d_name)) {
				getmystat(directory, entry->d_name, list_details, &ds);
				// Рекурсия
                		listmydir(result_file_name, list_hidden, list_details, list_recursive);
			} else {
				getmystat(directory, entry->d_name, list_details, &ds);
			}
		} else if (!ishidden(entry->d_name)) {
			if (!issuperdir(entry->d_name) && !ispwd(entry->d_name)) {
				getmystat(directory, entry->d_name, list_details, &ds);
				// Рекурсия
                		listmydir(result_file_name, list_hidden, list_details, list_recursive);
			} else {
				getmystat(directory, entry->d_name, list_details, &ds);
			}
		}
	  } else {
		// Не е директория
		//ds.is_dir = 0;
		if (ishidden(entry->d_name) && list_hidden == 1) {
			getmystat(directory, entry->d_name, list_details, &ds);
		} else if (!ishidden(entry->d_name)) {
			getmystat(directory, entry->d_name, list_details, &ds);
		}
	  }
	}
	dirs_array[dirs_index++] = ds;
	closedir(dir);
	return 0;
}

//--------------------------------------------
// FUNCTION: extractfilename()
// Извлича името на файл от пълния път
// PARAMETERS:
// const char *full_file_name,
//----------------------------------------------
char *extractfilename(const char *full_file_name) {
	char *str_copy = malloc((strlen(full_file_name) + 1)); 
	strcpy(str_copy, full_file_name);
	if (str_copy[(strlen(str_copy) - 1)] == '/') {
        	str_copy[(strlen(str_copy) - 1)] = '\0';
	}

	char *file_name;
	// strrchr() Връща последното срещане на символ / в низа str_copy
	if ((file_name = strrchr(str_copy, '/'))) {
		++file_name;
	} else {
	    	file_name = str_copy;
    	}
    	return file_name;
}

//--------------------------------------------
// FUNCTION: cmpdirs()
// Сравняваща функция за qsort() на
// масивът dirs_array
// PARAMETERS:
// const void *a,
// const void *b,
//----------------------------------------------
int cmpdirs(const void *a, const void *b) {
	const struct dir_struct *A = a;
	const struct dir_struct *B = b;
    	return strcmp(A->full_file_name, B->full_file_name);
}
//--------------------------------------------
// FUNCTION: cmpfsde()
// Сравняваща функция за qsort()
// PARAMETERS:
// const void *a,
// const void *b,
//----------------------------------------------
int cmpfsde(const void *a, const void *b) {
	const struct fs_data_entry *A = a;
	const struct fs_data_entry *B = b;
	if(A->full_file_name[0] == '.' && strlen(A->full_file_name) == 2) {
		return 1;
	}
    	return strcmp(A->full_file_name, B->full_file_name);
}

//--------------------------------------------
// FUNCTION: main()
// Функцията, извиквана от операционната система,
// при стартиране на програмата
// PARAMETERS:
// int argc,
// char *argv[],
//----------------------------------------------
int main(int argc, char *argv[]) {
	int option;
	int list_hidden = 0;	// 0 или 1
	int list_details = 0;	// 0 или 1
       	int list_recursive = 0;	// 0 или 1
	
	char *env_pwd_str;
	char *my_pwd_str; // Настоящата директория
	

	// Обхождаме argv и правим необходимото за всяка отделна опция
	// чрез switch. Когато всички опции са парснати, getopt ще върне -1
	while ((option = getopt(argc, argv, "alR?")) != -1) {
		switch (option) {
			case 'a':
				// Покажи скритите файлове (започващи с точка)
				//printf("\tОпция a\n");
				list_hidden = 1;
				break;
			case 'l':
				// Покажи подробна информация
				//printf("\tОпция l\n");
				list_details = 1;
				break;
			case 'R':
				// Покажи файловете рекурсивно
				//printf("\tОпция R\n");
				list_recursive = 1;
				break;
			case '?':
				// Помощна информация за
				// използването на програмата
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			default: /* Грешка */
				fprintf(stderr, "Помощна информация: %s [-a] [-l] [-R] [-?] [FILE]\n",
						argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	// Вземаме всички аргументи, които не са изброени като опции.
	if (optind < argc) {
		//printf("\tАргументи, които не са изброени в опциите: ");
		// Променливата optind е индекса на следващия елемент, който ще
		// бъде обработен в argv.
		while (optind < argc) {
			//printf("%s ", argv[optind]);
 			//printf("\n");

			int r = filetype(argv[optind]); // Проверка, дали е директория (стойност 2)
			if (r == 1) {
				struct dir_struct ds;
				ds.fsde_index = 0;
				ds.is_dir = 0;
				// Функцията getenv() връща указател към низ на променлива от средата.	
				env_pwd_str = getenv("PWD");
				// Копираме съдържанието на env_pwd_str в my_pwd_str
				const size_t len_env_pwd = strlen(env_pwd_str) + 1;
				my_pwd_str = malloc(len_env_pwd);
				strncpy(my_pwd_str, env_pwd_str, len_env_pwd);
				// Проверяваме, дали, файлът съществува
				if (access( argv[optind], F_OK ) != -1 ) {
					// Файлът се достъпва => вече е с пълен път
					char second_arg = '\0';
					ds.full_file_name = argv[optind];
					getmystat(argv[optind], &second_arg, list_details, &ds);
					dirs_array[dirs_index++] = ds;
				} else {
					ds.full_file_name = my_pwd_str;
					getmystat(my_pwd_str, argv[optind], list_details, &ds);
					dirs_array[dirs_index++] = ds;
				}
			} else if (r == 2) {
				listmydir(argv[optind], list_hidden, list_details, list_recursive);
			}
			optind++;
		}
 		//printf("\n");
	} else {
		// Няма аргументи => покажи файловете в
		// текущата директория
		//printf("\tНяма аргументи [FILE].");

		// Функцията getenv() връща указател към низ на променлива от средата.	
		env_pwd_str = getenv("PWD");
		// Копираме съдържанието на env_pwd_str в my_pwd_str
		const size_t len_env_pwd = strlen(env_pwd_str) + 1;
		my_pwd_str = malloc(len_env_pwd);
		strncpy(my_pwd_str, env_pwd_str, len_env_pwd);
		//printf("\tPWD is %s\n", my_pwd_str);

		// Извикваме нашата функция listmydir()
		listmydir(my_pwd_str, list_hidden, list_details, list_recursive);

	}
  // Сортиране dirs_array
  // TODO UNIQUE
  qsort(&dirs_array, dirs_index, sizeof(struct dir_struct), cmpdirs); 
  int j = 0;
  for(j=0; j < dirs_index; j++) {
	// Прескачаме празните директории
	if(dirs_array[j].fsde_index == 0) {
		continue;
	}
	// Печатаме директорията, която се листва
	if (dirs_array[j].is_dir == 1) {
	  	printf("\n%s:\n", dirs_array[j].full_file_name);
	} else if (dirs_array[j].is_dir == 0) {
	  	printf("\n%s:\n", dirs_array[j].full_file_name);
	} else {
	  	printf("\nDIRS UNKNOWN: %s:\n", dirs_array[j].full_file_name);
	}
	// Сортиране fsde_array
	qsort(&dirs_array[j].fsde_array, dirs_array[j].fsde_index, sizeof(struct fs_data_entry), cmpfsde); 
	// Print data
	int i = 0;
	for(i = 0; i < dirs_array[j].fsde_index; i++) {
		printf("%s", dirs_array[j].fsde_array[i].file_type);
		if (list_details == 1) {
			printf("%s", dirs_array[j].fsde_array[i].rusr);
			printf("%s", dirs_array[j].fsde_array[i].wusr);
			printf("%s", dirs_array[j].fsde_array[i].xusr);
			printf("%s", dirs_array[j].fsde_array[i].rgrp);
			printf("%s", dirs_array[j].fsde_array[i].wgrp);
			printf("%s", dirs_array[j].fsde_array[i].xgrp);
			printf("%s", dirs_array[j].fsde_array[i].roth);
			printf("%s", dirs_array[j].fsde_array[i].woth);
			printf("%s", dirs_array[j].fsde_array[i].xoth);
			printf("  %d", dirs_array[j].fsde_array[i].nlink);
        		printf("  %s", getusername(dirs_array[j].fsde_array[i].st_uid));
        		printf("  %s", getgroupname(dirs_array[j].fsde_array[i].st_gid));
			printf("  %d", dirs_array[j].fsde_array[i].st_size);

			struct tm *timestamp=localtime((const time_t *) &dirs_array[j].fsde_array[i].st_mtime);
			char buf_month[256];
        		strftime(buf_month, 64, "%b", timestamp);

			char buf_HM[256];
			strftime(buf_HM, 64, "%H:%M", timestamp);

        		printf("\t%s %2d %s", buf_month, timestamp->tm_mday, buf_HM);
		}
		printf("  %s", extractfilename(dirs_array[j].fsde_array[i].full_file_name));
		printf("\n");
	}
  }	

	exit(EXIT_SUCCESS);
}

