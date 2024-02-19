#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

int array[] = {16, 32, 52, 0,
               1, 2, 3, 0,
               4, 5, 6, 7, 0,
               8, 9, 10, 0};

int fillFileWithArray() {
    int my_file;
    void *my_map;
    my_file = open("file.bin", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);
    if (my_file == -1) {
        return 1;
    }
    lseek(my_file, sizeof(array) - 1, SEEK_SET);
    if (write(my_file, "", 1) == -1) {
        return 2;
    }

    my_map = mmap(0, sizeof(array), PROT_WRITE, MAP_SHARED, my_file, 0);
    if (my_map == MAP_FAILED) {
        return 3;
    }
    close(my_file);
    memcpy(my_map, array, sizeof(array));
    msync(my_map, sizeof(array), MS_SYNC);
    munmap(my_map, sizeof(array));
    return 0;
}

int checkIfArrayExists(const int *map, int array_number) {
    for (int i = 0; i < array_number; i++) {
        if (map[i] == 0) {
            return 0;
        }
    }

    return 1;
}

int printArrayByNumber(int array_number) {
    struct stat s;
    int *my_map;
    int my_file = open("file.bin", O_RDONLY, 0);
    if (my_file == -1) {
        return 1;
    }

    fstat(my_file, &s);
    my_map = (int*) mmap(0, s.st_size, PROT_READ, MAP_SHARED, my_file, 0);
    if (my_map == MAP_FAILED) {
        return 2;
    }

    close(my_file);
    if (!checkIfArrayExists(my_map, array_number)) {
        return 3;
    }

    int i = my_map[array_number] / sizeof((int)0);
    while (my_map[i] != 0) {
        printf("%d ", my_map[i]);
        i++;
    }

    return 0;
}

int main() {
    int fill_result = fillFileWithArray();
    switch (fill_result) {
        case 1:
        {
            printf("Не удалось открыть файл, чтобы записать в него массив\n");
            return 0;
        }
        case 2:
        {
            printf("Не удалось записать массив в файл\n");
            return 0;
        }
        case 3:
        {
            printf("Не удалось создать отображение файла в памяти\n");
            return 0;
        }
        default:
            break;
    }

    int array_number;
    scanf("%d", &array_number);
    int print_result = printArrayByNumber(array_number - 1);
    switch (print_result) {
        case 1:
        {
            printf("Не удалось открыть файл, чтобы считать массив\n");
            return 0;
        }
        case 2:
        {
            printf("Не удалось создать отображение файла в памяти\n");
            return 0;
        }
        case 3:
        {
            printf("В файле нет массива с номером %d\n", array_number);
            return 0;
        }
        default:
            break;
    }

    return 0;
}
