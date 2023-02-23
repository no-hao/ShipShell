#include "parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_BUF_SIZE 16

/**
 * Strips whitespace from the end of a string.
 *
 * @param s The string to strip.
 */
void strip_trailing_whitespace(char *s) {
  size_t len = strlen(s);
  while (len > 0 && isspace(s[len - 1])) {
    s[--len] = '\0';
  }
}

/**
 * Frees the memory used by a TokenList struct, including the token array.
 *
 * @param list The TokenList struct to free.
 */
void free_token_list(TokenList list) {
  for (size_t i = 0; i < list.num_tokens; i++) {
    free(list.tokens[i]);
  }
  free(list.tokens);
}

/**
 * Tokenizes a string into an array of tokens using the specified delimiter.
 *
 * @param input The input string to tokenize.
 * @param delimiter The delimiter to use for splitting the input string.
 * @return A TokenList struct containing the array of tokens and the number of
 * tokens, or an empty TokenList struct if an error occurs.
 */
TokenList tokenize_input(const char *input, const char *delimiter) {
  size_t max_tokens = 16;
  size_t num_tokens = 0;
  char **tokens = malloc(max_tokens * sizeof(*tokens));

  if (tokens == NULL) {
    perror("Error: failed to allocate memory for tokens");
    return (TokenList){NULL, 0};
  }

  size_t input_len = strlen(input);
  const char *start = input;
  const char *end = input;

  while (*end != '\0') {
    while (*end != '\0' && strchr(delimiter, *end) == NULL) {
      end++;
    }

    if (num_tokens >= max_tokens) {
      max_tokens += TOKEN_BUF_SIZE;
      char **new_tokens = realloc(tokens, max_tokens * sizeof(*tokens));
      if (new_tokens == NULL) {
        perror("Error: failed to reallocate memory for tokens");
        free_token_list((TokenList){tokens, num_tokens});
        return (TokenList){NULL, 0};
      }
      tokens = new_tokens;
    }

    size_t token_len = end - start;
    if (token_len > 0) {
      char *token = malloc(token_len + 1);
      if (token == NULL) {
        perror("Error: failed to allocate memory for token");
        free_token_list((TokenList){tokens, num_tokens});
        return (TokenList){NULL, 0};
      }
      memcpy(token, start, token_len);
      token[token_len] = '\0';
      strip_trailing_whitespace(token);
      tokens[num_tokens++] = token;
    }

    start = ++end;
  }

  TokenList result = {tokens, num_tokens};
  return result;
}
