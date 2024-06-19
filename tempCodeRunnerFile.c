// // //for save
// void print_ascii_image_buf(unsigned char* image_data, int width, int height) {
//     char buffer[BUFFER_SIZE];
//     char* ptr = buffer;
//     int chars_written;

//     for (int i = 0; i < height; i++) {
//         for (int j = 0; j < width; j++) {
//             int index = (int)((float)image_data[i * width + j] / 255 * NUM_CHARS);
//             if (index == NUM_CHARS)index = NUM_CHARS -1;
//             chars_written = snprintf(ptr, BUFFER_SIZE - (ptr - buffer), "%c", CHARS[index]); // 범위 내에 맞는 아스키 코드 선택
//             if (chars_written < 0 || ptr - buffer + chars_written >= BUFFER_SIZE) {
//                 fprintf(stderr, "Buffer overflow occurred!\r\n");
//                 exit(1);
//             }
//             ptr += chars_written;
//         }
//         if (ptr - buffer + 1 >= BUFFER_SIZE) {
//             fprintf(stderr, "Buffer overflow occurred!\r\n");
//             exit(1);
//         }
//         *ptr = '\0'; // 문자열 끝에 널 문자 추가
//         printf("%s\r\n", buffer); // 버퍼 출력
//         ptr = buffer; // 버퍼 초기화
//     }
// }