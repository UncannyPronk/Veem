#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char text[100][300]; int line = 0;
int input;
int editlineno;

void new_line() {
    
    printf("Line %d : ", line+1);
    scanf(" %[^\n]", &text[line]);
    // printf("Entered text: %s", text[line]);
    line++;
}

void commands() {
    printf("\n~~~VEEM - The most useless text editor~~~\n\n");
    printf("(0) : Insert new line\n");
    printf("(1) : Display text\n");
    printf("(2) : Edit line\n");
    printf("(3) : Clear File\n");
    printf("(4) : Save File\n");
    printf("(5) : Load File\n");
    printf("(6) : Show commands\n");
    printf("(7) : Quit VEEM\n\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
}

int main() {

    FILE *file;
    char filename[100];
    commands();

    do {
        printf("\n\nEnter command > ");
        scanf("%d", &input);
        switch (input) {
            case 0:
                // printf("New line");
                // printf("\n\n");
                new_line();
                break;
            case 1:
                for (int i=0; i<line; i++) {
                    printf("%s\n", text[i]);
                }
                break;
            case 2:
                // printf("Edit line");
                // printf("\n\n");
                printf("Enter which line to edit: ");
                scanf("%d", &editlineno);
                editlineno--;
                printf("Original text -> %s \n Line %d: ", text[editlineno], editlineno+1);
                scanf(" %[^\n]", &text[editlineno]);
                break;
            case 3:
                // printf("Clear file");
                // printf("\n\n");
                for (int i=0; i<line; i++) {
                    text[i][0] = '\0';
                }
                line = 0;
                break;
            case 4:
                // printf("Save file");
                printf("Enter filename: ");
                scanf("%s", &filename);
                file = fopen(filename, "w");

                if (file == NULL) {
                    printf("Error while saving...");
                } else {
                    for (int i=0; i<line; i++) {
                        fprintf(file, "%s\n", text[i]);
                    }
                }
                fclose(file);
                printf("File saved successfuly");
                // printf("\n\n");
                break;
            case 5:
                // printf("Load line");
                // printf("\n\n");
                printf("Enter filename: ");
                scanf("%s", &filename);
                file = fopen(filename, "r");
                if (file == NULL) {
                    printf("Error loading file.\n");
                } else {
                    line = 0;
                    while (fgets(text[line], sizeof(text[line]), file)) {
                        text[line][strcspn(text[line], "\n")] = '\0';
                        line++;
                        if (line >= 100) break;
                    }
                }
                break;
            case 6:
                commands();
                break;
            default:
                printf("\nInvalid command. Try again.\n\n");
                break;
        }
    } while (input != 7);

    return 0;
}
