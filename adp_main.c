#include "adp_player.h"

//#include <ncurses.h> // main.h로 이동
//#include <locale.h> // 한글
#include <string.h>
//#include <stdlib.h>  -화진-
//#include <stdio.h>
#include <dirent.h> // 파일가져올때 fopen

void ffmpeg_linking_check(){
    // ffmpeg 라이브러리의 링크를 확인하는 함수
    unsigned version = avcodec_version();
    unsigned major = (version >> 16) & 0xFF;
    unsigned minor = (version >> 8) & 0xFF;
    unsigned micro = version & 0xFF;

    printf("\r\nFFmpeg libavcodec version: %u.%u.%u\r\n", major, minor, micro);
}    
//#include <stdio.h>
// #include <libavformat/avformat.h>
// #include <libavcodec/avcodec.h>
// #include <libswscale/swscale.h>

// 실행 의존 라이브러리 설치 / 링크드 컴파일 옵션
//sudo apt-get install ffmpeg libavcodec-dev libavformat-dev libavutil-dev libavdevice-dev
// gcc -o gocom_sin_ori gocom_sin_ori.c -lncursesw -D_GNU_SOURCE
// gcc -o adp_main adp_main.c -I/usr/include/x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu -lavformat -lavcodec -lavutil -lswscale -lswresample -lncursesw -D_GNU_SOURCE -lm

//#define BUTTON_COUNT 6 -화진-
#define MAX_FILES 100
#define MAX_FILENAME_LENGTH 256

void imageToAscii(const char *filePath);
void moveToAscii(const char *filePath);
void imageToText(const char *filePath);
void setting();
void introduceUs();
//void exitProgram();
char* selectFileFromSourceFolder(char *folder);
// void callFunctionInAnotherFile1(const char *filePath);
// void callFunctionInAnotherFile2(const char *filePath);
// void callFunctionInAnotherFile3(const char *filePath);
void start_screen_ncurser();

void draw_buttons(const char** buttons, int button_count, int current_button, int yButton, int xButton, int number) {
    for (int i = 0; i < button_count; i++) {
        int yButton = yButton;

        //number이 4일때 == 설정을 눌렀을때 버튼이 y축으로 한칸내려와야함(있어야할곳엔 프린트가찍힐거임)
        if ((number == 4) && (i == 0)) {
            const char *message = "----------------set_image----------------";
            mvprintw(yButton + i, xButton, "%s", message);
            yButton += 1;
        } else if ((number == 4) && (i == 3)) {
            printf("\r\n");
            const char *message = "----------------set_video----------------";
            mvprintw(yButton + i+1, xButton, "%s", message);
            yButton += 2;
        } else if ((number == 4) && (i == 7)) {
            printf("\r\n");
            const char *message = "----------------set_text----------------";
            mvprintw(yButton + i+1, xButton, "%s", message);
            yButton += 2;
        }

        if (i == current_button) {
            attron(A_REVERSE);
            attron(A_BOLD);
            mvprintw(yButton + i, xButton, "%s", buttons[i]);
            attroff(A_REVERSE);
            attroff(A_BOLD);
        } else {
            mvprintw(yButton + i, xButton, "%s", buttons[i]);
        }
    }
}

int main() {
    //setlocale(LC_ALL, "Korean");
    start_screen_ncurser();

    const char *menu_buttons[] = {
    "1. IMAGE > ASCII",
    "2. VIDEO > ASCII",
    "3. IMG > TEXT",
    "4. CONFIGURE",
    "5. MAKERS",
    "6. EXIT"
}   ;

    int menu_button_count = sizeof(menu_buttons) / sizeof(menu_buttons[0]);
    int menu_current_button = 0;
    int menu_ybutton = 10;
    int menu_xbutton = 10;
    while (1) {
        //system("clear");
        clear();
        
        const char *message = "GOCOM FINAL PROJECT TEAM 3";
        mvprintw(menu_ybutton - 2, (COLS - strlen(message)) / 2, "%s", message);

        draw_buttons((const char **)menu_buttons, menu_button_count, menu_current_button, 10, 10, 0);

        refresh();

        int ch = getch();
        switch (ch) {
        case KEY_UP:
            menu_current_button = (menu_current_button - 1 + menu_button_count) % menu_button_count;
            break;
        case KEY_DOWN:
            menu_current_button = (menu_current_button + 1) % menu_button_count;
            break;
        case 27: // ESC 입력
            endwin();
            return 0; // 프로그램 종료
        case '\n':
            switch (menu_current_button) {
            case 0: {
                char *filePath = selectFileFromSourceFolder("./imgs");
                if (filePath != NULL) {
                    //clear();
                    system("clear");
                    //mvprintw(0, 0, "Selected file path: %s\n", filePath);
                    image_player(filePath, 120, 10);
                    //callFunctionInAnotherFile1(filePath);
                    free(filePath);
                    //getch();
                }
                break;
            }
            case 1: {
                char *filePath = selectFileFromSourceFolder("./videos");
                if (filePath != NULL) {
                    clear();
                    //mvprintw(0, 0, "Selected file path: %s\n", filePath);
                    video_player(filePath,1, 110, 50);
                    //callFunctionInAnotherFile2(filePath);
                    free(filePath);
                    //getch();
                }
                break;
            }
            case 2: {
                char *filePath = selectFileFromSourceFolder("./imgs");
                if (filePath != NULL) {
                    clear();
                    mvprintw(0, 0, "Selected file path: %s\n", filePath);
                    //callFunctionInAnotherFile3(filePath);
                    free(filePath);
                    getch();
                }
                break;
            }
            case 3:
                setting();
                break;
            case 4:
                introduceUs();
                printf("\r\n"); // 이렇게 줄바꿈 해야 한다.
                getch();
                break;
            case 5: // 나가기
                endwin();
                return 0;
            }
        }
    }
    endwin();
}

char* selectFileFromSourceFolder(char *folder) {
    DIR *d;
    struct dirent *dir;
    //char *folder = "./source";
    char *fileList[MAX_FILES];
    int fileCount = 0;
    int highlight = 0;

    d = opendir(folder);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                fileList[fileCount] = (char *)malloc(MAX_FILENAME_LENGTH);
                strncpy(fileList[fileCount], dir->d_name, MAX_FILENAME_LENGTH);
                fileCount++;
            }
        }
        closedir(d);

        if (fileCount == 0) {
            mvprintw(0, 0, "NO FILE IN DIRECTORY\n");
            refresh();
            getch();
            return NULL;
        }

        int choice = -1;
        while (1) {
            clear();
            draw_buttons((const char **)fileList, fileCount, highlight, 10, 10, 123);
            refresh();

            int ch = getch();
            switch (ch) {
            case KEY_UP:
                highlight--;
                if (highlight < 0) highlight = fileCount - 1;
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight > (fileCount - 1)) highlight = 0;
                break;
            case 10:
                choice = highlight;
                break;
            case 27 : //ESC
                return NULL;
            default:
                break;
            }

            if (choice != -1) {
                break;
            }
        }

        char *filePath = (char *)malloc(MAX_FILENAME_LENGTH);
        snprintf(filePath, MAX_FILENAME_LENGTH, "%s/%s", folder, fileList[choice]);

        for (int i = 0; i < fileCount; i++) {
            free(fileList[i]);
        }

        return filePath;
    } else {
        mvprintw(0, 0, "CAN'T OPEN DIRECTORY %s\n", folder);
        refresh();
        getch();
        return NULL;
    }
}

void callFunctionInAnotherFile1(const char *filePath) {
    mvprintw(1, 0, "SELECTED FILE: %s\n", filePath);
    imageToAscii(filePath);
    refresh();
}

void callFunctionInAnotherFile2(const char *filePath) {
    mvprintw(1, 0, "SELECTED FILE: %s\n", filePath);
    moveToAscii(filePath);
    refresh();
}

void callFunctionInAnotherFile3(const char *filePath) {
    mvprintw(1, 0, "SELECTED FILE: %s\n", filePath);
    imageToText(filePath);
    refresh();
}

void imageToAscii(const char *filePath) {
    clear();
    mvprintw(12, 10, "IMAGE -> ASCII WORKING: %s", filePath);
    refresh();
    getch();
}

void moveToAscii(const char *filePath) {
    clear();
    mvprintw(12, 10, "VIDEO -> ASCII WORKING: %s", filePath);
    refresh();
    getch();
}

void imageToText(const char *filePath) {
    clear();
    mvprintw(12, 10, "IMAGE -> TXT WORKING: %s", filePath);
    refresh();
    getch();
}

void setting() {
    start_screen_ncurser();
    const char *set_buttons[] = {
    "1. img_resolution",
    "2. img_delay",
    "3. img_addr",
    "1. video_resolution",
    "2. video_delay",
    "3. video_interval",
    "4. video_addr",
    "1. image_to_text_addr"
}   ;

    int set_button_count = sizeof(set_buttons) / sizeof(set_buttons[0]);
    int set_current_button = 0;
    int set_ybutton = 10;
    int set_xbutton = 10;
    while (1) {
        //system("clear");
        clear();
        draw_buttons((const char **)set_buttons, set_button_count, set_current_button, 10, 10, 4);
        refresh();
        //리프레시를 두번이나 시켜도 진입할때 프린트가 되지 않음. 스위치문 한번 돌아야 프린트됨. 이유가 뭐지?
        int ch = getch();
        refresh();
        switch (ch) {
        case KEY_UP:
            set_current_button = (set_current_button - 1 + set_button_count) % set_button_count;
            break;
        case KEY_DOWN:
            set_current_button = (set_current_button + 1) % set_button_count;
            break;
        case 27: // ESC 입력
            return;
        case '\n':
            switch (set_current_button) {
            case 0: {
                // "1. img_resolution",
                break;
            }
            case 1: {
                //"2. img_delay",
                break;
            }
            case 2: {
                 //"3. img_addr",
                break;
            }
            case 3:
                //"1. video_resolution",
                break;
            case 4:
                //"2. video_delay",
                break;
            case 5:
                //"3. video_interval",
                break;
            case 6:
                //"4. video_addr",
                break;
            case 7:
                //"1. image_to_text_addr"
                break;
            default:
                break;
            }
        }
    }
}

void start_screen_ncurser() {
    initscr(); // ncurses 초기화
    cbreak(); // 문자를 입력할 때 바로 리턴받기 위한 모드
    noecho(); // 입력한 문자가 화면에 표시되지 않도록 설정
    keypad(stdscr, TRUE); //특수 키 입력
    curs_set(0); // 커서 숨기기
}

void introduceUs() {
    //clear();
    system("clear");
    //mvprintw(12, 10, "MAKERS WORKING");
    printf("MAKERS WORKING\r\n");
    printf("2020037108 문은식\r\n");
    printf("2020037108 이화진\r\n");
    printf("2020037108 신승우\r\n");
    printf("2020037108 양승민\r\n");
    //refresh();
    getchar();
}

void exitProgram() {
    clear();
    mvprintw(12, 10, "EXIT");
    refresh();
    getch();
    endwin();
    exit(0);
}
