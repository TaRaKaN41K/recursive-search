#include <stdio.h>
#include <stdio.h>
#include <getopt.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

int isdebug=0;
char* in_sign;

// Функция для чтения файла и поиска сигнатуры
int read_file(const char* filename, unsigned char* signature, size_t signature_len) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        printf("Error opening file %s\n", filename); // нужно добавить перевод строки \n
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    unsigned char* buffer = malloc(file_size);
    if (!buffer) {
        perror("Error allocating memory");
        fclose(fp);
        return -1;
    }
    fread(buffer, 1, file_size, fp);
    fclose(fp);

    int found = 0;

    if (file_size < signature_len)
    {
        free(buffer);
        return 0;
    }
    
    int i;
    for (i = 0; i <= file_size - signature_len; i++)
    {
        if (memcmp(buffer + i, signature, signature_len) == 0) {
            found = 1;
            break;
        }
    }

    free(buffer);

    if (found>0)
    {
        if (isdebug)
        {
            fprintf(stderr, "found singnature %s in file %s the first match %i - %lu symbols\n", in_sign, filename, i, i+(strlen(in_sign)/2)-2);
        }
        else
        {
            printf("%s \n", filename);
        }
    }
    return found;
}

// Функция для рекурсивного поиска по каталогу
void recursive_search(char *d, unsigned char* signature)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(d);
    if (!dir) {
        perror("opendir");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            char* path = malloc(strlen(d) + strlen(entry->d_name) + 2);
            if (!path)
            {
                perror("Error allocating memory");
                exit(1);
            }
            strcpy(path, d);
            strcat(path, "/");
            strcat(path, entry->d_name);
            recursive_search(path, signature);
        }
        else if(entry->d_type == DT_REG)
        {
            char* name = malloc(strlen(d) + strlen(entry->d_name) + 2);
            if (!name)
            {
                perror("Error allocating memory");
                exit(1);
            }
            strcpy(name, d);
            strcat(name, "/");
            strcat(name, entry->d_name);
            read_file(name, signature, strlen((const char*)signature));
        }
    }

    closedir(dir);
}

// Функция для преобразования строки формата 0x... в последовательность байтов
unsigned char* parseHexString(char* str) {
    int strLen = (int)strlen(str)-2;
    if (strLen % 2 != 0) {
        return NULL; // некорректная длина строки
    }
    unsigned char* bytes = malloc((strLen / 2) * sizeof(unsigned char));
    for (int i = 2; i <= strLen; i += 2) {
        char byteStr[3] = { str[i], str[i + 1], '\0' };
        unsigned long int byteValue = strtol(byteStr, NULL, 16);
        bytes[(i/2)-1] = (unsigned char)byteValue;
    }
    return bytes;
}
    

int main(int argc, char **argv)
{
    char *debug = getenv("LAB11DEBUG");
    if (debug != NULL)
    {
        fprintf(stderr, "Debug mode is on\n");
        isdebug = 1;
    }
    
    if ((argv[1] != NULL) && (argv[2] != NULL))
    {
        char* d = argv[1];
        char* signature_str = argv[2];
        in_sign = argv[2];
        unsigned char* signature = parseHexString(signature_str);
        
        if (signature == NULL) {
            printf("Error: invalid signature string.\n");
        return 1;
        }

    recursive_search(d, signature);
    free(signature);
        return 0;
        
    }
    static struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {NULL, 0, NULL, 0}
    };
    
    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "hv", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                printf("Usage: %s [OPTIONS] catalog the_purpose_of_the_search\n", argv[0]);
                printf("  -h, --help     display this help and exit\n");
                printf("  -v, --version  output version information and exit\n");
                return 0;
            case 'v':
                printf("Program version: 1.0\nFN: Kalashov Feodor\nGroup: N32481\nVar: 2\n");
                return 0;
            case '?':
                break;
            default:
                fprintf(stderr, "Error: unknown option '%c'. Use --help for usage information.\n", optopt);
                return 1;
        }
    }

    return 0;
}
