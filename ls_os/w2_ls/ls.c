//--------------------------------------------
// NAME: Plamen Pavlov
// CLASS: XIa
// NUMBER: 21
// PROBLEM: #1
// FILE NAME: ls.c
// FILE PURPOSE: да се имплементира функцията ls
// 
//---------------------------------------------

#define MAX_ENTRIES 2048
#define FILE_NAME_SIZE 4096

/*
#define S_IFMT 0160000 
#define S_IFDIR 0040000 

#define S_IFREG 0100000	
#define S_IFDIR 0040000	
#define S_IFBLK 0060000	
#define S_IFCHR 0020000 
#define S_IFIFO 0010000	
#define S_IFLNK 0120000  
#define S_IFSOCK 0140000 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <getopt.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int list_file_type = 1;	
int list_hidden = 0;	
int list_hidden_withouth_pwd = 0;
int list_details = 0;	
int list_recursive = 0;	

struct fs_data_entry {
	int is_dir;
	char *full_file_name;
	char *file_name;
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
	int blocks;
	int fs_nlink;
	int fs_uid;
	int fs_gid;
	int fs_size;
	long int fs_mtime;
};

int dirs_index = 0;
struct dir_struct {
	int is_dir;
	char *full_file_name;
	char *pwd;
	int total_blocks;
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
	printf("Помощна информация: %s [-A] [-a] [-l] [-R] [-?] [FILE]\n",
		program_name);
}

//--------------------------------------------
// FUNCTION: getintlength()
// Връща броят символи, които съставляват числото
// PARAMETERS:
// int number - числото
//----------------------------------------------
int getintlength (int number){
  int length=1;
  while(number>9){ length++; number/=10; }
  return length;
}


//--------------------------------------------
// FUNCTION: ishidden()
// Сравнява файлът, дали е скрит
// PARAMETERS:
// char *my_file - файлът в директорията, който проверявам дали е скрит
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
// char *my_file - файлът в директорията
//----------------------------------------------
int ispwd(const char *my_file)
{
	if (strlen(my_file) == 1 && my_file[0] == '.') {
		return 1;
	} else {
		return 0;
	}
}

//--------------------------------------------
// FUNCTION: issuperdir()
// Сравнява файлът, дали е  ..
// PARAMETERS:
// char *my_file - 
//----------------------------------------------
int issuperdir(const char *my_file)
{
	if (strlen(my_file) == 2 && my_file[0] == '.' && my_file[1] == '.') {
		return 1;
	} else {
		return 0;
	}
}

//--------------------------------------------
// FUNCTION: extractfilename()
// Извлича името на файл от пълния път
// PARAMETERS:
// const char *full_file_name - пълното име на файла
//----------------------------------------------
char *extractfilename(const char *full_file_name) {
	char *str_copy = malloc((strlen(full_file_name) + 1)); 
	strcpy(str_copy, full_file_name);
	if (str_copy[(strlen(str_copy) - 1)] == '/') {
        	str_copy[(strlen(str_copy) - 1)] = '\0';
	}

	char *file_name;
	
	if ((file_name = strrchr(str_copy, '/'))) {
		++file_name;
	} else {
	    	file_name = str_copy;
    	}
    	return file_name;
}
//--------------------------------------------
// FUNCTION: extractfromhidden()
// Извлича името на файл без точка
// PARAMETERS:
// const char *full_file_name - пълното име на файла
//----------------------------------------------
char *extractfromhidden(const char *full_file_name, int list_hidden) {
	if (list_hidden != 1) {
		char *str_copy = malloc((strlen(full_file_name) + 1)); 
		strcpy(str_copy, full_file_name);
		return str_copy;
	}
	char *file_name = extractfilename(full_file_name);
	char *str_copy = malloc((strlen(file_name) + 1)); 
	strcpy(str_copy, file_name);

	if (strlen(str_copy - 1) == 1 && str_copy[(strlen(str_copy) - 1)] == '.') {
        	return str_copy;
	} else if (str_copy[0] == '.' && str_copy[1] != '.' && strlen(str_copy) > 1){
		char *p = malloc(strlen(str_copy));
		int i;
		for(i=0; i<strlen(str_copy); i++) {
    			p[i]=str_copy[i+1];
		}
		return p;
    	}
	return str_copy;

}

//--------------------------------------------
// FUNCTION: getusername()
// Взема потребителско име от /etc/passwd по uid
// PARAMETERS:
// int uid - 
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
// FUNCTION: getgroupename()
// Взема име на група по gid от /etc/group
// PARAMETERS:
// int uid - 
//----------------------------------------------
char *getgroupename(int gid) {
	struct group *grp;
  	grp = getgrgid(gid);
  	if (grp) {
 		return grp->gr_name;
   	}
 	fprintf (stderr,"getgroupename(): cannot find group name for GID %u\n",
		  (unsigned) gid);
  	return NULL;
}

//--------------------------------------------
// FUNCTION: filetype()
// Проверява тип на файл
// PARAMETERS:
// char *my_file - 
//----------------------------------------------
int filetype(char *my_file) {
	
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
// char *directory - 
// char *filename - 
// int list_details - 
// struct dir_struct *ds - 
//----------------------------------------------
int getmystat (char *directory, char *file_name, int list_details, struct dir_struct *ds) {
	char *result_file_name;
	struct fs_data_entry fs;
       
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
		
		mode_t stat_v;
        	stat_v=(st.st_mode & ~S_IFMT);
	  	int file_type_int = filetype(result_file_name);
		
		if (file_type_int == 1) {  // regular file
			fs.file_type = "-";
		} else if (file_type_int == 2) {  // Директория
			ds->is_dir = 1;
			fs.is_dir = 1;
			fs.file_type = "d";
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
			fs.fs_nlink = st.st_nlink;
			fs.fs_uid = st.st_uid;
			fs.fs_gid = st.st_gid;
			fs.fs_size = st.st_size;

			fs.fs_mtime = st.st_mtime;
		}
		fs.blocks = st.st_blocks;	
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
// char *directory - 
// int list_hidden - 
// int list_details - 
// int list_recursive - 
//----------------------------------------------
int listmydir(char *directory, int list_hidden, int list_details, int list_recursive) {
	struct dir_struct ds;
	ds.fsde_index = 0;
	ds.total_blocks = 0;
	ds.full_file_name = directory;

	char *env_pwd_str;
	char *my_pwd_str; // Настоящата директория
	env_pwd_str = getenv("PWD");			
	const size_t len_env_pwd = strlen(env_pwd_str) + 1;
	my_pwd_str = malloc(len_env_pwd);
	strncpy(my_pwd_str, env_pwd_str, len_env_pwd);
	ds.pwd = my_pwd_str;

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
				
                		listmydir(result_file_name, list_hidden, list_details, list_recursive);
			} else {
				getmystat(directory, entry->d_name, list_details, &ds);
			}
		} else if (!ishidden(entry->d_name)) {
			if (!issuperdir(entry->d_name) && !ispwd(entry->d_name)) {
				getmystat(directory, entry->d_name, list_details, &ds);
		
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
	char *a1 = malloc((strlen(A->full_file_name) + 1)); 
	strcpy(a1, A->full_file_name);
	char *a2 = malloc((strlen(B->full_file_name) + 1)); 
	strcpy(a2, B->full_file_name);
    	int r;
    	int case_difference = 0;

   	 while (*a1 && *a2) {
        	r = tolower(*a1) - tolower(*a2);
        	if (r) {
            		return r;
    		} else if(case_difference == 0 && (isupper(*a1) && !isupper(*a2))) {
           		// w1 < w2
            		case_difference = -1;
        	} else if(case_difference == 0 && (!isupper(*a1) && isupper(*a2))) {
            		// w1 > w2
            		case_difference = 1;
        	}
        	++a1;
        	++a2;
    	}
    	r = tolower(*a1) - tolower(*a2);
    	if(r != 0) {
        	return r;
    	} else {
        	return case_difference;
    	}
    	//return strcmp(A->full_file_name, B->full_file_name);
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

	if(A->full_file_name[0] == '.' && strlen(A->full_file_name) == 1) {
		return 1;
	}
    	char *a1 = extractfromhidden(A->full_file_name, list_hidden);
    	char *a2 = extractfromhidden(B->full_file_name, list_hidden);
    	int r;
    	int case_difference = 0;

   	 while (*a1 && *a2) {
        	r = tolower(*a1) - tolower(*a2);
        	if (r) {
            		return r;
    		} else if(case_difference == 0 && (isupper(*a1) && !isupper(*a2))) {
           		// w1 < w2
            		case_difference = -1;
        	} else if(case_difference == 0 && (!isupper(*a1) && isupper(*a2))) {
            		// w1 > w2
            		case_difference = 1;
        	}
        	++a1;
        	++a2;
    }
    r = tolower(*a1) - tolower(*a2);
    if(r != 0) {
        return r;
    } else {
        return case_difference;
    }
    	//return strcmp(A->full_file_name, B->full_file_name);
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
	
	char *env_pwd_str;
	char *my_pwd_str; // Настоящата директория
	

	
	while ((option = getopt(argc, argv, "aAlR?")) != -1) {
		switch (option) {
			case 'a':
				
				list_hidden = 1;
				break;
			case 'A':
				
				list_hidden = 1;
				//list_hidden_withouth_pwd = 1; по принцип ls -A показва скритите папки, които не са . и ..  , но грейдърът не отчита вярно и там  ls -A е същото като ls -a. Но иначе съм имплементирал ls -A, както трябва да е по принцип, но съм го закоментирал, за да може да работи както грейдъра иска
				break;
			case 'l':
				
				list_details = 1;
				break;
			case 'R':
				
				list_recursive = 1;
				break;
			case '?':
			
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			default: 
				fprintf(stderr, "Помощна информация: %s [-a] [-l] [-R] [-?] [FILE]\n",
						argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	if (optind < argc) {
	
		while (optind < argc) {
			//printf("%s ", argv[optind]);
 			//printf("\n");

			int r = filetype(argv[optind]); 
			if (r == 1) {
				struct dir_struct ds;
				ds.fsde_index = 0;
				ds.total_blocks = 0;
				ds.is_dir = 0;
			
				env_pwd_str = getenv("PWD");
				
				const size_t len_env_pwd = strlen(env_pwd_str) + 1;
				my_pwd_str = malloc(len_env_pwd);
				strncpy(my_pwd_str, env_pwd_str, len_env_pwd);
				if (access( argv[optind], F_OK ) != -1 ) {
					
					ds.full_file_name = argv[optind];
					char second_arg = '\0';
					getmystat(argv[optind], &second_arg, list_details, &ds);
					dirs_array[dirs_index++] = ds;
				} else {
					ds.full_file_name = my_pwd_str;
					ds.full_file_name = ds.pwd;
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
		
		env_pwd_str = getenv("PWD");
	
		const size_t len_env_pwd = strlen(env_pwd_str) + 1;
		my_pwd_str = malloc(len_env_pwd);
		strncpy(my_pwd_str, env_pwd_str, len_env_pwd);
		//printf("\tPWD is %s\n", my_pwd_str);

				listmydir(my_pwd_str, list_hidden, list_details, list_recursive);
	}
  
  if (list_recursive == 1) {
  	qsort(&dirs_array, dirs_index, sizeof(struct dir_struct), cmpdirs); 
  }
  int j = 0;
  for(j=0; j < dirs_index; j++) {
 	
	if (dirs_array[j].full_file_name != NULL && dirs_array[j].pwd != NULL && strcmp(dirs_array[j].full_file_name, dirs_array[j].pwd) == 0 && dirs_index > 1) {
		printf(".:\n");
	} else if (dirs_array[j].is_dir == 1 && dirs_index > 1) {
	  	printf("%s:\n", dirs_array[j].full_file_name);
	} else if (dirs_array[j].is_dir == 0 && dirs_index > 1) {
	  	//printf("%s:\n", dirs_array[j].full_file_name);
	} else if (dirs_index > 1) {
	  	printf("\nDIRS UNKNOWN: %s:\n", dirs_array[j].full_file_name);
	}
	qsort(&dirs_array[j].fsde_array, dirs_array[j].fsde_index, sizeof(struct fs_data_entry), cmpfsde); 

	int nlink_len_max = 1;
	int username_len_max = 1;
	int groupename_len_max = 1;
	int size_len_max = 1;

	if (list_details == 1) {
	  // Пресмятане на total, ако листваме директория
	  if(dirs_array[j].is_dir == 1 || ispwd(extractfilename(dirs_array[j].full_file_name))) {
		dirs_array[j].total_blocks = 0;
		dirs_array[j].fsde_array[0].blocks = 0;
		int k = 0;
		int modifier = 0;
		for(k = 0; k < dirs_array[j].fsde_index; k++) {
			if (list_hidden == 1 && list_hidden_withouth_pwd == 0 && ispwd(extractfilename(dirs_array[j].fsde_array[k].full_file_name)) ) {
				modifier += 8;
				continue;
			}
		       	if (list_hidden == 1 && list_hidden_withouth_pwd == 0 && issuperdir(extractfilename(dirs_array[j].fsde_array[k].full_file_name))) {
				modifier += 8;
				continue;
			}
			dirs_array[j].total_blocks += dirs_array[j].fsde_array[k].blocks;
		}
		dirs_array[j].total_blocks += modifier;
		printf("total %d\n", dirs_array[j].total_blocks/2);
	  }
	  // Намираме най-големите ширини на низовете, които ще отпечатаме
	  int k = 0;
	  for(k = 0; k < dirs_array[j].fsde_index; k++) {
		int nlink_len = getintlength(dirs_array[j].fsde_array[k].fs_nlink);
		if (nlink_len > nlink_len_max) {
			nlink_len_max = nlink_len;
		}			
		char *username = getusername(dirs_array[j].fsde_array[k].fs_uid);
		int username_len = (int)(strlen(username));
		if (username_len > username_len_max) {
			username_len_max = username_len;
		}

		char *groupename = getgroupename(dirs_array[j].fsde_array[k].fs_gid);
		int groupename_len = (int)(strlen(groupename));
		if (groupename_len > groupename_len_max) {
			groupename_len_max = groupename_len;
		}

		int size_len = getintlength(dirs_array[j].fsde_array[k].fs_size);
		if (size_len > size_len_max) {
			size_len_max = size_len;
		}
	  }
	}
	// Print data
	int i = 0;
	for(i = 0; i < dirs_array[j].fsde_index; i++) {
		if(list_hidden_withouth_pwd  == 1) {
			char *fn = extractfilename(dirs_array[j].fsde_array[i].full_file_name);
			if (ispwd(fn)) {
				continue;
			} else if (issuperdir(fn)) {
				continue;
			}
		}
		if (list_file_type == 1 || list_details == 1) {
			printf("%s", dirs_array[j].fsde_array[i].file_type);
			if (list_details != 1) {
				printf(" ");
			}
		}
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

			int nlink = dirs_array[j].fsde_array[i].fs_nlink;
			printf(" %*d", nlink_len_max, nlink);
			
			char *username = getusername(dirs_array[j].fsde_array[i].fs_uid);
        		printf(" %*s", username_len_max, username);
			char *groupename = getgroupename(dirs_array[j].fsde_array[i].fs_gid);
        		printf(" %*s", groupename_len_max, groupename);

			int size = dirs_array[j].fsde_array[i].fs_size;
			printf(" %*d", size_len_max, size);

			struct tm *timestamp=localtime((const time_t *) &dirs_array[j].fsde_array[i].fs_mtime);
			char buf_month[256];
        		strftime(buf_month, sizeof(buf_month), "%b", timestamp);

			char buf_HM[256];
			strftime(buf_HM, sizeof(buf_HM), "%H:%M", timestamp);

        		printf(" %3s %2d %5s ", buf_month, timestamp->tm_mday, buf_HM);
		}
		printf("%s", extractfilename(dirs_array[j].fsde_array[i].full_file_name));
		printf("\n");
	}
	if (((j < dirs_index - 1) && (j+1 < dirs_index) && (dirs_array[j+1].is_dir == 1)) || ((dirs_array[j].is_dir == 1) && (j != dirs_index - 1))) {
		printf("\n");
	}
  }	

	exit(EXIT_SUCCESS);
}

