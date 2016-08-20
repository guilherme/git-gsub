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
  const char *MatchedLine = argv[2];
  const char *pcreErrorStr;
  int errorOffset;

  int pcreExecRet;

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
            pcreExecRet = pcre_exec(compiledRegexp, optmizedRegexp, line, strlen(line), 0, 0, NULL, 0);
            if(pcreExecRet < 0) {
              switch(pcreExecRet) {
                case PCRE_ERROR_NOMATCH      : break; // didn't match the pattern
                case PCRE_ERROR_NULL         : break; // Something was null
                case PCRE_ERROR_BADOPTION    : break; // Bad option passed
                case PCRE_ERROR_BADMAGIC     : break; // Magic number bad.
                case PCRE_ERROR_UNKNOWN_NODE : break; // Something kooky in the compiled re
                case PCRE_ERROR_NOMEMORY     : break; // out of memory
                default                      : break; // Unknown error
              }
            } else {
              printf("%s:%d\n", (const char *)dir->d_name, lines);
              printf("-%s", line);
              printf("+%s", line); // TODO: DISCOVER HOW TO SUBSTITUTE
              printf("change?[yN]\n");
              scanf("%c", &choice);
              if(choice == 'y' || choice == 'Y') {
                // TODO: SUBSTITUTE IN FILE.
                // PROBABLY: fseek the (fp - read) position and call write in the file, with the substituted line.
              };
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

