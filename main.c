#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
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


  // TODO: LOOP THROUGH FILES TO SEE IF THERE'S A MATCH.
  // TODO: IF THERE'S A MATCH, ASK THE USER TO CHANGE OR NO THE CONTENT.
  printf("String: %s\n", MatchedLine);
  pcreExecRet = pcre_exec(compiledRegexp, optmizedRegexp, MatchedLine, strlen(MatchedLine), 0, 0, NULL, 0);
  if(pcreExecRet < 0) {
    switch(pcreExecRet) {
      case PCRE_ERROR_NOMATCH      : printf("String did not match the pattern\n");        break;
      case PCRE_ERROR_NULL         : printf("Something was null\n");                      break;
      case PCRE_ERROR_BADOPTION    : printf("A bad option was passed\n");                 break;
      case PCRE_ERROR_BADMAGIC     : printf("Magic number bad (compiled re corrupt?)\n"); break;
      case PCRE_ERROR_UNKNOWN_NODE : printf("Something kooky in the compiled re\n");      break;
      case PCRE_ERROR_NOMEMORY     : printf("Ran out of memory\n");                       break;
      default                      : printf("Unknown error\n");                           break;
    }
  } else {
    printf("Result: We have a match!\n");
  };

  pcre_free(compiledRegexp);
  if(optmizedRegexp != NULL) {
    pcre_free_study(optmizedRegexp);
  };
};

