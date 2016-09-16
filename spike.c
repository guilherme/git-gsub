#include <pcre.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define VARIABLES_SIZES 255
#define OVECCOUNT 30

char *gsub_pattern(const char *user_input, const char *pattern, const char *new_value) {
  int i;
  int error_offset;
  int pcre_exec_ret;
  int ovector[OVECCOUNT];

  int new_value_size;

  const char *pcre_error_str;

  char *new_string = malloc(sizeof(char *)*VARIABLES_SIZES);

  if(new_string == NULL) {
    printf("Something went wrong allocating memory! %s\n", strerror(errno));
    exit(1);
  };
  memset(new_string, 0, VARIABLES_SIZES);

  pcre *compiled_regexp = pcre_compile(pattern, 0, &pcre_error_str, &error_offset, NULL);

  if(compiled_regexp == NULL) {
    printf("ERROR: Could not compile '%s': %s\n", pattern, pcre_error_str);
    exit(1);
  };

  pcre_extra *optmized_regexp = pcre_study(compiled_regexp, 0, &pcre_error_str);

  if(pcre_error_str != NULL) {
    printf("ERROR: Could not study '%s': %s\n", pattern, pcre_error_str);
    exit(1);
  };

  pcre_exec_ret = pcre_exec(compiled_regexp, optmized_regexp, user_input, strlen(user_input), 0, 0, ovector, OVECCOUNT);

  if(pcre_exec_ret < 0) {
    switch(pcre_exec_ret) {
      case PCRE_ERROR_NOMATCH      :
        printf("does nt match\n");
        break; // didn't match the pattern
      case PCRE_ERROR_NULL         : break; // Something was null
      case PCRE_ERROR_BADOPTION    : break; // Bad option passed
      case PCRE_ERROR_BADMAGIC     : break; // Magic number bad.
      case PCRE_ERROR_UNKNOWN_NODE : break; // Something kooky in the compiled re
      case PCRE_ERROR_NOMEMORY     : break; // out of memory
      default                      : break; // Unknown error
    }
  } else {
    new_value_size = strlen(new_value);
    int substring_start_index;
    int substring_length;
    int user_input_size = strlen(user_input);

    char *copy_from = (char *)user_input;
    char *copy_to = new_string;

    int copied_offset = 0;

    for (i = 0; i < pcre_exec_ret; i++)
    {
      substring_start_index = ovector[2*i];
      substring_length = ovector[2*i+1] - ovector[2*i];


      /* copy everything before the found pattern to new string */
      memcpy(copy_to, copy_from, substring_start_index - copied_offset);
      copy_from += (substring_length + substring_start_index) - copied_offset;
      copy_to   += substring_start_index - copied_offset;

      /* copy the new value in the place of the substituted value */
      memcpy(copy_to, new_value, new_value_size);
      copy_to += new_value_size;

      copied_offset = substring_start_index + substring_length;
    }
    memcpy(copy_to, copy_from, user_input_size - copied_offset);
  };
  free(compiled_regexp);
  free(optmized_regexp);
  return new_string;
};

int main() {
   char *user_input;
   char *pattern;
   char *new_value;

   user_input = malloc(sizeof(char *)*VARIABLES_SIZES);

   if(user_input == NULL) {
     printf("Something went wrong allocating memory! %s\n", strerror(errno));
     exit(1);
   };

   pattern = malloc(sizeof(char *)*VARIABLES_SIZES);

   if(pattern == NULL) {
     printf("Something went wrong allocating memory! %s\n", strerror(errno));
     exit(1);
   };

   new_value = malloc(sizeof(char *)*VARIABLES_SIZES);

   if(new_value == NULL) {
     printf("Something went wrong allocating memory! %s\n", strerror(errno));
     exit(1);
   };

   printf("Type the value:");
   fgets(user_input, VARIABLES_SIZES, stdin);
   if ((strlen(user_input)>0) && (user_input[strlen (user_input) - 1] == '\n'))
     user_input[strlen(user_input) - 1] = '\0';

   printf("Pattern to be substituted:");
   fgets(pattern, VARIABLES_SIZES, stdin);

   if ((strlen(pattern)>0) && (pattern[strlen (pattern) - 1] == '\n'))
     pattern[strlen(pattern) - 1] = '\0';

   printf("Value to substitute:");
   fgets(new_value, VARIABLES_SIZES, stdin);

   if ((strlen(new_value)>0) && (new_value[strlen (new_value) - 1] == '\n'))
     new_value[strlen(new_value) - 1] = '\0';

   char *substituted_user_input = gsub_pattern(user_input, pattern, new_value);
   printf("Substitution %s\n", substituted_user_input);

   free(substituted_user_input);
   free(user_input);
   free(pattern);
   free(new_value);
};
