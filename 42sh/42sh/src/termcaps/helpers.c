
#include "../../lib/termcaps.h"
#include <ctype.h>


t_token_position *new_token_position(char *token, int start_position)
{
    t_token_position *position;

    position = (t_token_position *)malloc(sizeof(t_token_position));
    position->token = strdup(token);
    position->start_position = start_position;
    position->end_position = start_position + strlen(token);
    position->next = NULL;
    position->prev = NULL;
    return position;
}

void add_token_position(t_token_position **positions, char *token, int start_position)
{
    t_token_position *traverse;

    traverse = *positions;
    while (traverse->next)
        traverse = traverse->next;

    traverse->next = new_token_position(token, start_position);
    traverse->next->prev = traverse;
}

t_token_position *create_token_list(char buffer[])
{
    t_token_position *position;
    int i;
    char *string;
    int this_position;
    int *this_token;

    position = NULL;
    string = NULL;
    i = 0;
    while (buffer[i])
    {
        if (!isspace(buffer[i]))
            string = charpush(string, buffer[i]);
        else 
        {
            if (string != NULL)
            {
                add_token_position(&position, string, i);
                free_and_null(&string);
            }
        }
        i++;
    }
    if (string != NULL)
    {
        add_token_position(&position, string, i);
        free_and_null(&string);
    }
    return position;
}

char     *check_token_position(char buffer[], int cursor_position)
{
    t_token_position *position_list;
    t_token_position *traverse;
    int i;

    position_list = create_token_list(buffer);
    traverse = position_list;
    i = 0;
    while (traverse)
    {
        // if (traverse->start_position <= cursor_position && traverse->end_position >= cursor_position)
        //     return strdup(traverse->token);
        
        // else if (traverse->prev && traverse->next && (cursor_position >= traverse->prev->end_position && cursor_position <= traverse->next->start_position))
        //     break ;
        
        traverse = traverse->next;
    }

    return NULL;
}

char *extract_word_at_position(char *str, int position) {
    if (str == NULL || position < 0 || position > strlen(str)) {
        return NULL;  // Return NULL if the input is invalid or out of bounds
    }

    if (isspace(str[position])) {
        return NULL;  // Return NULL if the position is a whitespace or at the end
    }
    if (position == strlen(str))
    {
        position--;
    }

    // Find the start of the word
    int start = position;
    while (start > 0 && !isspace(str[start - 1])) {
        start--;
    }

    // Find the end of the word
    int end = position;
    while (end < strlen(str) && !isspace(str[end])) {
        end++;
    }

    // Extract the word
    int word_len = end - start;
    char *word = (char *)malloc(word_len + 1);
    if (word == NULL) {
        return NULL;  // Return NULL if memory allocation fails
    }

    strncpy(word, str + start, word_len);
    word[word_len] = '\0';  // Null-terminate the string

    return word;
}

// Function to replace or insert a word at a specific cursor position
char *replace_word_at_position(char *str, int position, char *replacement) {
    if (str == NULL || replacement == NULL || position < 0 || position > strlen(str)) {
        return NULL;  // Return NULL if the input is invalid or out of bounds
    }

    int start = position;
    int end = position;
    char *word_to_replace = extract_word_at_position(str, position);

    // If no word is found, we are on a whitespace or at the end, so insert the new word
    if (word_to_replace == NULL) {
        // If cursor is at the end of the string, append the word
        if (position == strlen(str)) {
            int new_len = strlen(str) + strlen(replacement) + 2;  // +1 for space, +1 for null terminator
            char *new_str = (char *)malloc(new_len);
            if (new_str == NULL) {
                return NULL;  // Handle memory allocation failure
            }

            strcpy(new_str, str);
            strcat(new_str, " ");
            strcat(new_str, replacement);

            return new_str;
        }

        // Move start to the first non-whitespace character after the current position
        while (start > 0 && isspace(str[start - 1])) {
            start--;
        }

        // Calculate the new length needed for the buffer
        int original_len = strlen(str);
        int replacement_len = strlen(replacement);
        int new_len = original_len + replacement_len + 1;  // +1 for the additional space

        // Allocate a new string to hold the result
        char *new_str = (char *)malloc(new_len + 1);
        if (new_str == NULL) {
            return NULL;  // Return NULL if memory allocation fails
        }

        // Copy the part before the insertion point
        strncpy(new_str, str, start);
        new_str[start] = '\0';

        // Append the replacement word and a space
        strcat(new_str, replacement);
        strcat(new_str, " ");

        // Append the part after the insertion point
        strcat(new_str, str + start);

        return new_str;
    }

    // Find the start of the word
    while (start > 0 && !isspace(str[start - 1])) {
        start--;
    }

    // Find the end of the word
    while (end < strlen(str) && !isspace(str[end])) {
        end++;
    }

    int original_len = strlen(str);
    int word_len = strlen(word_to_replace);
    int replacement_len = strlen(replacement);
    int new_len = original_len - word_len + replacement_len;

    // Allocate a new string to hold the result
    char *new_str = (char *)malloc(new_len + 1);
    if (new_str == NULL) {
        free(word_to_replace);
        return NULL;  // Return NULL if memory allocation fails
    }

    // Copy the part before the word
    strncpy(new_str, str, start);
    new_str[start] = '\0';

    // Append the replacement word
    strcat(new_str, replacement);

    // Append the part after the word
    strcat(new_str, str + end);

    // Free the extracted word
    free(word_to_replace);

    return new_str;
}

t_autocomm_str  *new_autocomm_string(char *string, int position)
{
    t_autocomm_str *new_string;

    new_string = (t_autocomm_str *)malloc(sizeof(t_autocomm_str));
    new_string->buffer = strdup(string);
    new_string->cursor_position = position;
    new_string->next = NULL;
    return new_string;
}

void    add_autocomm_string(t_autocomm_str **head, char *string, int position)
{
    t_autocomm_str *traverse;

    traverse = *head;
    while (traverse->next)
        traverse = traverse->next;

    traverse->next = new_autocomm_string(string, position);
}

void    push_command(t_autocomm_str **head, char *string, int position)
{
    if (*head == NULL)
        *head = new_autocomm_string(string, position);
    else 
        add_autocomm_string(head, string, position);
}

t_autocomm_str *fetch_at_index(t_autocomm_str *head, int index)
{
    t_autocomm_str *traverse;
    int position;

    position = 0;
    traverse = head;
    while (traverse && position < index)
    { 
        traverse = traverse->next;
        position++;
    }

    return traverse;
}

void    free_commands(t_autocomm_str *head)
{
    t_autocomm_str *traverse;
    t_autocomm_str *prev;   

    traverse = head;
    while (traverse)
    {
        prev = traverse;
        traverse = traverse->next;
        free(prev->buffer);
        free(prev);
    }
}