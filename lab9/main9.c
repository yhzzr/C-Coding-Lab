// Main program file MAIN9.C
// Written by Daniel W. Lewis
// Revised Jan 2, 2015
//
// Purpose: Create and manipulate a linked list of audio segments

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "wav.h"

typedef struct LIST_TAG
	{
	struct LIST_TAG	*nextnode ;
	AUDIO			*segment ;
	} LIST ;

LIST *	CreateList(AUDIO *segment1, AUDIO *segment2, AUDIO *segment3) ;
AUDIO *	CombineSegments(LIST *list) ;

void DisplayAudio(char *filespec) ;

int main(int argc, char **argv)
	{
	AUDIO *src, *dst, *part1, *part2, *part3 ;
	char *src_filespec, *dst_filespec ;
	unsigned one_third ;
	LIST *list ;

	if (argc != 3)
		{
		fprintf(stderr, "Usage: %s src-file dst-file\n", argv[0]) ;
		exit(255) ;
		}

	src_filespec = argv[1] ;
	dst_filespec = argv[2] ;

	src = ReadWAV16(src_filespec) ;
	one_third = src->num_samples / 3 ;

	part1 = CopySegment(src, 0, one_third) ;
	part2 = CopySegment(src, one_third + 1, 2*one_third) ;
	part3 = CopySegment(src, 2*one_third+1, src->num_samples - 1) ;

	list  = CreateList(part1, part2, part3) ;
	dst   = CombineSegments(list) ;

	WriteWAV16(dst_filespec, dst) ;
	DisplayAudio(dst_filespec) ;

	return 0 ;
	}

LIST *CreateList(AUDIO *segment1, AUDIO *segment2, AUDIO *segment3)
	{
	// To be completed by student ...
		LIST *temp1;
		LIST *temp2;
		LIST *temp3;
		
		temp1 = (LIST *)malloc(sizeof(LIST));
		temp2 = (LIST *)malloc(sizeof(LIST));
		temp3 = (LIST *)malloc(sizeof(LIST));
		
		
		temp1->segment = segment1;
		temp2->segment = segment2;
		temp3->segment = segment3;
		
		temp3->nextnode = NULL;
		temp2->nextnode = temp3;
		temp1->nextnode = temp2;
		
		return temp1;
	}

AUDIO *CombineSegments(LIST *list)
	{
	// To be completed by student ...
		AUDIO *temp = list->segment;

		LIST *current;
		current = list;
		
		while(current->nextnode != NULL)
		{
			temp = InsertSegment(temp, current->nextnode->segment, temp->num_samples);
			current = current->nextnode;
		}

		return temp;		
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
