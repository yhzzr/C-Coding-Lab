// Main program file MAIN5.C
// Written by Daniel W. Lewis
// Revised Jan 2, 2015
//
// Purpose: Create double size and half size versions of an image.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "bmp3.h"

IMAGE	*HalfSize(IMAGE *image) ;
IMAGE	*DoubleSize(IMAGE *image) ;

typedef struct
	{
	char	*format ;
	IMAGE	*(*function)() ;
	double	param ;
	} OPTION ;

OPTION	*GetOption(int argc, char **argv, OPTION options[]) ;
void	DisplayImage(char *filespec, char *format, ...) ;

int main(int argc, char **argv)
	{
	char *src_filespec, *dst_filespec ;
	static OPTION options[] =
		{
		{"Half Size",			HalfSize,			  0.0},
		{"Double Size",			DoubleSize,			  0.0},
		{NULL,					NULL,				  0.0}
		} ;
	OPTION *option ;
	IMAGE *image ;

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

IMAGE *DoubleSize(IMAGE *src)
	{
	// To be completed by student ...
        int i,j;
        unsigned rows, cols;
        
        rows = (src->rows) * 2;
        cols = (src->cols) * 2;
        
        IMAGE *dst;
        dst = NewImage(rows,cols);
        
        for(i=0;i<rows;i =i+2)
        {
            for(j=0;j<cols;j=j+2)
            {
                dst->pxlrow[i][j].blu = src->pxlrow[i/2][j/2].blu;
                dst->pxlrow[i+1][j].blu = src->pxlrow[i/2][j/2].blu;
                dst->pxlrow[i][j+1].blu = src->pxlrow[i/2][j/2].blu;
                dst->pxlrow[i+1][j+1].blu = src->pxlrow[i/2][j/2].blu;
                
                dst->pxlrow[i][j].red = src->pxlrow[i/2][j/2].red;
                dst->pxlrow[i+1][j].red = src->pxlrow[i/2][j/2].red;
                dst->pxlrow[i][j+1].red = src->pxlrow[i/2][j/2].red;
                dst->pxlrow[i+1][j+1].red = src->pxlrow[i/2][j/2].red;
                
                dst->pxlrow[i][j].grn = src->pxlrow[i/2][j/2].grn;
                dst->pxlrow[i+1][j].grn = src->pxlrow[i/2][j/2].grn;
                dst->pxlrow[i][j+1].grn = src->pxlrow[i/2][j/2].grn;
                dst->pxlrow[i+1][j+1].grn = src->pxlrow[i/2][j/2].grn;
            }
	}
 
        return dst;
	}

IMAGE *HalfSize(IMAGE *src)
	{
	// To be completed by student ...
        int i,j;
        
        IMAGE *dst;
	dst = NewImage((src->rows+1)/2,(src->cols+1)/2);

	unsigned blue;
	unsigned gree;
	unsigned reed;

	for(i=0;i<(src->rows);i+=2)
		for(j=0;j<(src->cols);j+=2)
		{
			if((i+1)<src->rows)
			{
				if(j+1<src->cols)
				{
					blue = (src->pxlrow[i][j].blu + src->pxlrow[i+1][j].blu + src->pxlrow[i][j+1].blu + src->pxlrow[i+1][j+1].blu)/4;
					gree = (src->pxlrow[i][j].grn + src->pxlrow[i+1][j].grn + src->pxlrow[i][j+1].grn + src->pxlrow[i+1][j+1].grn)/4;
					reed = (src->pxlrow[i][j].red + src->pxlrow[i+1][j].red + src->pxlrow[i][j+1].red + src->pxlrow[i+1][j+1].red)/4;
				}
				else
				{
					blue = (src->pxlrow[i][j].blu + src->pxlrow[i+1][j].blu)/2; 
					gree = (src->pxlrow[i][j].grn + src->pxlrow[i+1][j].grn)/2; 
					reed = (src->pxlrow[i][j].red + src->pxlrow[i+1][j].red)/2;
				}
			}
			else
			{
				if((j+1)<src->cols)
				{
					blue = (src->pxlrow[i][j].blu + src->pxlrow[i][j+1].blu)/2;	
					gree = (src->pxlrow[i][j].grn + src->pxlrow[i][j+1].grn)/2;	
					reed = (src->pxlrow[i][j].red + src->pxlrow[i][j+1].red)/2;	
				}
				else
				{
					blue = src->pxlrow[i][j].blu;
					gree = src->pxlrow[i][j].grn;
					reed = src->pxlrow[i][j].red;
				}
			}

			dst->pxlrow[i/2][j/2].blu = blue;
			dst->pxlrow[i/2][j/2].grn = gree;
			dst->pxlrow[i/2][j/2].red = reed;
		}

        return dst;
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

