// Main program file MAIN4.C
// Written by Daniel W. Lewis
// Revised Jan 2, 2015
//
// Purpose: Change the brightness and contrast of an image.

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "bmp2.h"

IMAGE		*AdjustBrightness(IMAGE *image, double percent) ;
IMAGE		*AdjustContrast(IMAGE *image, double percent) ;

typedef struct
	{
	char	*format ;
	IMAGE	*(*function)() ;
	double	param ;
	} OPTION ;

OPTION		*GetOption(int argc, char **argv, OPTION options[]) ;
void		DisplayImage(char *filespec, char *format, ...) ;

int main(int argc, char **argv)
	{
	char *src_filespec, *dst_filespec ;
	static OPTION options[] =
		{
		{"Brightness (%.1f)",	AdjustBrightness,	  120},
		{"Contrast (%.1f)",		AdjustContrast,		  150},
		{NULL,					NULL,				    0}
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

IMAGE *AdjustBrightness(IMAGE *image, double percent)
	{
	// To be completed by student ...
		if(percent>0)
		{
			unsigned row, col, red, grn, blu;

			row = GetRows(image);
			col = GetCols(image);

			int i,j;
			for(i=0;i<row;i++)
				for(j=0;j<col;j++)
				{		
					GetRGB(image,i,j,&red,&grn,&blu);
					
						red =((red+(255 * (percent - 100)/100))<255)?(red + (255*(percent - 100)/100)):255; 
						grn =((grn+(255 * (percent - 100)/100))<255)?(grn + (255*(percent - 100)/100)):255;
						blu =((blu+(255 * (percent - 100)/100))<255)?(blu + (255*(percent - 100)/100)):255;
			
					PutRGB(image,i,j,red,grn,blu);
				}
		}
		else
			printf("The percentage should be positive!\n");

		return image;
	}

IMAGE *AdjustContrast(IMAGE *image, double percent)
	{
	// To be completed by student ...

		if(percent>0)
		{
			unsigned row,col,red,grn,blu;

			row = GetRows(image);
			col = GetCols(image);

			int i, j;
			
			double temp1,temp2,temp3;
			
			for(i=0;i<row;i++)
			{
				for(j=0;j<col;j++)
				{
					GetRGB(image,i,j,&red,&grn,&blu);
					
					temp1 = ((double)(red)-128.0)*(double)(percent)/100.0;
					temp2 = ((double)(grn)-128.0)*(double)(percent)/100.0;
					temp3 = ((double)(blu)-128.0)*(double)(percent)/100.0;

					red = (unsigned)(((128.0+temp1)<0.0)?0.0:(128.0+temp1));
					grn = (unsigned)(((128.0+temp2)<0.0)?0.0:(128.0+temp2));
					blu = (unsigned)(((128.0+temp3)<0.0)?0.0:(128.0+temp3));

					if(red>255)
						red = 255;
					if(grn>255)
						grn = 255;
					if(blu>255)
						blu = 255;	
						
					PutRGB(image,i,j,red,grn,blu);
				}
			}
		}
		else 
			printf("The percentage should be positive!\n");

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

