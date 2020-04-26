#include "display.h"

extern int rotate;

unsigned char* frame_black;
unsigned char* frame_red;

#define TAG "display"

void display_init() {
    if (epd2in9b_init() != 0) {
        ESP_LOGE(TAG, "e-Paper init failed");
        vTaskDelay(2000 / portTICK_RATE_MS);
        return;
    }
    ESP_LOGE(TAG, "e-Paper initialized");
    clear_frame();

    frame_black = (unsigned char*) malloc(EPD_WIDTH * EPD_HEIGHT / 8);
    frame_red = (unsigned char*) malloc(EPD_WIDTH * EPD_HEIGHT / 8);

    show_boot_screen();
}
void show_boot_screen() {
    /*
     * Clear black and clear red frame
     */
    paint(frame_black, EPD_WIDTH, EPD_HEIGHT);
    clear(0); // 0: UNCOLORED, 1: COLORED
    paint(frame_red, EPD_WIDTH, EPD_HEIGHT);
    clear(0); // 0: UNCOLORED, 1: COLORED
    
    rotate = ROTATE_90;
    draw_string_in_grid_align_center(1, 0, 300,  EPD_WIDTH/2-10, "Open Rowing Computer", &Ubuntu16);
    set_partial_window(frame_black, frame_red, 0,0,100,100);
    display_frame(frame_black, frame_red);
}
void show_error_screen() {
    
}
void display_update(tDisplayStruct * data) {
    char buffer[3];

    /*
     * Check incoming data
     */
    if(data->spm > 99) {
        ESP_LOGE(TAG, "incoming data structure spm value too high (%d)", data->spm);
    }

    /*
     * Clear black and clear red frame
     */
    paint(frame_black, EPD_WIDTH, EPD_HEIGHT);
    clear(0); // 0: UNCOLORED, 1: COLORED
    paint(frame_red, EPD_WIDTH, EPD_HEIGHT);
    clear(0); // 0: UNCOLORED, 1: COLORED

    /*
     * draw SPM
     */
    paint(frame_black, EPD_WIDTH, EPD_HEIGHT);
    rotate = ROTATE_90;
    itoa(data->spm, buffer, 10);
    draw_string_in_grid_align_center(1, 0, 110, 0, buffer, &Calibri);
    draw_string_in_grid_align_center(1, 0, 215, 80, "s", &Ubuntu16);
    draw_vertical_line(120, 0, 10, 1);
    draw_vertical_line(121, 0, 10, 1);
    draw_vertical_line(120, EPD_WIDTH-10, EPD_WIDTH, 1);
    draw_vertical_line(121, EPD_WIDTH-10, EPD_WIDTH, 1);
    draw_string_in_grid_align_center(1, 0, 110, 100, "minute", &Ubuntu16);
    rotate = ROTATE_0;
    draw_vertical_line(27, 10, 95, 1);
    draw_vertical_line(28, 10, 95, 1);

    /*
     * draw BT logo
     */
    if(data->ble_active) {
        rotate = ROTATE_90;
        draw_bitmap_mono(EPD_HEIGHT-bt_logo.width-10, 10, &bt_logo);
    }

    /*
     * IO
     */
    set_partial_window(frame_black, frame_red, 0,0,100,100);
    display_frame(frame_black, frame_red);
}