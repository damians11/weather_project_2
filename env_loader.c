#include "env_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_env(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        line[strcspn(line, "\n")] = 0;
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "=");
        if (key && value) setenv(key, value, 1);
    }

    fclose(file);
}
