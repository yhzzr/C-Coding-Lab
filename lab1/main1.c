// Main program file MAIN1.C
// Written by Daniel W. Lewis
// Revised Jan 2, 2015
//
// Purpose: Create a posterized and a negative version of an image.

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "bmp1.h"

unsigned	Brightness(unsigned red, unsigned grn, unsigned blu) ;
IMAGE		*InvertColors(IMAGE *image) ;
IMAGE		*PosterizeImage(IMAGE *image, double percent) ;

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
        {"Invert Colors",		InvertColors,		  0},
        {"Posterize (%.1f)",	PosterizeImage,		  50},
        {NULL,					NULL,				  0}
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
    brightness = 0.2126 * red + 0.7152*grn + 0.0722*blu;
    return brightness;
}

IMAGE *PosterizeImage(IMAGE *image, double percent)
{
    // To be completed by student ...
    unsigned int threshold;
    threshold = 255 * percent / 100;
    unsigned brightness;
    
    unsigned int row = GetRows(image);
    unsigned int cols = GetCols(image);
    
    unsigned red;
    unsigned grn;
    unsigned blu;
    
    for(int i=0; i<row;i++)
    {
        for(int j=0; j<cols;j++)
        {
            red = GetRed(image, i, j);
            grn = GetGrn(image, i, j);
            blu = GetBlu(image, i, j);
            brightness = Brightness(red,grn,blu);
            
            if (brightness>threshold) {
                PutRed(image,i,j,255);
                PutGrn(image,i,j,255);
                PutBlu(image,i,j,255);
            }
            else
            {
                PutRed(image,i,j,0);
                PutGrn(image,i,j,0);
                PutBlu(image,i,j,0);
            }
        }
    }
    
    return image;
}

IMAGE *InvertColors(IMAGE *image)
{
    // To be completed by student ...
    unsigned int row = GetRows(image);
    unsigned int cols = GetCols(image);
    
    unsigned red;
    unsigned grn;
    unsigned blu;
    
    for(int i=0; i<row;i++)
    {
        for(int j=0;j<cols;j++)
        {
            red = 255 - GetRed(image, i, j);
            grn = 255 - GetGrn(image, i, j);
            blu = 255 - GetBlu(image, i, j);
            
            PutRed(image,i,j,red);
            PutGrn(image,i,j,grn);
            PutBlu(image,i,j,blu);
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

