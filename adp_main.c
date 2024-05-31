#include "adp_player.h"

//#include <ncurses.h> // main.h로 이동
//#include <locale.h> // 한글
#include <string.h>
//#include <stdlib.h>  -화진-
//#include <stdio.h>
#include <dirent.h> // 파일가져올때 fopen
#include <sys/stat.h>
#include <locale.h>

//-----------setting 전역변수-----------
int img_resolution = 0;
int img_delay = 0;
int video_resolution = 0;
int video_delay = 0;
int video_interval = 0;
    
char *img_addr = "img_addr";
char *video_addr = "video_addr";
char *image_to_text_addr = "image_addr";
//-----------setting 전역변수-----------

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
void set_to_conf(int ir, int id, int vr, int vd, int vi, char* ia, char* va, char* ta);
void set_from_conf(int *img_resolution, int *img_delay, int *video_resolution, int *video_delay, int *video_interval, char *img_addr, char *video_addr, char *image_to_text_addr);

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
        } else if ((number == 4) && (i == 8)) {
            yOffset += 5;
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
    setlocale(LC_ALL, "");
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
    
    //만약 set_conf에서 conf파일이 없다면, setInit으로 처리하기(이것도 만들어야함)

    while (1) {
        //system("clear");
        clear();

        //세팅 전역변수을 파일값에서 참고하여 가져와야함(set_conf?메서드 ㅇㅇ)

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
                setting(); //아규먼트값을 setting전역변수를 넣어줘(주소값으로)
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

void setting() { //TODO: 파라미터값을 전역변수애들 주소 받아와줘야함!!!!!!!!
    //-----------setting 지역변수 (전역변수와 이름은 같음)-----------
    int img_resolution = 0;
    int img_delay = 0;
    int video_resolution = 0;
    int video_delay = 0;
    int video_interval = 0;
        
    char *img_addr = "img_addr";
    char *video_addr = "video_addr";
    char *image_to_text_addr = "image_addr";
    //-----------setting 지역변수 (전역변수와 이름은 같음)-----------
    //conf.txt의 값을 가져와서 셋팅
    // FILE *file = fopen("conf.txt", "r");
    // if (file != NULL) {
    //     set_from_conf(&img_resolution, &img_delay, &video_resolution, &video_delay, &video_interval, img_addr, video_addr, image_to_text_addr);
    // }
    

    start_screen_ncurser();

    // 버튼 이름을 설정합니다.
    char img_resolution_button[50];
    char img_delay_button[50];
    char video_resolution_button[50];
    char img_addr_button[500];
    char video_addr_button[500];
    char image_to_text_addr_button[500];
    char video_delay_button[50];
    char video_interval_button[50];

    int set_current_button = 0;
    int set_ybutton = 10;
    int set_xbutton = 10;

    main_loop:
    sprintf(img_resolution_button, "1. img_resolution: %d", img_resolution);
    sprintf(img_delay_button, "2. img_delay: %d (microseconds)", img_delay);
    sprintf(video_resolution_button, "1. video_resolution: %d", video_resolution);
    sprintf(img_addr_button, "3. img_addr: %s", img_addr);
    sprintf(video_addr_button, "4. video_addr: %s", video_addr);
    sprintf(image_to_text_addr_button, "1. image_to_text_addr: %s", image_to_text_addr);
    sprintf(video_delay_button, "2. video_delay: %d (microseconds)", video_delay);
    sprintf(video_interval_button, "3. video_interval: %d", video_interval);

    const char *set_buttons[] = {
        img_resolution_button,
        img_delay_button,
        img_addr_button,
        video_resolution_button,
        video_delay_button,
        video_interval_button,
        video_addr_button,
        image_to_text_addr_button,
        "save and exit",
        "exit"
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
                        // 숫자를 입력받는 칸 생성
                        mvprintw(set_ybutton + 1, 50, "you can write 0~1000");
                        mvprintw(set_ybutton + 2, 50, "Delay (microseconds) : ");
                        refresh();
                        echo();//사용자가 입력한 값이 화면에 표시되도록
                        int temp = 0;
                        scanw("%d", &temp);//정수만 받아
                        if ((temp >= 0) && (temp <= 1000)) {
                            img_delay = temp;
                        }
                        noecho();//입력값이 화면에 보이지 않도록
                        goto main_loop;
                    }
                    case 2: {
                        //"3. img_addr",
                        mvprintw(set_ybutton + 2, 50, "Image Address: ");
                        refresh();
                        echo();
                        char temp[500]; // 충분한 크기의 문자열을 저장할 수 있는 공간 할당
                        getstr(temp); //scanw으로 하니까 계속 버스에러났었는데 getstr로 해결
                        img_addr = temp;
                        noecho();
                        goto main_loop;
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
                    case 4: {
                        //"2. video_delay",
                        // 숫자를 입력받는 칸 생성
                        mvprintw(set_ybutton + 6, 50, "you can write 0~1000");
                        mvprintw(set_ybutton + 7, 50, "Delay (microseconds) : ");
                        refresh();
                        echo(); // 사용자가 입력한 값이 화면에 표시되도록
                        int temp = 0;
                        scanw("%d", &temp); // 정수만 받아
                        if (temp >= 0 && temp <= 1000) {
                            video_delay = temp;
                        }
                        noecho(); // 입력값이 화면에 보이지 않도록
                        goto main_loop;
                    }

                    case 5: {
                        //"3. video_interval",

                        const char *video_interval_buttons[] = {
                            "1",
                            "2",
                            "3",
                            "4",
                            "5",
                        };

                        int video_interval_button_count = sizeof(video_interval_buttons) / sizeof(video_interval_buttons[0]);
                        int video_interval_current_button = 0;
                        int video_interval_ybutton = 16;
                        int video_interval_xbutton = 51;
                        int video_interval_window_width = 20;
                        int video_interval_window_height = video_interval_button_count + 2;

                        while (1) {
                            draw_buttons((const char **)video_interval_buttons, video_interval_button_count, video_interval_current_button, video_interval_ybutton, video_interval_xbutton, 41);
                            refresh();
                            int ch_video_interval = getch();
                            switch (ch_video_interval) {
                                case KEY_LEFT:
                                    video_interval_current_button = (video_interval_current_button - 1 + video_interval_button_count) % video_interval_button_count;
                                    break;
                                case KEY_RIGHT:
                                    video_interval_current_button = (video_interval_current_button + 1) % video_interval_button_count;
                                    break;
                                case '\n':
                                    switch (video_interval_current_button) {
                                        case 0:
                                            video_interval = 1;
                                            goto main_loop;
                                        case 1:
                                            video_interval = 2;
                                            goto main_loop;
                                        case 2:
                                            video_interval = 3;
                                            goto main_loop;
                                        case 3:
                                            video_interval = 4;
                                            goto main_loop;
                                        case 4:
                                            video_interval = 5;
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
                    case 6:{
                        //"4. video_addr",
                        mvprintw(set_ybutton + 2, 50, "Video Address: ");
                        refresh();
                        echo();
                        char temp[500]; // 충분한 크기의 문자열을 저장할 수 있는 공간 할당
                        getstr(temp); //scanw으로 하니까 계속 버스에러났었는데 getstr로 해결
                        video_addr = temp;
                        noecho();
                        goto main_loop;
                    }
                    case 7:{
                        //"1. image_to_text_addr"
                        mvprintw(set_ybutton + 2, 50, "Image To Text Address: ");
                        refresh();
                        echo();
                        char temp[500]; // 충분한 크기의 문자열을 저장할 수 있는 공간 할당
                        getstr(temp); //scanw으로 하니까 계속 버스에러났었는데 getstr로 해결
                        image_to_text_addr = temp;
                        noecho();
                        goto main_loop;
                    }
                    case 8:
                        //save and exit
                        
                        //여기서 파라미터로 받았던 주소값들에 밸류로 내 지역변수 값을 넣어줘

                        //이후 conf파일로도 저장.
                        set_to_conf(img_resolution, img_delay, video_resolution, video_delay, video_interval, img_addr, video_addr, image_to_text_addr);
                        return;
                    case 9:
                        //exit
                        return;
                    default:
                        break;
                }
                break;
        }
    }
}

void set_to_conf(int img_resolution, int img_delay, int video_resolution, int video_delay, int video_interval, char* img_addr, char* video_addr, char* image_to_text_addr) {
    FILE *file = fopen("conf.txt", "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    //각각의 키밸류를 공백으로 구분한다고 생각해
    fprintf(file, "img_resolution: %d\n", img_resolution);
    fprintf(file, "img_delay: %d\n", img_delay);
    fprintf(file, "video_resolution: %d\n", video_resolution);
    fprintf(file, "video_delay: %d\n", video_delay);
    fprintf(file, "video_interval: %d\n", video_interval);
    fprintf(file, "img_addr: %s\n", img_addr);
    fprintf(file, "video_addr: %s\n", video_addr);
    fprintf(file, "image_to_text_addr: %s\n", image_to_text_addr);
    fclose(file);
}

void set_from_conf(int *img_resolution, int *img_delay, int *video_resolution, int *video_delay, int *video_interval, char *img_addr, char *video_addr, char *image_to_text_addr) {
    char line[512];
    FILE *file = fopen("conf.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        char key[128];
        char value[384];

        // 줄 끝의 콤마와 개행 문자를 제거
        char *newline = strchr(line, ',');
        if (newline) {
            *newline = '\0';
        }

        // 키와 값을 파싱
        if (sscanf(line, "%[^:]: %[^\n]", key, value) == 2) {
            if (strcmp(key, "img_resolution") == 0) {
                *img_resolution = atoi(value);
            } else if (strcmp(key, "img_delay") == 0) {
                *img_delay = atoi(value);
            } else if (strcmp(key, "video_resolution") == 0) {
                *video_resolution = atoi(value);
            } else if (strcmp(key, "video_delay") == 0) {
                *video_delay = atoi(value);
            } else if (strcmp(key, "video_interval") == 0) {
                *video_interval = atoi(value);
            } else if (strcmp(key, "img_addr") == 0) {
                strncpy(img_addr, value, 499);
                img_addr[499] = '\0';
            } else if (strcmp(key, "video_addr") == 0) {
                strncpy(video_addr, value, 499);
                video_addr[499] = '\0';
            } else if (strcmp(key, "image_to_text_addr") == 0) {
                strncpy(image_to_text_addr, value, 499);
                image_to_text_addr[499] = '\0';
            }
        }
    }

    fclose(file);
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
