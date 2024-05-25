#include "adp_player.h"

//#include <ncurses.h> // main.h로 이동
//#include <locale.h> // 한글
#include <string.h>
//#include <stdlib.h>  -화진-
//#include <stdio.h>
#include <dirent.h> // 파일가져올때 fopen
#include <sys/stat.h>

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
void make_folder_if_have_not(const char* directory);

void draw_buttons(const char** buttons, int button_count, int current_button, int yButton, int xButton, int number) {
    //여기서 if number==41코드 추가하니까 왜 메인에서 프린트를 못하는거지? ??? 여기서 막힘. 내일다시하자.
    //number == 41 설정-1번일때
    if (number == 41) {
       for (int i = 0; i < button_count; i++) {
            if (i == current_button) {
                attron(A_REVERSE);
                attron(A_BOLD);
                mvprintw(yButton, xButton + i*5, "%s", buttons[i]);
                attroff(A_REVERSE);
                attroff(A_BOLD);
            } else {
                mvprintw(yButton, xButton + i*5, "%s", buttons[i]);
            }
        }
    return;
    }

    int yOffset = yButton;  // yButton 변수를 yOffset으로 변경
    for (int i = 0; i < button_count; i++) {
        // number이 4일때 == 설정을 눌렀을때 버튼이 y축으로 한칸 내려와야함(있어야할곳엔 프린트가찍힐거임)
        if ((number == 4) && (i == 0)) {
            const char *message = "----------------set_image----------------";
            mvprintw(yOffset + i, xButton, "%s", message);
            yOffset += 1;
        } else if ((number == 4) && (i == 3)) {
            printf("\r\n");
            const char *message = "----------------set_video----------------";
            mvprintw(yOffset + i + 1, xButton, "%s", message);
            yOffset += 2;
        } else if ((number == 4) && (i == 7)) {
            printf("\r\n");
            const char *message = "----------------set_text----------------";
            mvprintw(yOffset + i + 1, xButton, "%s", message);
            yOffset += 2;
        }

        if (i == current_button) {
            attron(A_REVERSE);
            attron(A_BOLD);
            mvprintw(yOffset + i, xButton, "%s", buttons[i]);
            attroff(A_REVERSE);
            attroff(A_BOLD);
        } else {
            mvprintw(yOffset + i, xButton, "%s", buttons[i]);
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
    int img_resolution = 0;
    int img_delay = 0;
    int video_resolution = 0;
    int video_delay = 0;
    int video_interval = 0;
    
    char *img_addr, *video_addr, image_to_text_addr;

    start_screen_ncurser();

    // 버튼 이름을 설정합니다.
    char img_resolution_button[50];
    char img_delay_button[50];
    char video_resolution_button[50];

    int set_current_button = 0;
    int set_ybutton = 10;
    int set_xbutton = 10;

    main_loop:
    sprintf(img_resolution_button, "1. img_resolution: %d", img_resolution);
    sprintf(img_delay_button, "2. img_delay: %d", img_delay);
    sprintf(video_resolution_button, "1. video_resolution: %d", video_resolution);

    const char *set_buttons[] = {
        img_resolution_button,
        img_delay_button,
        "3. img_addr",
        video_resolution_button,
        "2. video_delay",
        "3. video_interval",
        "4. video_addr",
        "1. image_to_text_addr"
    };

    int set_button_count = sizeof(set_buttons) / sizeof(set_buttons[0]);

    while (1) {
        //system("clear");
        clear();
        draw_buttons((const char **)set_buttons, set_button_count, set_current_button, 10, 10, 4);
        refresh();
        int ch = getch();
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

                        // "1. img_resolution" 버튼을 선택한 경우
                        // 이미지 해상도 설정

                        const char *img_resolution_buttons[] = {
                            "72",
                            "98",
                            "128",
                            "144",
                            "240",
                            "360",
                            "480",
                            "720"
                        };

                        int img_resolution_button_count = sizeof(img_resolution_buttons) / sizeof(img_resolution_buttons[0]);
                        int img_resolution_current_button = 0;
                        int img_resolution_ybutton = 11;
                        int img_resolution_xbutton = 51;
                        int img_resolution_window_width = 20;
                        int img_resolution_window_height = img_resolution_button_count + 2;

                        while (1) {
                            draw_buttons((const char **)img_resolution_buttons, img_resolution_button_count, img_resolution_current_button, img_resolution_ybutton, img_resolution_xbutton , 41);
                            refresh();
                            int ch_img_resolution = getch();
                            switch (ch_img_resolution) {
                                case KEY_LEFT:
                                    img_resolution_current_button = (img_resolution_current_button - 1 + img_resolution_button_count) % img_resolution_button_count;
                                    break;
                                case KEY_RIGHT:
                                    img_resolution_current_button = (img_resolution_current_button + 1) % img_resolution_button_count;
                                    break;
                                case '\n':
                                    switch (img_resolution_current_button) {
                                        case 0:
                                            // "72" 선택한 경우
                                            img_resolution = 72;
                                            goto main_loop;
                                        case 1:
                                            // "98" 선택한 경우
                                            img_resolution = 98;
                                            goto main_loop;
                                        case 2:
                                            // "128" 선택한 경우
                                            img_resolution = 128;
                                            goto main_loop;
                                        case 3:
                                            // "144" 선택한 경우
                                            img_resolution = 144;
                                            goto main_loop;
                                        case 4:
                                            // "240" 선택한 경우
                                            img_resolution = 240;
                                            goto main_loop;
                                        case 5:
                                            // "360" 선택한 경우
                                            img_resolution = 360;
                                            goto main_loop;
                                        case 6:
                                            // "480" 선택한 경우
                                            img_resolution = 480;
                                            goto main_loop;
                                        case 7:
                                            // "720" 선택한 경우
                                            img_resolution = 720;
                                            goto main_loop;
                                        case 27: // ESC 입력
                                            goto main_loop;
                                        default:
                                            break;
                                    }
                                    // 선택이 완료되면 반복문을 탈출합니다.
                                    break;
                                case 27: // ESC 입력
                                    return;
                            }
                        }
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
                    case 3: {
                        //"1. video_resolution",
                        // "1. video_resolution" 버튼을 선택한 경우
                        // 비디오 해상도 설정

                        const char *video_resolution_buttons[] = {
                            "72",
                            "98",
                            "128",
                            "144",
                            "240",
                            "360",
                            "480",
                            "720"
                        };

                        int video_resolution_button_count = sizeof(video_resolution_buttons) / sizeof(video_resolution_buttons[0]);
                        int video_resolution_current_button = 0;
                        int video_resolution_ybutton = 16;
                        int video_resolution_xbutton = 51;
                        int video_resolution_window_width = 20;
                        int video_resolution_window_height = video_resolution_button_count + 2;

                        while (1) {
                            draw_buttons((const char **)video_resolution_buttons, video_resolution_button_count, video_resolution_current_button, video_resolution_ybutton, video_resolution_xbutton, 41);
                            refresh();
                            int ch_video_resolution = getch();
                            switch (ch_video_resolution) {
                                case KEY_LEFT:
                                    video_resolution_current_button = (video_resolution_current_button - 1 + video_resolution_button_count) % video_resolution_button_count;
                                    break;
                                case KEY_RIGHT:
                                    video_resolution_current_button = (video_resolution_current_button + 1) % video_resolution_button_count;
                                    break;
                                case '\n':
                                    switch (video_resolution_current_button) {
                                        case 0:
                                            video_resolution = 72;
                                            goto main_loop;
                                        case 1:
                                            video_resolution = 98;
                                            goto main_loop;
                                        case 2:
                                            video_resolution = 128;
                                            goto main_loop;
                                        case 3:
                                            video_resolution = 144;
                                            goto main_loop;
                                        case 4:
                                            video_resolution = 240;
                                            goto main_loop;
                                        case 5:
                                            video_resolution = 360;
                                            goto main_loop;
                                        case 6:
                                            video_resolution = 480;
                                            goto main_loop;
                                        case 7:
                                            video_resolution = 720;
                                            goto main_loop;
                                        case 27: // ESC 입력
                                            goto main_loop;
                                        default:
                                            break;
                                    }
                                    // 선택이 완료되면 반복문을 탈출
                                    break;
                                case 27: // ESC 입력
                                    return;
                            }
                        }
                        break;
                    }
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
                break;
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

//파라미터로 경로를 받음
void make_folder_if_have_not(const char* directory) {
    // 디렉토리 존재 여부 확인
    struct stat st = {0};

    if (stat(directory, &st) == -1) {
        // 디렉토리가 존재하지 않으면 생성
        if (mkdir(directory, 0777) == -1) {
            perror("mkdir failed");
        } else {
            printf("디렉토리 생성: %s\n", directory);
        }
    } else {
        printf("이미 존재하는 디렉토리: %s\n", directory);
    }
}
