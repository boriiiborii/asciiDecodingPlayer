#ifndef ADP_PLAYER_H
#define ADP_PLAYER_H

#define BUFFER_SIZE (720 * 720 * 1) // 고정 버퍼 (NW * NW * 2 ) 권장
#define NUM_CHARS 13 //ASCII 아트에 사용할 문자의 개수
const char CHARS[] = " .,-~:;=!*#$@";

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

// 비디오 INCLUDE
#include <libavutil/imgutils.h>       // ffmpeg
#include <libavutil/mem.h>            // ffmpeg
#include <libavutil/opt.h>            // ffmpeg
#include <libavformat/avformat.h>     // ffmpeg
#include <libswscale/swscale.h>       // ffmpeg
#include <libswresample/swresample.h> // ffmpeg
#include <libavcodec/avcodec.h>       // ffmpeg
#include <math.h>
//#include <conio.h> // 비동기 입력 //
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep

#include <ncurses.h>

void resize_image(unsigned char* input_data, int input_width, int input_height, unsigned char** output_data, int* output_width, int* output_height, int video_resolution);
void normalize_image(unsigned char* input_data, int width, int height);
void print_ascii_image_buf(unsigned char* image_data, int width, int height);
void print_ascii_image(unsigned char* image_data, int new_width, int new_height, int img_delay);

int image_player(char* img_addr, int img_resolution, int img_delay);// .h로 이동

void print_space(int num);
void topBar(int frame_number, int totalFrame, int pause_state, int video_resolution);
void loadingBar(int screenWidth, int totalFrame, int frame);

int video_player(char* video_addr, int output_interval, int video_resolution, int frame_delay);// .h로 이동

char* ascii_image_to_str_for_txt(unsigned char* image_data, int new_width, int new_height, int img_delay);
char* image_player_for_txt(char* img_addr, int img_resolution, int img_delay);

int img_to_txt(unsigned char* img_txt, const char* addr);
char* convert_to_txt(const char* str);
char* concat_strings(const char* str1, const char* str2);

// 이미지 크기 조정
void resize_image(unsigned char* input_data, int input_width, int input_height,unsigned char** output_data, int* output_width, int* output_height, int video_resolution) {
    float aspect_ratio = (float)input_width * 2 / input_height;
    *output_width = video_resolution;
    *output_height = video_resolution / aspect_ratio;
    *output_data = (unsigned char*)malloc(video_resolution * (*output_height));
    stbir_resize_uint8(input_data, input_width, input_height, 0, *output_data, *output_width, *output_height, 0, 1);
} // video_resolution 매개변수 추가

void normalize_image(unsigned char* input_data, int width, int height) {
    int min_value = 255;
    int max_value = 0;
    // 이미지의 최소 및 최대 값을 찾습니다.
    for (int i = 0; i < width * height; i++) {
        if (input_data[i] < min_value) {
            min_value = input_data[i];
        }
        if (input_data[i] > max_value) {
            max_value = input_data[i];
        }
    }
    // 이미지를 정규화합니다.
    for (int i = 0; i < width * height; i++) {
        input_data[i] = (unsigned char)((input_data[i] - min_value) / (float)(max_value - min_value) * 255);
    }
}
// 리눅스 용으로 변경
void print_ascii_image_buf(unsigned char* image_data, int width, int height) {
    char buffer[BUFFER_SIZE];
    char* ptr = buffer;
    int chars_written;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int index = (int)((float)image_data[i * width + j] / 255 * NUM_CHARS);
            if (index == NUM_CHARS)index = NUM_CHARS -1;
            chars_written = snprintf(ptr, BUFFER_SIZE - (ptr - buffer), "%c", CHARS[index]); // 범위 내에 맞는 아스키 코드 선택
            if (chars_written < 0 || ptr - buffer + chars_written >= BUFFER_SIZE) {
                fprintf(stderr, "Buffer overflow occurred!\r\n");
                exit(1);
            }
            ptr += chars_written;
        }
        if (ptr - buffer + 1 >= BUFFER_SIZE) {
            fprintf(stderr, "Buffer overflow occurred!\r\n");
            exit(1);
        }
        *ptr = '\0'; // 문자열 끝에 널 문자 추가
        printf("%s\r\n", buffer); // 버퍼 출력
        ptr = buffer; // 버퍼 초기화
    }
}

void print_ascii_image(unsigned char* image_data, int new_width, int new_height, int img_delay) {
    for (int i = 0; i < new_height; i++) {
        for (int j = 0; j < new_width; j++) {
            int index = (int)((float)image_data[i * new_width + j] / 255 * NUM_CHARS);
            if(index == NUM_CHARS) index = NUM_CHARS-1;
            printf("%c", CHARS[index]);
            //printf("%3d  ",image_data[i * new_width + j]);
            //printf("%02d%c_",index,CHARS[index]);
            //printf("\r\n");
        }
        fflush(stdout); // 출력 버퍼를 비워줌
        printf("\r\n");
        usleep(img_delay * 1000); // 마이크로초 단위의 지연
       
    }
}
int image_player(char* img_addr, int img_resolution, int img_delay) {
    // Load image using stb_image
    int width, height, channels;
    unsigned char* img = stbi_load(img_addr, &width, &height, &channels, 0); // 이미지 주소에서 가져오기
    if (!img) {
        printf("Failed to load image.\r\n");
        getchar();
        return -1;
    }

    // Calculate new width and height while maintaining aspect ratio
    int new_width, new_height;
    if (width > height) {
        new_width = img_resolution;
        new_height = (int)((float)height / width * img_resolution);
    }
    else {
        new_height = img_resolution;
        new_width = (int)((float)width / height * img_resolution);
    }

    // Convert image to grayscale
    unsigned char* gray_img = (unsigned char*)malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        int r = img[3 * i];
        int g = img[3 * i + 1];
        int b = img[3 * i + 2];
        gray_img[i] = (unsigned char)(0.2989 * r + 0.5870 * g + 0.1140 * b);
    }

    // Resize image using stb_image_resize
    unsigned char* resized_img = (unsigned char*)malloc(new_width * new_height);
    //stbir_resize_uint8(gray_img, width, height, 0, resized_img, new_width, new_height, 0, 1);
    resize_image(gray_img, width, height, &resized_img, &new_width, &new_height,img_resolution);
    // 이미지 정규화
    normalize_image(resized_img, new_width, new_height);

    // Print ASCII image
    print_ascii_image(resized_img, new_width, new_height, img_delay);

    // Free memory
    free(img);
    free(gray_img);
    free(resized_img);
    
    // Wait for user input (pressing any key)
    printf("Press any key to continue...");
    getchar(); // 대기하고 사용자 입력 받기

    return 0;
}
void print_space(int num) {
    for (int i = 0; i < num; i++) {
        printf(" ");
    }
}
// video_resolution 추가
void topBar(int frame_number, int totalFrame, int pause_state, int video_resolution) {
    printf("Frame:%4d/%4d", frame_number, totalFrame);
    print_space((int)video_resolution / 2 - 16 - 3); // 16 == length of "Frame:%4d/%4d"
    if (pause_state != 0 && pause_state != 1) pause_state = 0;
    if (pause_state == 0) {
        printf("[ ▶ ]"); // 5 byte
        print_space((int)video_resolution / 2 - 21 - 1); // 21 == length of "ASCII DECODING PLAYER"
    }
    else if (pause_state == 1) {
        printf("[ || ]"); // 6 byte
        print_space((int)video_resolution / 2 - 21 - 2); // 21 == length of "ASCII DECODING PLAYER"
    }
    printf("ASCII DECODING PLAYER");
    printf("\r\n");
}
void loadingBar(int screenWidth, int totalFrame, int frame) {
    int totalBarLeng = screenWidth - 6; // 로딩바의 길이 
    int barLeng = (int)lround(totalBarLeng * (float)(frame) / totalFrame); // 현재 로딩바의 길이
    int percentage;

    printf("[");
    for (int j = 0; j < barLeng; j++) {
        printf("█");
    }
    //printf(".");
    for (int j = 0; j < totalBarLeng - barLeng - 1; j++) {
        printf("-");
    }
    percentage = (int)((100 *(float)frame / totalFrame) + 0.6);
    if(percentage>=99) percentage = 100;
    printf("]%3d%%\r\n", percentage);
    fflush(stdout);
}
int video_player(char* video_addr, int output_interval, int video_resolution, int frame_delay) {
    int key; // 비동기 입력 키
    nodelay(stdscr, TRUE);  // 비동기 입력 모드 설정

    // 입력 동영상 파일 경로
    char* input_file = video_addr;

    // FFmpeg 네트워크 초기화
    avformat_network_init();

    // 입력 파일을 열고 포맷 컨텍스트를 생성
    AVFormatContext* format_context = NULL;
    if (avformat_open_input(&format_context, input_file, NULL, NULL) != 0) {
        printf("Error: Couldn't open input file.\r\n");
        nodelay(stdscr, FALSE);
        getchar(); // 대기하고 사용자 입력 받기
        return 1;
    }
    // 파일에서 스트림 정보 찾기
    if (avformat_find_stream_info(format_context, NULL) < 0) {
        printf("Error: Couldn't find stream information.\r\n");
         nodelay(stdscr, FALSE);
        getchar(); // 대기하고 사용자 입력 받기
        return 1;
    }

    // 비디오 스트림 인덱스 찾기
    int video_stream_index = -1;
    for (int i = 0; i < format_context->nb_streams; i++) {
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    // 비디오 스트림이 없는 경우 오류 출력
    if (video_stream_index == -1) {
        printf("Error: Couldn't find a video stream.\r\n");
        nodelay(stdscr, FALSE);
        getchar(); // 대기하고 사용자 입력 받기
        return 1;
    }

    // 코덱 파라미터 가져오기
    AVCodecParameters* codec_params = format_context->streams[video_stream_index]->codecpar;

    // 코덱 찾기
    //AVCodec * codec앞에 const붙여야 컴파일 되서 붙여놓음 -화진-
    const AVCodec * codec = avcodec_find_decoder(codec_params->codec_id);
    //------------화-
    if (!codec) {
        printf("Error: Unsupported codec.\r\n");
        nodelay(stdscr, FALSE);
        getchar(); // 대기하고 사용자 입력 받기
        return 1;
    }

    // 코덱 컨텍스트 할당 및 초기화
    AVCodecContext* codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        printf("Error: Failed to allocate codec context.\r\n");
        nodelay(stdscr, FALSE);
        getchar(); // 대기하고 사용자 입력 받기
        return 1;
    }

    // 코덱 파라미터를 코덱 컨텍스트로 설정
    if (avcodec_parameters_to_context(codec_context, codec_params) < 0) {
        printf("Error: Failed to initialize codec context.\r\n");
        nodelay(stdscr, FALSE);
        getchar(); // 대기하고 사용자 입력 받기
        return 1;
    }

    // 코덱 열기
    if (avcodec_open2(codec_context, codec, NULL) < 0) {
        printf("Error: Failed to open codec.\r\n");
        nodelay(stdscr, FALSE);
        getchar(); // 대기하고 사용자 입력 받기
        return 1;
    }

    // 프레임 및 패킷 할당
    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        printf("Error: Failed to allocate frame.\r\n");
        nodelay(stdscr, FALSE);
        getchar(); // 대기하고 사용자 입력 받기
        return 1;
    }

    AVPacket* packet = av_packet_alloc();
    if (!packet) {
        printf("Error: Failed to allocate packet.\r\n");
        nodelay(stdscr, FALSE);
        getchar(); // 대기하고 사용자 입력 받기
        return 1;
    }

    int frame_number = 1;
    // 영상의 모든 프레임 갯수 저장
    int totalFrame = format_context->streams[video_stream_index]->nb_frames;
    //
    //int key;
    //
    // 프레임 읽기 반복
    while (av_read_frame(format_context, packet) >= 0) {
        if (packet->stream_index == video_stream_index) {
            // 패킷을 디코딩
            int response = avcodec_send_packet(codec_context, packet);
            if (response < 0) {
                printf("Error decoding packet: %s\r\n", av_err2str(response));
                return 1;
            }
            
            while (response >= 0) {
                response = avcodec_receive_frame(codec_context, frame);
                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                    break;
                }
                else if (response < 0) {
                    printf("Error decoding frame: %s\r\n", av_err2str(response));
                    return 1;
                }

                if (frame_number % output_interval == 0) {
                    system("clear"); // 화면을 지우는 명령 (리눅스 운영 체제에서만 동작)
                    // 프레임 크기 조정
                    unsigned char* resized_img;
                    int resized_width, resized_height;
                    resize_image(frame->data[0], frame->width, frame->height, &resized_img, &resized_width, &resized_height, video_resolution);
                    // 프레임 이미지 정규화
                    normalize_image(resized_img, video_resolution, resized_height);
                    // 탑 바 출력
                    topBar(frame_number, totalFrame, 0, video_resolution);
                    // 로딩 바 출력
                    loadingBar(video_resolution, totalFrame, frame_number);
                    // ASCII 아트 출력
                    print_ascii_image_buf(resized_img, video_resolution, resized_height);

                    usleep(frame_delay * 1000); // 마이크로초 단위로 대기 (리눅스 운영 체제에서만 동작)

                    // 메모리 해제
                    //free(resized_img);
                
                    // 비동기 키보드 입력 확인
                    key = getch();
                    if (key != ERR) {
                        switch (key) {
                            case 27: // ESC 키
                                //endwin(); // ncurses 종료
                                system("clear");
                                nodelay(stdscr, FALSE);  // 비동기 입력 모드 비활성화
                                goto end;
                                break;
                            case ' ': // 스페이스 바
                                system("clear");
                                topBar(frame_number,totalFrame,1,video_resolution);
                                loadingBar(video_resolution, totalFrame, frame_number);
                                print_ascii_image_buf(resized_img, video_resolution, resized_height);
                                //frame_number++;
                                if (getchar() == 27){
                                    system("clear");
                                    nodelay(stdscr, FALSE);  // 비동기 입력 모드 비활성화
                                    goto end;
                                }
                                break;
                            case '\n': // 엔터
                                break;
                            default:
                                // 다른 키에 대한 동작 수행
                                break;
                        }
                    }
                    // 메모리 해제
                    free(resized_img);
                }
                frame_number++;
            }
        }
        av_packet_unref(packet);
    }
    nodelay(stdscr, FALSE);  // 비동기 입력 모드 비활성화
    printf("Press any key to continue...");
    getchar(); // 대기하고 사용자 입력 받기
end:
    // 포맷 컨텍스트 및 메모리 해제
    avformat_close_input(&format_context);
    avcodec_free_context(&codec_context);
    av_frame_free(&frame);
    av_packet_free(&packet);

    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
char* ascii_image_to_str_for_txt(unsigned char* image_data, int new_width, int new_height, int img_delay) {
    // 이미지를 저장할 문자열 동적 할당
    int str_size = (new_width + 1) * new_height + 1; // 각 행마다 개행 문자를 포함하기 때문에 new_width + 1
    char* result = (char*)malloc(str_size);
    if (result == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }

    char* ptr = result;
    for (int i = 0; i < new_height; i++) {
        for (int j = 0; j < new_width; j++) {
            int index = (int)((float)image_data[i * new_width + j] / 255 * NUM_CHARS);
            if (index == NUM_CHARS) index = NUM_CHARS - 1;
            *ptr = CHARS[index];
            ptr++;
        }
        *ptr = '\n'; // 개행 문자 추가
        ptr++;
        usleep(img_delay * 1); // 마이크로초 단위의 지연
    }
    *ptr = '\0'; // 문자열 마지막에 널 문자 추가

    return result;
}

char* image_player_for_txt(char* img_addr, int img_resolution, int img_delay) {
    // Load image using stb_image
    int width, height, channels;
    unsigned char* img = stbi_load(img_addr, &width, &height, &channels, 0); // 이미지 주소에서 가져오기
    if (!img) {
        printf("Failed to load image.\r\n");
        getchar();
        return NULL;
    }

    // Calculate new width and height while maintaining aspect ratio
    int new_width, new_height;
    if (width > height) {
        new_width = img_resolution;
        new_height = (int)((float)height / width * img_resolution);
    }
    else {
        new_height = img_resolution;
        new_width = (int)((float)width / height * img_resolution);
    }

    // Convert image to grayscale
    unsigned char* gray_img = (unsigned char*)malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        int r = img[3 * i];
        int g = img[3 * i + 1];
        int b = img[3 * i + 2];
        gray_img[i] = (unsigned char)(0.2989 * r + 0.5870 * g + 0.1140 * b);
    }

    // Resize image using stb_image_resize
    unsigned char* resized_img = (unsigned char*)malloc(new_width * new_height);
    //stbir_resize_uint8(gray_img, width, height, 0, resized_img, new_width, new_height, 0, 1);
    resize_image(gray_img, width, height, &resized_img, &new_width, &new_height,img_resolution);
    // 이미지 정규화
    normalize_image(resized_img, new_width, new_height);

    // Print ASCII image
    //print_ascii_image(resized_img, new_width, new_height, img_delay);
    //print_ascii_image_for_txt(resized_img,new_width,new_height,100,file_name);
    char* img_txt = (char*)malloc((new_width+1) * (new_height+1));
    img_txt = ascii_image_to_str_for_txt(resized_img,new_width,new_height,100);

    // Free memory
    free(img);
    free(gray_img);
    free(resized_img);
    
    // Wait for user input (pressing any key)
    //printf("Press any key to continue...");
    //getchar(); // 대기하고 사용자 입력 받기

    return img_txt;
}
int img_to_txt(unsigned char* img_txt, const char* addr) {
    // 파일 포인터 선언
    FILE* file;

    // 파일 열기, "w" 모드는 쓰기 모드
    file = fopen(addr, "w");

    // 파일 열기에 실패하면 에러 메시지 출력 후 함수 종료
    if (file == NULL) {
        printf("파일 열기에 실패했습니다.\n");
        return 1;
    }

    // 파일에 문자열 쓰기
    fprintf(file, "%s", img_txt);

    // 파일 닫기
    fclose(file);

    // 파일 저장에 성공했다는 메시지 출력 및 저장 위치 정보 표시
    printf("파일이 성공적으로 저장되었습니다.\r\n저장 위치: %s\r\n", addr);

    return 0;
}
char* convert_to_txt(const char* str) {
    // 문자열을 복사하여 작업하기 위한 임시 문자열
    char* temp_str = strdup(str);
    // '/'를 기준으로 문자열 분할
    char* token = strtok(temp_str, "/");
    char* last_token = NULL;
    while (token != NULL) {
        last_token = token;
        token = strtok(NULL, "/");
    }
    // '.'를 기준으로 문자열 분할
    token = strtok(last_token, ".");
    char* file_name = (char*)malloc(strlen(token) + 5); // "txt" + '\0'
    if (file_name != NULL) {
        strcpy(file_name, token);
        strcat(file_name, ".txt");
    }
    free(temp_str);
    return file_name;
}
char* concat_strings(const char* str1, const char* str2) {
    // 충분한 메모리를 할당하여 두 문자열을 합칠 공간을 만듭니다.
    char* result = (char*)malloc(strlen(str1) + strlen(str2) + 1);
    if (result == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }
    // str1 문자열을 result에 복사합니다.
    strcpy(result, str1);
    // str2 문자열을 result에 이어붙입니다.
    strcat(result, str2);
    return result;
}
#endif