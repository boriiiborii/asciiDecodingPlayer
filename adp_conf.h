#ifndef ADP_CONF_H
#define ADP_CONF_H

#include <ncurses.h>
#include <stdlib.h>
void start_screen_ncurser();
void draw_buttons(const char** buttons, int button_count, int current_button, int yButton, int xButton, int number);
void setting(int img_resolution, int img_delay, int video_resolution, int video_delay, int video_interval, char* img_addr, char* video_addr, char* image_to_text_addr);
void set_init();
void set_to_conf(int img_resolution, int img_delay, int video_resolution, int video_delay, int video_interval, char* img_addr, char* video_addr, char* image_to_text_addr);
void set_from_conf(int *img_resolution, int *img_delay, int *video_resolution, int *video_delay, int *video_interval, char *img_addr, char *video_addr, char *image_to_text_addr);
void introduceUs();

void start_screen_ncurser() {
    initscr(); // ncurses 초기화
    cbreak(); // 문자를 입력할 때 바로 리턴받기 위한 모드
    noecho(); // 입력한 문자가 화면에 표시되지 않도록 설정
    keypad(stdscr, TRUE); // 특수 키 입력
    curs_set(0); // 커서 숨기기
}

void draw_buttons(const char** buttons, int button_count, int current_button, int yButton, int xButton, int number) {
    //승우 화진 같이 쓰는 함수
    //number == 41 // 설정에서 설정 두번 들어갈때 41 세팅
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
            const char *message = "======================== SET IMAGE ========================";
            mvprintw(yOffset + i, xButton, "%s", message);
            yOffset += 1;
        } else if ((number == 4) && (i == 3)) {
            printf("\r\n");
            const char *message = "======================== SET VIDEO ========================";
            mvprintw(yOffset + i + 1, xButton, "%s", message);
            yOffset += 2;
        } else if ((number == 4) && (i == 7)) {
            printf("\r\n");
            const char *message = "========================= SET TXT =========================";
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

void setting(int img_resolution, int img_delay, int video_resolution, int video_delay, int video_interval, char* img_addr, char* video_addr, char* image_to_text_addr) {
    //-----------setting 지역변수 (전역변수와 이름은 같음)-----------
    int img_resolution_setting = img_resolution;
    int img_delay_setting = img_delay;
    int video_resolution_setting = video_resolution;
    int video_delay_setting = video_delay;
    int video_interval_setting = video_interval;
        
    char *img_addr_setting = "./imgs";
    char *video_addr_setting = "./videos";
    char *image_to_text_addr_setting = "./save";

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
    int set_ybutton = 1;
    int set_xbutton = 1;

    main_loop:
    sprintf(img_resolution_button, " img_resolution: %d", img_resolution_setting);
    sprintf(img_delay_button, " img_delay: %d (microseconds)", img_delay_setting);
    sprintf(video_resolution_button, " video_resolution: %d", video_resolution_setting);
    sprintf(img_addr_button, " img_addr: %s", img_addr_setting);
    sprintf(video_addr_button, " video_addr: %s", video_addr_setting);
    sprintf(image_to_text_addr_button, " image_to_text_addr: %s", image_to_text_addr_setting);
    sprintf(video_delay_button, " video_delay: %d (microseconds)", video_delay_setting);
    sprintf(video_interval_button, " video_interval: %d", video_interval_setting);

    const char *set_buttons[] = {
        img_resolution_button,
        img_delay_button,
        img_addr_button,
        video_resolution_button,
        video_delay_button,
        video_interval_button,
        video_addr_button,
        image_to_text_addr_button,
        "Init and Exit",
        "Save and Exit",
        "Exit"
    };

    int set_button_count = sizeof(set_buttons) / sizeof(set_buttons[0]);

    while (1) {
        //system("clear");
        clear();
        draw_buttons((const char **)set_buttons, set_button_count, set_current_button, 1, 1, 4);
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
                        int img_resolution_ybutton = 2;
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
                                            img_resolution_setting = 72;
                                            goto main_loop;
                                        case 1:
                                            // "98" 선택한 경우
                                            img_resolution_setting = 98;
                                            goto main_loop;
                                        case 2:
                                            // "128" 선택한 경우
                                            img_resolution_setting = 128;
                                            goto main_loop;
                                        case 3:
                                            // "144" 선택한 경우
                                            img_resolution_setting = 144;
                                            goto main_loop;
                                        case 4:
                                            // "240" 선택한 경우
                                            img_resolution_setting = 240;
                                            goto main_loop;
                                        case 5:
                                            // "360" 선택한 경우
                                            img_resolution_setting = 360;
                                            goto main_loop;
                                        case 6:
                                            // "480" 선택한 경우
                                            img_resolution_setting = 480;
                                            goto main_loop;
                                        case 7:
                                            // "720" 선택한 경우
                                            img_resolution_setting = 720;
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
                        //mvprintw(set_ybutton + 2, 50, "[ Number range 0~1000 ]");
                        mvprintw(set_ybutton + 2, 50, "Delay (microseconds 0~1000) : ");
                        refresh();
                        echo();//사용자가 입력한 값이 화면에 표시되도록
                        int temp = 0;
                        scanw("%d", &temp);//정수만 받아
                        if ((temp >= 0) && (temp <= 1000)) {
                            img_delay_setting = temp;
                        }
                        noecho();//입력값이 화면에 보이지 않도록
                        goto main_loop;
                    }
                    case 2: {
                        //"3. img_addr",
                        mvprintw(set_ybutton + 3, 50, "Image Address: ");
                        refresh();
                        echo();
                        char temp[500]; // 충분한 크기의 문자열을 저장할 수 있는 공간 할당
                        getstr(temp); //scanw으로 하니까 계속 버스에러났었는데 getstr로 해결
                        img_addr_setting = temp;
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
                        int video_resolution_ybutton = 7;
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
                                            video_resolution_setting = 72;
                                            goto main_loop;
                                        case 1:
                                            video_resolution_setting = 98;
                                            goto main_loop;
                                        case 2:
                                            video_resolution_setting = 128;
                                            goto main_loop;
                                        case 3:
                                            video_resolution_setting = 144;
                                            goto main_loop;
                                        case 4:
                                            video_resolution_setting = 240;
                                            goto main_loop;
                                        case 5:
                                            video_resolution_setting = 360;
                                            goto main_loop;
                                        case 6:
                                            video_resolution_setting = 480;
                                            goto main_loop;
                                        case 7:
                                            video_resolution_setting = 720;
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
                        //mvprintw(set_ybutton + 7, 50, "You can write 0~1000");
                        mvprintw(set_ybutton + 7, 50, "Delay (microseconds 0~1000) : ");
                        refresh();
                        echo(); // 사용자가 입력한 값이 화면에 표시되도록
                        int temp = 0;
                        scanw("%d", &temp); // 정수만 받아
                        if (temp >= 0 && temp <= 1000) {
                            video_delay_setting = temp;
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
                        int video_interval_ybutton = 9;
                        int video_interval_xbutton = 50;
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
                                            video_interval_setting = 1;
                                            goto main_loop;
                                        case 1:
                                            video_interval_setting = 2;
                                            goto main_loop;
                                        case 2:
                                            video_interval_setting = 3;
                                            goto main_loop;
                                        case 3:
                                            video_interval_setting = 4;
                                            goto main_loop;
                                        case 4:
                                            video_interval_setting = 5;
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
                        mvprintw(set_ybutton + 9, 50, "Video Address: ");
                        refresh();
                        echo();
                        char temp[500]; // 충분한 크기의 문자열을 저장할 수 있는 공간 할당
                        getstr(temp); //scanw으로 하니까 계속 버스에러났었는데 getstr로 해결
                        video_addr_setting = temp;
                        noecho();
                        goto main_loop;
                    }
                    case 7:{
                        //"1. image_to_text_addr"
                        mvprintw(set_ybutton + 12, 50, "Image To Text Address: ");
                        refresh();
                        echo();
                        char temp[500]; // 충분한 크기의 문자열을 저장할 수 있는 공간 할당
                        getstr(temp); //scanw으로 하니까 계속 버스에러났었는데 getstr로 해결
                        image_to_text_addr_setting = temp;
                        noecho();
                        goto main_loop;
                    }
                    case 8:
                        //init and exit
                        set_init();
                        return;
                    case 9:
                        //save and exit
                        //여기서 파라미터로 받았던 주소값들에 밸류로 내 지역변수 값을 넣어줘
                        //이후 conf파일로도 저장.
                        set_to_conf(img_resolution_setting, img_delay_setting, video_resolution_setting, video_delay_setting, video_interval_setting, img_addr_setting, video_addr_setting, image_to_text_addr_setting);
                        return;
                    case 10:
                        //exit
                        return;    
                    default:
                        break;
                }
                break;
        }
    }
}

void set_init() {
    FILE *file = fopen("conf.txt", "w");
    //각각의 키,밸류를 ':' 으로 구분한다고 생각해 생각만
    fprintf(file, "img_resolution: %d\n", 144);
    fprintf(file, "img_delay: %d\n", 10);
    fprintf(file, "video_resolution: %d\n", 144);
    fprintf(file, "video_delay: %d\n", 30);
    fprintf(file, "video_interval: %d\n", 1);
    fprintf(file, "img_addr: %s\n", "./imges");
    fprintf(file, "video_addr: %s\n", "./videos");
    fprintf(file, "image_to_text_addr: %s\n", "./save");
    fclose(file);
}

void set_to_conf(int img_resolution, int img_delay, int video_resolution, int video_delay, int video_interval, char* img_addr, char* video_addr, char* image_to_text_addr) {
    FILE *file = fopen("conf.txt", "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
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
    FILE *file = fopen("conf.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    
    char line[500]; // 한 줄을 저장할 버퍼
    while (fgets(line, sizeof(line), file)) {
        char key[50]; // 키를 저장할 버퍼
        char value[500]; // 값을 저장할 버퍼
        if (sscanf(line, "%[^:]: %[^\n]", key, value) == 2) {
            // 키와 값을 읽어와서 적절한 변수에 할당
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
                //img_addr = value;
            } else if (strcmp(key, "video_addr") == 0) {
                //strcpy(video_addr, value);
            } else if (strcmp(key, "image_to_text_addr") == 0) {
                //strcpy(image_to_text_addr, value);
            }
        }
    }
    
    fclose(file);
}

void introduceUs() {
    clear();
    //system("clear");
    mvprintw(5, 5, "%s","MAKERS WORKING");
    mvprintw(6, 5, "%s","2020037108 문은식");
    mvprintw(7, 5, "%s","2020037108 이화진");
    mvprintw(8, 5, "%s","2020037108 신승우");
    mvprintw(9, 5, "%s","2020037108 양승민");
   // 한글 입력에 #include <locale.h> , setlocale(LC_ALL, ""); 의존성
    refresh();
    //getch(); // main while에 포함돼있다.
}
#endif