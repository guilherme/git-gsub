#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

// USAGE:
// git gsub [options] pattern new-value

int main(int argc, char *argv[]) {

  if(argc < 3) {
    printf("Usage: git-gsub [options] pattern VALUE\n");
    exit(1);
  };



  const char *regExp = argv[1];
  const char *pcreErrorStr;
  int errorOffset;

  char *substitute_word = argv[2];
  int substitute_word_size = strlen(substitute_word);

  int numberOfMatchesInLine;

  pcre *compiledRegexp = pcre_compile(regExp, 0, &pcreErrorStr, &errorOffset, NULL);

  if(compiledRegexp == NULL) {
    printf("ERROR: Could not compile '%s': %s\n", regExp, pcreErrorStr);
    exit(1);
  };

  pcre_extra *optmizedRegexp = pcre_study(compiledRegexp, 0, &pcreErrorStr);

  if(pcreErrorStr != NULL) {
    printf("ERROR: Could not study '%s': %s\n", regExp, pcreErrorStr);
    exit(1);
  };

  DIR           *d;
  FILE         *fp;
  char *line = NULL;
  char choice;
  int lines;
  size_t len = 0;
  ssize_t read;
  struct dirent *dir;
  d = opendir(".");
  int ovector[30]; // TODO WHAT IF THERE ARE MORE THAN 30 occurrences?
  // TODO: find also in subdirectory. Stacks could be a good data structure to do it without using recursion.
  // TODO: Put Concurrency.
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      switch(dir->d_type) {
        case DT_BLK :  break; // BLOCK DEVICE
        case DT_DIR :  break; // DIRECTORY
        case DT_REG : // REGULAR FILE
          line = NULL;
          lines = 0;
          len = 0;
          choice = '\0';
          fp = fopen(dir->d_name, "r+");
          if (fp == NULL)
              exit(EXIT_FAILURE);
          while ((read = getline(&line, &len, fp)) != -1) {
            lines++;
            numberOfMatchesInLine = pcre_exec(compiledRegexp, optmizedRegexp, line, strlen(line), 0, 0, ovector, 30);
            if(numberOfMatchesInLine < 0) {
              switch(numberOfMatchesInLine) {
                case PCRE_ERROR_NOMATCH      : break; // didn't match the pattern
                case PCRE_ERROR_NULL         : break; // Something was null
                case PCRE_ERROR_BADOPTION    : break; // Bad option passed
                case PCRE_ERROR_BADMAGIC     : break; // Magic number bad.
                case PCRE_ERROR_UNKNOWN_NODE : break; // Something kooky in the compiled re
                case PCRE_ERROR_NOMEMORY     : break; // out of memory
                default                      : break; // Unknown error
              }
            } else {
              int match_offset_start = 0;
              int match_offset_end = 0;
              int matched_word_size;
              int newLineSize;

              for(int i = 0; i < numberOfMatchesInLine; i++) {
                match_offset_start += ovector[2*i];
                match_offset_end += ovector[2*i+1];

                printf("%s:%d\n", (const char *)dir->d_name, lines);
                printf("-%s", line);
                printf("+%s", line); // TODO: DISCOVER HOW TO SUBSTITUTE
                printf("change?[yN]\n");
                scanf("%c", &choice);
                if(choice == 'y' || choice == 'Y') {
                  matched_word_size = (match_offset_end - match_offset_start);
                  newLineSize = read - matched_word_size + substitute_word_size;

                  printf("matched_word_size: %d\n", matched_word_size);

                  char *newLine = malloc(newLineSize*sizeof(char));
                  char *lineWalker = line;
                  int lineOffset = 0;
                  int substituteWordOffset = 0;
                  char *copyFrom = line;
                  // TODO IMPROVE THIS. THIS IS SHITTY.
                  while(*lineWalker) {
                    if (lineOffset == match_offset_start) {
                      copyFrom = substitute_word;
                    }
                    if (*substitute_word == '\0') {
                      copyFrom = line+lineOffset;
                    };
                    *newLine = *copyFrom;
                    lineWalker++;
                    lineOffset++;
                  };
                  printf("new line: %s\n", newLine);

                  fseek(fp, -read, SEEK_CUR);
                  fputs(newLine, fp);
                  free(newLine);
                } else {
                  printf("false?\n");
                }
              }
            };
          }
          fclose(fp);
          if (line)
            free(line);
          break;
      };
    }
    closedir(d);
  };

  pcre_free(compiledRegexp);
  if(optmizedRegexp != NULL) {
    pcre_free_study(optmizedRegexp);
  };

};

