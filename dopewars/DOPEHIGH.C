/*
 * Copyright (c) 2012 Mike Mallett
 * 
 * THIS SOFTWARE IS PROVIDED 'AS IS' WITH
 * ABSOLUTELY NO WARRANTY, NOT EVEN THE WARRANTY
 * OF MERCHANTABILITY OR FITNESS FOR A PARTICAULAR
 * PURPOSE. IN NO WAY SHALL THE AUTHOR(S) BE HELD
 * LIABLE FOR ANY DAMAGES WHATSOEVER.
 */

#include <stdio.h>
#include <stdlib.h>
#include <endian.h>

int main (int argc, char *argv[]) {
  // Setup variables
  char name[31];
  name[30] = '\0';
  char date[11];
  date[10] = '\0';
  unsigned int worth;
  unsigned short dead;

  // Open file
  FILE *infile = fopen("dopewars.sco", "rb");
  if (infile == NULL) {
    printf("Unable to open dopewars.sco\n");
    exit(-1);
  }

  printf("            Dealer                Played    Earnings\n");
  printf("----------------------------------------------------\n");

  // The file loop ...
  while (fgets(name,31,infile) != NULL) {
    fread(&worth,4,1,infile);
    fgets(date,11,infile);
    fread(&dead,2,1,infile);

    // Swap endianness if required
    worth = le32toh(worth);
    
    printf("%s  %s  %u",name,date,worth);
    if (dead & 0xffff) printf("  Dead");
    printf("\n");
  }

  fclose(infile);
  exit(1);
}

