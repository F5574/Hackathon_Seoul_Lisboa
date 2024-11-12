#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct s_inc
{
    char **image_matrix;
    char **message_matrix;
    size_t line_count;
} t_inc;

#define COLOR_RED     "\x1b[31m"
#define COLOR_RESET   "\x1b[0m"

int load_image_matrix(const char *filename, t_inc *inc)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return -1;
    }

    size_t line_count = 0;
    char **matrix = NULL;
    char *line = NULL;
    size_t buffer_size = 0;

    while (getline(&line, &buffer_size, file) != -1) {
        line[strcspn(line, "\n")] = '\0';

        char **temp = realloc(matrix, sizeof(char*) * (line_count + 1));
        if (temp == NULL) {
            perror("Failed to realloc memory");
            for (size_t i = 0; i < line_count; i++) {
                free(matrix[i]);
            }
            free(matrix);
            free(line);
            fclose(file);
            return -1;
        }
        matrix = temp;

        matrix[line_count] = strdup(line);
        if (matrix[line_count] == NULL) {
            perror("Failed to strdup line");
            for (size_t i = 0; i < line_count; i++) {
                free(matrix[i]);
            }
            free(matrix);
            free(line);
            fclose(file);
            return -1;
        }

        line_count++;
    }

    free(line);
    fclose(file);

    inc->image_matrix = matrix;
    inc->line_count = line_count;
    return 0;
}

void free_image_matrix(t_inc *inc)
{
    if (inc->image_matrix) {
        for (size_t i = 0; i < inc->line_count; i++) {
            free(inc->image_matrix[i]);
        }
        free(inc->image_matrix);
        inc->image_matrix = NULL;
        inc->line_count = 0;
    }
}

int allocate_and_initialize_message_matrix(t_inc *inc) {
    inc->message_matrix = malloc(sizeof(char*) * inc->line_count);
    if (inc->message_matrix == NULL) {
        perror("Failed to allocate message_matrix");
        return -1;
    }
    
    for (size_t i = 0; i < inc->line_count; i++) {
        size_t line_length = strlen(inc->image_matrix[i]);
        inc->message_matrix[i] = malloc(line_length + 1);
        if (inc->message_matrix[i] == NULL) {
            perror("Failed to allocate message_matrix line");
            for (size_t j = 0; j < i; j++) {
                free(inc->message_matrix[j]);
            }
            free(inc->message_matrix);
            return -1;
        }
		memset(inc->message_matrix[i], ' ', line_length);
        // memset(inc->message_matrix[i], '#', line_length);
        inc->message_matrix[i][line_length] = '\0';
    }
    return 0;
}

void free_message_matrix(t_inc *inc) {
    if (inc->message_matrix) {
        for (size_t i = 0; i < inc->line_count; i++) {
            free(inc->message_matrix[i]);
        }
        free(inc->message_matrix);
        inc->message_matrix = NULL;
    }
}

void create_filename(char letter, char *filename, size_t size) {
    if (letter == ' ') {
        snprintf(filename, size, "src/letters/sp.txt");
        
    }
    else {
        char uppercase_letter = toupper((unsigned char) letter);
        snprintf(filename, size, "src/letters/%c.txt", uppercase_letter);
    }
}


int get_widest_part_length(FILE *file) {
    char *line = NULL;
    size_t buffer_size = 0;
    size_t max_length = 0;

    while (getline(&line, &buffer_size, file) != -1) {
        line[strcspn(line, "\n")] = '\0';
        size_t line_length = strlen(line);

        if (line_length > max_length) {
            max_length = line_length;
        }
    }

    free(line);
    fseek(file, 0, SEEK_SET);
    return max_length;
}

int get_utf8_char_length(const unsigned char *s) {
    if (s == NULL) return -1;
    unsigned char ch = s[0];
    if (ch <= 0x7F) {
        return 1;
    } else if ((ch & 0xE0) == 0xC0) {
        if ((s[1] & 0xC0) != 0x80) return -1;
        return 2;
    } else if ((ch & 0xF0) == 0xE0) {
        if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80) return -1;
        return 3;
    } else if ((ch & 0xF8) == 0xF0) {
        if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80) return -1;
        return 4;
    } else {
        return -1;
    }
}

int write_single_letter(FILE *file, t_inc *inc, int idx) {
    char *line = NULL;
    size_t buffer_size = 0;
    size_t small_image_line = 0;

    while (getline(&line, &buffer_size, file) != -1) {
        line[strcspn(line, "\n")] = '\0';

        if (small_image_line >= inc->line_count) {
            break;
        }

        char *message_line = inc->message_matrix[small_image_line];

        size_t message_byte_idx = 0;
        size_t message_char_idx = 0;
        while (message_char_idx < idx && message_line[message_byte_idx] != '\0') {
            int char_length = get_utf8_char_length((unsigned char *)&message_line[message_byte_idx]);
            if (char_length == -1) {
                fprintf(stderr, "Invalid UTF-8 sequence in message_line.\n");
                free(line);
                return -1;
            }
            message_byte_idx += char_length;
            message_char_idx++;
        }

        size_t src_pos = 0;
        size_t dest_pos = message_byte_idx;

        while (line[src_pos] != '\0' && message_line[dest_pos] != '\0') {
            int src_char_len = get_utf8_char_length((unsigned char *)&line[src_pos]);
            if (src_char_len == -1) {
                fprintf(stderr, "Invalid UTF-8 sequence in source line.\n");
                free(line);
                return -1;
            }

            size_t message_line_len = strlen(message_line);
            if (dest_pos + src_char_len > message_line_len) {
                break;
            }

            memcpy(&message_line[dest_pos], &line[src_pos], src_char_len);

            src_pos += src_char_len;
            dest_pos += src_char_len;
        }

        small_image_line++;
    }

    free(line);
    return 0;
}

int write_message(unsigned char *input, t_inc *inc) 
{
    int idx = 3;
    char filename[256];
    size_t input_length = strlen((char*)input);

    for (size_t i = 0; i < input_length; i++) 
    {
        create_filename(input[i], filename, sizeof(filename));

        FILE *file = fopen(filename, "r");
        if (!file) {
            fprintf(stderr, "Failed to open file: %s\n", filename);
            return -1;
        }

        int width = get_widest_part_length(file);

        if (idx + width > 200) {
            fclose(file);
            return 1;
        }

        int res = write_single_letter(file, inc, idx);
        if (res != 0) {
            fclose(file);
            return res;
        }

        fclose(file);
        idx += width;
    }
    return 0;
}

void print_combined_matrix(t_inc *inc) {
    for (size_t i = 0; i < inc->line_count; i++) {
        char *image_line = inc->image_matrix[i];
        char *message_line = inc->message_matrix[i];

        size_t img_pos = 0;
        size_t msg_pos = 0;
        size_t img_len = strlen(image_line);
        size_t msg_len = strlen(message_line);

        while (img_pos < img_len && msg_pos < msg_len) {
            int img_char_len = get_utf8_char_length((unsigned char *)&image_line[img_pos]);
            int msg_char_len = get_utf8_char_length((unsigned char *)&message_line[msg_pos]);

            if (img_char_len == -1 || msg_char_len == -1) {
                fprintf(stderr, "Invalid UTF-8 sequence detected during printing.\n");
                break;
            }
			if (strncmp(&message_line[msg_pos], " ", msg_char_len) != 0) {
            // if (strncmp(&message_line[msg_pos], "#", msg_char_len) != 0) {
				// printf(COLOR_RED);
                fwrite(&message_line[msg_pos], 1, msg_char_len, stdout);
				// printf(COLOR_RESET); 
            } else {
                fwrite(&image_line[img_pos], 1, img_char_len, stdout);
            }

            img_pos += img_char_len;
            msg_pos += msg_char_len;
        }

        while (img_pos < img_len) {
            int img_char_len = get_utf8_char_length((unsigned char *)&image_line[img_pos]);
            if (img_char_len == -1) {
                fprintf(stderr, "Invalid UTF-8 sequence detected during printing.\n");
                break;
            }
            fwrite(&image_line[img_pos], 1, img_char_len, stdout);
            img_pos += img_char_len;
        }

        while (msg_pos < msg_len) {
            int msg_char_len = get_utf8_char_length((unsigned char *)&message_line[msg_pos]);
            if (msg_char_len == -1) {
                fprintf(stderr, "Invalid UTF-8 sequence detected during printing.\n");
                break;
            }
            fwrite(&message_line[msg_pos], 1, msg_char_len, stdout);
            msg_pos += msg_char_len;
        }

        putchar('\n');
    }
}

int draw_front_postcard(const char *input, const char *filename) {
	t_inc inc;
    inc.image_matrix = NULL;
    inc.message_matrix = NULL;
    inc.line_count = 0;

    if (load_image_matrix(filename, &inc) != 0) {
        fprintf(stderr, "Error loading image matrix\n");
        return 1;
    }

    if (allocate_and_initialize_message_matrix(&inc) != 0) {
        fprintf(stderr, "Error allocating message matrix\n");
        free_image_matrix(&inc);
        return 1;
    }

    int write_res = write_message(input, &inc);
    if (write_res == 1) 
    {
        fprintf(stderr, "Error: Message width exceeds limit.\n");
        free_message_matrix(&inc);
        free_image_matrix(&inc);
        return 1;
    }
    else if (write_res == -1)
    {
        fprintf(stderr, "Error: Could not process message.\n");
        free_message_matrix(&inc);
        free_image_matrix(&inc);
        return 1;
    }

    print_combined_matrix(&inc);

    free_message_matrix(&inc);
    free_image_matrix(&inc);
	return 0;
}

int draw_back_postcard()
{
		t_inc inc;
    inc.image_matrix = NULL;
    inc.message_matrix = NULL;
    inc.line_count = 0;


  if (load_image_matrix(filename, &inc) != 0) {
        fprintf(stderr, "Error loading image matrix\n");
		return (1);
    }


}


int main()
{
	const char* input = "semi jeong";
    const char* filename = "src/image/gyeongbok_palace";
	const char* filename2 = "src/image/gyeongbok_palace";
    draw_front_postcard(input, filename);
	draw_back_postcard(filename2);
    return 0;
}