// Main program file MAIN10.C
// Written by Daniel W. Lewis
// Revised Jan 2, 2015
//
// Purpose: Remove intervals of silence from an audio recording

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <sys/stat.h>
#include "wav.h"

AUDIO	*RemoveSilence(AUDIO *audio, unsigned max_level, unsigned min_samples) ;
void	DisplayAudio(char *filespec) ;

int main(int argc, char **argv)
	{
	unsigned max_level = 200 ;
	unsigned min_samples = 2000 ;
	char *srcfile, *dstfile ;
	int arg, error = 0 ;
	AUDIO *audio ;

	switch (argc)
		{
		case 5:
			if (sscanf(argv[4], "%d", &arg) != 1 || arg <= 0) error = 1 ;
			else min_samples = (unsigned) arg ;
		case 4:
			if (sscanf(argv[3], "%d", &arg) != 1 || arg <= 0 || arg > INT16_MAX) error = 1 ;
			else max_level = (unsigned) arg ;
		case 3:
			srcfile = argv[1] ;
			dstfile = argv[2] ;
			break ;
		default:
			error = 1 ;
			break ;
		}

	if (error != 0)
		{
		fprintf(stderr, "\n") ;
		fprintf(stderr, "      Usage: %s src-file dst-file {max-level {min-samples}}\n\n", argv[0]) ;
		fprintf(stderr, "  max-level: Range: 0 to %-10u (default=%u)\n", INT16_MAX, max_level) ;
		fprintf(stderr, "min-samples: Range: 0 to %-10u (default=%u)\n", INT32_MAX, min_samples) ;
		exit(255) ;
		}

	audio = ReadWAV16(srcfile) ;
	audio = RemoveSilence(audio, max_level, min_samples) ;
	WriteWAV16(dstfile, audio) ;

	FreeAudio(audio) ;
	DisplayAudio(dstfile) ;

	return 0 ;
	}

AUDIO *RemoveSilence(AUDIO *audio, unsigned max_level, unsigned min_samples)
	{
	// To be completed by student ...
		unsigned index;
		unsigned end_file = audio->num_samples;
		unsigned start = 0;
		unsigned end = 0;
		
		while(index < end_file)
		{
			if((abs(audio->samples[index])) > max_level)
			{
				end = index;
				if((end-start) >= min_samples)
				{
					audio = DeleteSegment(audio,start,end);
					index = index - (end-start);
					end_file = end_file - (end-start);
				}
				start = index+1;
			}

			index++;
		}
		return audio;
	}

void DisplayAudio(char *filespec)
	{
	char progspec[200], command[1000], *program_files ;
	struct stat filestat ;

#if defined(_WIN32)
	program_files = getenv("ProgramFiles(x86)") ;
	if (program_files == NULL) program_files = getenv("C:\\Program Files") ;
	if (program_files == NULL) return ;

	sprintf(progspec, "%s\\Audacity\\audacity.exe", program_files) ;
	if (stat(progspec, &filestat) != 0) return ;
	if ((filestat.st_mode & S_IFREG) == 0) return ;

	sprintf(command, "\"%s\" %s", progspec, filespec) ;
#elif defined(__unix)
	sprintf(command, "audacity \"%s\"", filespec) ;
#elif defined(__APPLE__)
	sprintf(command, "/Applications/Audacity.app/Contents/MacOS/Audacity \"%s\"", filespec) ;
#endif 
	system(command) ;
	}

