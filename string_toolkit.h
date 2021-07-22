#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define st_is_alnum(t) \
    (t > 47 && t < 58) || (t > 96 && t < 123) || (t > 64 && t < 91) || t == 95
#define st_is_num(t) \
    t > 47 && t < 58

#ifndef __cplusplus
#define bool   unsigned char
#define true   1
#define false  0
#endif

char * st_get_content_no_flags(const unsigned int argc, char ** argv) {
    if (argc < 2 || (strlen(argv[0]) > 2 && argv[0][0] == '-' && argv[0][1] == '-'))
        return NULL;
    
    unsigned int * indices = malloc(argc * sizeof(unsigned int));
    unsigned int indices_length = 0;
    unsigned int size = 0;
    
    for (unsigned int i = 1; i < argc; i++) {
        if (strlen(argv[i]) > 2 && argv[i][0] == '-' && argv[i][1] == '-')
            continue;
        
        indices[indices_length] = i;
        indices_length++;
        
        size += (strlen(argv[i]) + 1) * sizeof(char);
    }
    
    if (!size) {
        free(indices);
        return NULL;
    }
    
    char * ptr = malloc(size);
    ptr[(size / sizeof(char)) - 1] = '\0';
    
    unsigned int index = 0;
    for (unsigned int s = 0; s < indices_length; s++) {
        for (unsigned int j = 0; j < strlen(argv[indices[s]]); j++) {
            ptr[index] = argv[indices[s]][j];
            index++;
        }
        
        if (s != (indices_length - 1)) {
            ptr[index] = ' ';
            index++;
        }
    }
    
    free(indices);
    return ptr;
}

char * st_get_content_no_options(const unsigned int argc, char ** argv) {
    if (argc < 2 || (strlen(argv[0]) > 2 && argv[0][0] == '-' && argv[0][1] == '-'))
        return NULL;
    
    unsigned int end = argc;
    unsigned int size = 0;
    for (unsigned int i = 1; i < argc; i++) {
        if (strlen(argv[i]) > 2 && argv[i][0] == '-' && argv[i][1] == '-') {
            end = i;
            break;
        }
        
        size += (strlen(argv[i]) + 1) * sizeof(char);
    }
    
    char * ptr = malloc(size);
    ptr[(size / sizeof(char)) - 1] = '\0';
    
    unsigned int index = 0;
    for (unsigned int start = 1; start < end; start++) {
        for (unsigned int j = 0; j < strlen(argv[start]); j++) {
            ptr[index] = argv[start][j];
            index++;
        }
        
        if (start != (end - 1)) {
            ptr[index] = ' ';
            index++;
        }
    }
    
    return ptr;
}

typedef struct {
    char * value;
    const bool found;
    const bool has_value;
} st_flag_data;

st_flag_data st_flag_get_value(const unsigned int argc, char ** argv, const char * key) {
    if (!argc) {
        const st_flag_data output = { NULL, 0, 0 };
        return output;
    }

    char * key_ptr = malloc((2 + strlen(key)) * sizeof(char));
    memset(key_ptr, '-', 2 * sizeof(char));
    
    for (unsigned int i = 0; i < strlen(key); i++)
        key_ptr[i + 2] = key[i];
    key_ptr[strlen(key) + 2] = '\0';
    
    bool found = false;
    
    unsigned int start = 0;
    unsigned int end = argc;
    unsigned int size = 0;
    
    for (unsigned int i = 0; i < argc; i++) {
        if (!found && !strcmp(argv[i], key_ptr)) {
            start = i + 1;
            found = true;
            continue;
        }
        
        if (found) {
            if (strlen(argv[i]) > 2 && argv[i][0] == '-' && argv[i][1] == '-') {
                end = i;
                break;
            }
            
            size += (1 + strlen(argv[i])) * sizeof(char);
        }
    }
    
    st_flag_data output = { NULL, found, size != 0 };
    
    if (!found)
        return output;
    
    if (size) {
        output.value = malloc(size);
        output.value[size / sizeof(char)] = '\0';
        
        unsigned int index = 0;
        for (; start != end; start++) {
            for (unsigned int j = 0; j < strlen(argv[start]); j++) {
                output.value[index] = argv[start][j];
                index++;
            }
            
            if (start != (end - 1)) {
                output.value[index] = ' ';
                index++;
            }
        }
    }
    
    return output;
}

char * st_shorten(const char * string, const unsigned int limit, const char * placeholder) {
    if (strlen(string) <= limit)
        return NULL;
    
    const unsigned int placeholder_length = placeholder == NULL ? 3 : strlen(placeholder);
    char * ptr = malloc((1 + limit + placeholder_length) * sizeof(char));
    for (unsigned int i = 0; i < limit; i++)
        ptr[i] = string[i];
    
    if (placeholder != NULL) {
        for (unsigned int i = 0; i < placeholder_length; i++)
            ptr[placeholder_length + i] = placeholder[i];
    } else
        memset(ptr + limit, '.', 3 * sizeof(char));
    
    ptr[limit + placeholder_length] = '\0';
    return ptr;
}

// like strcat but you don't have to include a fixed string length
char * st_dynamic_concat(const unsigned int amount, ...) {    
    unsigned int bytes = 1 * sizeof(char);
    va_list vl1;
    va_start(vl1, amount);
    
    for (unsigned int i = 0; i < amount; i++) {
        const char * arg = va_arg(vl1, const char *);
        bytes += strlen(arg) * sizeof(char);
    }
    
    va_end(vl1);
    
    char * ptr = malloc(bytes);
    
    va_list vl2;
    va_start(vl2, amount);
    
    unsigned int index = 0;
    for (unsigned int i = 0; i < amount; i++) {
        const char * arg = va_arg(vl2, const char *);
        const unsigned int argl = strlen(arg);
        
        for (unsigned int j = index; j < index + argl; j++)
            ptr[j] = arg[j - index];
        index += argl;
    }
    
    va_end(vl2);
    ptr[index] = '\0';
    return ptr;
}

bool st_has_custom_emoji(char text[]) {
    unsigned char s = 0; // status
    unsigned char nc = 0; // number count
    bool ia = false; // is animated
    
    for (unsigned int i = 0; i < strlen(text); i++) {
        if (!s && text[i] == '<')
            s++;
        else if (s == 1) {
            if (text[i] == 'a' && !ia) {
                ia = true;
                continue;
            } else if ((ia && text[i] == 'a') || text[i] != ':') return false;
            s++;
        } else if (s == 2) {
            if (st_is_alnum(text[i]))
                nc++;
            else {
                if (text[i] != ':' || nc < 2 || nc > 32)
                    return false;
                nc = 0;
                s++;
            }
        } else if (s == 3) {
            if (st_is_num(text[i])) 
                nc++;
            else
                return text[i] == '>' && nc > 16 && nc < 20;
        }
    }
    
    return false;
}

void st_proper_case(char text[]) {
    bool s = false;
    unsigned int i = 0;
    
    if (text[0] != ' ') {
        text[0] = toupper(text[0]);
        i++;
    }
    
    for (; i < strlen(text); i++) {
        if (text[i] == ' ') {
            s = true;
            continue;
        } else if (s) {
            text[i] = toupper(text[i]);
            s = false;
        } else
            text[i] = tolower(text[i]);
    }
}

void st_mock(char text[]) {
    for (unsigned int i = 0; i < strlen(text); i += 2)
        text[i] = toupper(text[i]);
}

void st_scramble(char text[]) {
    const unsigned int length = strlen(text);
    if (length == 1)
        return;
    
    for (unsigned int i = 0; i < length - 1; i++) {
        const unsigned int j = rand() % length;
        const char temp = text[j];
        text[j] = text[i];
        text[i] = temp;
    }
}

char * st_progress_bar(
    const double in_total, const double total,
    const char elapsed_char, const char progress_char,
    const char empty_char, const unsigned int bar_length
) {
    const unsigned int available = floor((in_total / total) * bar_length);
    const unsigned int remaining_length = bar_length - (available + (available == bar_length ? 0 : 1));

    char * result = malloc((1 * sizeof(char)) + (available * sizeof(char)) + (remaining_length * sizeof(char)));
    result[sizeof(result) / sizeof(char)] = '\0';
    
    memset(result, elapsed_char, available * sizeof(char));
    
    if (remaining_length) {
        memset(result + available, progress_char, sizeof(char));
        memset(result + available + 1, empty_char, remaining_length * sizeof(char));
    }
    
    return result;
}

char * st_to_abbreviation(const char * text) {
    char * result = malloc(sizeof(text));
    unsigned char s = 0;
    unsigned int length = 0;
    
    if (text[0] != ' ') {
        result[0] = text[0];
        length++;
    }
    
    for (unsigned int i = 0; i < strlen(text); i++) {
        if (text[i] == ' ') {
            s = 1;
            continue;
        } else if (s) {
            result[length] = text[i];
            length++;
        }
        
        s = 0;
    }
    
    result[length] = '\0';
    return result;
}