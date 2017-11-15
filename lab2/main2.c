// Main program file MAIN2.C
// Written by Daniel W. Lewis
// Revised Jan 2, 2015
//
// Purpose: Create a greyscale and a sepia version of an image.

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "bmp1.h"

unsigned	Brightness(unsigned red, unsigned grn, unsigned blu) ;
IMAGE		*ColorToGreyscale(IMAGE *image) ;
IMAGE		*ColorToSepia(IMAGE *image) ;

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
		{"Greyscale",			ColorToGreyscale,	  0.0},
		{"Sepia",				ColorToSepia,		  0.0},
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

unsigned Brightness(unsigned red, unsigned grn, unsigned blu)
	{
		// To be completed by student ...
		unsigned int brightness;
		brightness = 0.2126 * red + 0.7152 * grn + 0.0722 * blu;
		return brightness;
	}

IMAGE *ColorToGreyscale(IMAGE *image)
	{
		// To be completed by student ...
		unsigned brightness;

		unsigned int row = GetRows(image);
		unsigned int cols = GetCols(image);

		unsigned red;
		unsigned grn;
		unsigned blu;

		for(int i=0; i<row;i++)
		{
			for(int j=0;j<cols;j++)
			{
				red = GetRed(image,i,j);
				grn = GetGrn(image,i,j);
				blu = GetBlu(image,i,j);
				brightness = Brightness(red,grn,blu);
				
				PutRed(image,i,j,brightness);
				PutGrn(image,i,j,brightness);
				PutBlu(image,i,j,brightness);
			}	
		}
		
		return image;
	}

IMAGE *ColorToSepia(IMAGE *image)
	{
		// To be completed by student ...
		unsigned int row = GetRows(image);
		unsigned int cols = GetCols(image);
		
		unsigned oldred;
		unsigned oldgrn;
		unsigned oldblu;

		unsigned newred;
		unsigned newgrn;
		unsigned newblu;

		for(int i=0;i<row;i++)
		{
			for(int j=0;j<cols;j++)
			{
				oldred = GetRed(image,i,j);
				oldgrn = GetGrn(image,i,j);
				oldblu = GetBlu(image,i,j);
				
				newred = 0.393*oldred + 0.769*oldgrn + 0.189*oldblu;
				newgrn = 0.349*oldred + 0.686*oldgrn + 0.168*oldblu;
				newblu = 0.272*oldred + 0.534*oldgrn + 0.131*oldblu;
				
				if(newred<=255)
					PutRed(image,i,j,newred);
				else
					PutRed(image,i,j,255);

				if(newgrn<=255)
					PutGrn(image,i,j,newgrn);
				else
					PutGrn(image,i,j,255);
				
				if(newblu<=255)
					PutBlu(image,i,j,newblu);
				else 
					PutBlu(image,i,j,255);
			}
		}
	
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

