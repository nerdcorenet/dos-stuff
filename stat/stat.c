/* stat.c - A version of stat designed for DOS.
 * Copyright (c) 2010, Mike Mallett
 *
 * This program is designed to display various information
 * about a file, directory, FIFO, character/block device,
 * or filesystem. If the filename given is a root-level path
 * (ex. "C:\", statfs() is used, otherwise stat() is used.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys\stat.h>
#include <sys\vfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

/* Creates a human-readable string from a bytecount
 * Format is "1023.99 kB"
 * Args:
 *   int SIZE - the bytecount
 *   char *STR - the string to write
 * Returns: the number of 1024s SIZE was divided by
 */
int humansize(int size, char *str) {
  char sips[8] = {'k','M','G','T','P','E','Z','Y'}; // kilo, Mega, ...
  char sip = ' '; // SI Prefix
  float h_size = size;
  int ttfs = -1; // Ten-Twnety-Fours
  while (h_size > 1024) {
    h_size /= 1024;
    sip = sips[++ttfs];
  }
  int s_len = sprintf(str, "%.2f %cB", h_size, sip);
  str[s_len] = '\0';
  return (ttfs + 1);
}

/* Print usage summary and exit()
 * cmd is meant to be argv[0]
 */
void usage(char *cmd) {
  printf("Usage: %s [-h] <filename>\n", cmd);
  printf("Prints info for a file, directory, FIFO, volume (root dir), etc.\n\n");
  printf("  -h Prints human-readable sizes (DEFAULT: bytes and human-readable)\n");
  exit(-1);
}

void main (int argc, char *argv[]) {
  // Setup variables
  char *filename;
  char *cmd = argv[0];
  struct stat s;
  unsigned char argt; // For command-line argument processing
  char hflag=0;
  char h_size[11]; // strlen("1023.99 YB\0") == 11

  // Process command-line args
  while (--argc > 0 && (*++argv)[0] == '-')
    while (argt = *++argv[0])
      switch (argt) {
      case 'h': // Display only human-readable format
        hflag = 1; break;
      default:
        printf("ERROR: Invalid argument\n");
        usage(cmd);
        break;
      }
  if (argc != 1) {
    printf("ERROR: Missing filename\n");
    usage(cmd);
  }

  filename = *argv;

  if (stat(filename, &s) != 0) {
    printf("Unable to stat '%s'\n", filename);
    exit(-1);
  }

  if (S_ISDIR(s.st_mode)) {
    if (s.st_ino == 1) {
      printf("stat for filesystem '%c:'\n\n", (s.st_dev + 65));
      printf(" Root-level subdirectories: %u\n", (s.st_nlink - 2));
      printf(" Root-level files: %u\n", ((s.st_size / 32) - (s.st_nlink - 2)));
      struct statfs sfs;
      int statfs_ret = statfs(filename, &sfs);
      if (statfs_ret != 0) {
        printf(" ERROR RETRIEVING FILESYSTEM INFO: %d\n", statfs_ret);
      } else {
        long total = sfs.f_bsize * sfs.f_blocks;
        long free = sfs.f_bsize * sfs.f_bfree;
        long used = total - free;
        // strlen("1023.99 YB\0") == 11
        char total_h[11], free_h[11], used_h[11];
        humansize(total, total_h);
        humansize(free, free_h);
        humansize(used, used_h);
        // call _get_volume_info() for some additional tidbits
        int maxfile, maxdir; // Maximum file/directory length
        char fstype[33]; // 32 bytes is suggested in the libc reference
        _get_volume_info(filename, &maxfile, &maxdir, fstype);
        if (hflag) {
          printf(" Total size: %s\n", total_h);
          printf(" Used: %s\n", used_h);
          printf(" Free: %s\n", free_h);
        } else {
          printf(" Total size: %u bytes (%s)\n", total, total_h);
          printf(" Used: %u bytes (%s)\n", used, used_h);
          printf(" Free: %u bytes (%s)\n", free, free_h);
        }
        //printf(" More Info [?]: 0x%04x\n", sfs.f_fsid[1]);
        printf(" Block Size: %u\n", s.st_blksize);
        printf(" Maximum filename length: %u\n", maxfile);
        printf(" Maximum directory name length: %u\n", maxdir);
        //printf(" FS type: %s\n", fstype);
      }
    } else {
      printf("stat for directory '%s'\n\n", filename);
      printf(" Subdirectories: %u\n", (s.st_nlink - 2));
      printf(" Files: %u\n", ((s.st_size / 32) - (s.st_nlink - 2)));
    }
  } else if (S_ISFIFO(s.st_mode)) {
    printf("stat for FIFO '%s'\n\n", filename);
  } else if (S_ISBLK(s.st_mode)) {
    printf("stat for block device '%s'\n\n", filename);
  } else if (S_ISCHR(s.st_mode)) {
    printf("stat for character device '%s'\n\n", filename);
  } else if (S_ISREG(s.st_mode)) {
    printf("stat for file '%s'\n\n", filename);
    if (hflag) {
      humansize(s.st_size, h_size);
      printf(" Size: %s\n", h_size);
    } else if (s.st_size > 1024) {
      humansize(s.st_size, h_size);
      printf(" Size: %u bytes (%s)\n", s.st_size, h_size);
    } else {
      printf(" Size: %u bytes\n", s.st_size);
    }
  } else {
    printf("stat for '%s'\n FILE HAS UNKNOWN TYPE: 0x%04x\n\n", filename, s.st_mode);
  }

  if (s.st_dev >= 0)
    printf(" Drive: %c:\n", (s.st_dev + 65)); // ASCII 'A' == 65
  else
    printf(" Drive: %d\n", s.st_dev);

  printf(" Modification Time: %s", asctime(localtime(&s.st_mtime))); // asctime() contains '\n\0'
  if (s.st_ctime != s.st_mtime)
    printf(" Creation Time: %s", asctime(localtime(&s.st_ctime)));
  if (s.st_atime != s.st_mtime)
    printf(" Access Time: %s", asctime(localtime(&s.st_atime)));
  printf(" First Cluster: %u\n", s.st_ino);
  //printf(" UID/GID: %u/%u\n", s.st_uid, s.st_gid);
  printf(" Mode: %o\n", s.st_mode);

  exit(1);
}
