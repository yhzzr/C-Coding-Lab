// Main program file MAIN7.C
// Written by Daniel W. Lewis
// Revised Feb 13, 2015
//
// Purpose: Create horizontally and vertically mirrored versions of an image using multi-threading.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "bmp3.h"
#include <pthread.h>

void *ThreadMirrorCols(void *arg);
IMAGE *MirrorLeftRight(IMAGE *image);

void *ThreadMirrorRows(void *arg);
IMAGE *MirrorUpDown(IMAGE *image);

IMAGE * image;
typedef struct
	{
	char	*format ;
	IMAGE	*(*function)() ;
	double	param ;
	} OPTION ;

#define NUM_THREADS 8 
pthread_mutex_t mutex;

OPTION	*GetOption(int argc, char **argv, OPTION options[]) ;
void	DisplayImage(char *filespec, char *format, ...) ;

int main(int argc, char **argv)
	{
	char *src_filespec, *dst_filespec ;
	pthread_mutex_init(&mutex,NULL);
	static OPTION options[] =
		{
		{"Mirror Left-Right",	MirrorLeftRight,	   0},
		{"Mirror Up-Down",		MirrorUpDown,		   0},
		{NULL,					NULL,				   0}
		} ;
	OPTION *option ;

	if (argc != 3 && argc != 4)
		{
		fprintf(stderr, "Usage: %s src-file dst-file {option#}\n", argv[0]) ;
		exit(255) ;
		}

	src_filespec = argv[1] ;
	dst_filespec = argv[2] ;

	image = ReadBMP24(src_filespec) ;

	if ((option = GetOption(argc, argv, options))== NULL)
		{
		fprintf(stderr, "Usage: %s src-file dst-file {option#}\n", argv[0]) ;
		exit(255) ;
		}

	image = (option->function)(image, option->param) ;

	WriteBMP24(dst_filespec, image) ;
	FreeImage(image) ;
	DisplayImage(dst_filespec, option->format, option->param) ;

	return 0 ;
	}


void *ThreadMirrorCols(void *arg)
	{
		// To be completed by student ...
		int col_num = (int)arg;
		unsigned temp_b;
		unsigned temp_g;
		unsigned temp_r;
		int cols = image->cols-1;
		
		pthread_mutex_lock(&mutex);
		for(int i=col_num;i<((image->cols)/2);i+=NUM_THREADS)
		{	
			for(int j=0;j<image->rows;j++)
			{
				temp_b = image->pxlrow[j][i].blu;
				temp_g = image->pxlrow[j][i].grn;
				temp_r = image->pxlrow[j][i].red;

				image->pxlrow[j][i].blu = image->pxlrow[j][cols-i].blu;
				image->pxlrow[j][i].grn = image->pxlrow[j][cols-i].grn;
				image->pxlrow[j][i].red = image->pxlrow[j][cols-i].red;

				image->pxlrow[j][cols-i].blu = temp_b;
				image->pxlrow[j][cols-i].grn = temp_g;
				image->pxlrow[j][cols-i].red = temp_r;
			}
		}
		pthread_mutex_unlock(&mutex);
		pthread_exit(NULL);
	}

IMAGE *MirrorLeftRight(IMAGE *image)
	{
	   	// To be completed by student ...
		pthread_t threads[NUM_THREADS];
		
		//creating the threads
		long i,j;
		for(i=0;i<NUM_THREADS;i++)
			pthread_create(&threads[i],NULL,ThreadMirrorCols, (void *)i);
		
		//waiting for all the threads to finish
		for(j=0;j<NUM_THREADS;j++)
			pthread_join(threads[j],NULL);

		return image;	
	}

void *ThreadMirrorRows(void *arg)
	{
		// To be completed by student ...
		int row_num = (int)arg;
		int rows = (image->rows)-1;
		PIXEL *temprow;
		pthread_mutex_lock(&mutex);
		for(int i=row_num;i<((image->rows)/2);i+=NUM_THREADS)
		{
			temprow = image->pxlrow[i];
			image->pxlrow[i] = image->pxlrow[rows-i];
			image->pxlrow[rows-i] = temprow;
		}
		pthread_mutex_unlock(&mutex);
		pthread_exit(NULL);
	}

IMAGE *MirrorUpDown(IMAGE *image)
	{
        	// To be completed by student ...
		pthread_t threads[NUM_THREADS];
		
		//creating the threads
		long m,n;
		for(m=0;m<NUM_THREADS;m++)
			pthread_create(&threads[m],NULL,ThreadMirrorRows,(void *)m);
			
		//waiting for all the threads to finish
		for(n=0;n<NUM_THREADS;n++)
			pthread_join(threads[n],NULL);

		return image;
	}


OPTION *GetOption(int argc, char **argv, OPTION options[])
	{
	unsigned number, index = 0 ;

	// count number of options
	for (number = 0; options[number].function != NULL; number++) ;

	if (argc == 4)
		{
		sscanf(argv[3], "%u", &index) ;
		}
	else
		{
		printf("\nOptions:\n\n") ;
		for (index = 0; index < number; index++)
			{
			char title[100] ;
			sprintf(title, options[index].format, options[index].param) ;
			printf("%2d: %s\n", index + 1, title) ;
			}

		printf("\nOption? ") ;
		scanf("%u", &index) ;
		}

	return (1 <= index && index <= number) ? &options[index - 1] : NULL ;
	}

void DisplayImage(char *filespec, char *format, ...)
	{
	char progspec[200], command[1000], *program_files, title[100] ;
	struct stat filestat ;
	va_list args ;
	
	va_start(args, format) ;
	vsprintf(title, format, args) ;
	va_end(args) ;

#if defined(_WIN32)
	program_files = getenv("ProgramFiles(x86)") ;
	if (program_files == NULL) program_files = getenv("C:\\Program Files") ;
	if (program_files == NULL) program_files = "" ;

	sprintf(progspec, "%s\\IrfanView\\i_view32.exe", program_files) ;
	if (stat(progspec, &filestat) == 0 && (filestat.st_mode & S_IFREG) != 0)
		{
		sprintf(command, "\"%s\" %s /hide=7 /title=%s", progspec, filespec, title) ;
		}
	else sprintf(command, "mspaint \"%s\"\n", filespec) ;
#elif defined(__unix)
	sprintf(command, "qiv \"%s\"", filespec) ;
#elif defined(__APPLE__)
	sprintf(command, "open \"%s\"", filespec) ;
#endif 
	system(command) ;
	}

