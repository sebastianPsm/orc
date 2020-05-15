#include "display.h"

extern int rotate;

unsigned char* frame_black;
unsigned char* frame_red;

#define TAG "display"

TaskHandle_t update_task;

void display_init() {
    if (epd2in9b_init() != 0) {
        ESP_LOGE(TAG, "e-Paper init failed");
        vTaskDelay(2000 / portTICK_RATE_MS);
        return;
    }
    ESP_LOGI(TAG, "e-Paper initialized");
    clear_frame();

    frame_black = (unsigned char*) malloc(EPD_WIDTH * EPD_HEIGHT / 8);
    frame_red = (unsigned char*) malloc(EPD_WIDTH * EPD_HEIGHT / 8);

    //show_boot_screen();
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

void _display_update(void * data) {
    char buffer[3];
    tStatus * status = (tStatus *) data;

    ESP_LOGI(TAG, "Start task... ");

    while(true) {
        ESP_LOGI(TAG, "Update task suspended");
        vTaskSuspend( NULL );
        ESP_LOGI(TAG, "Update...");

        /*
         * Check incoming data
         */
        if(status->spm > 99) {
            ESP_LOGE(TAG, "incoming data structure spm value too high (%d)", status->spm);
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
        itoa(status->spm, buffer, 10);

        paint(frame_black, EPD_WIDTH, EPD_HEIGHT);
        rotate = ROTATE_90;
        draw_string_in_grid_align_center(3, 0, EPD_HEIGHT, 0, buffer, &Calibri);
        draw_string_in_grid_align_center(6, 1, EPD_HEIGHT, 100, "SPM", &Ubuntu16);

        draw_vertical_line(98, 0, 10, 1);
        draw_vertical_line(99, 0, 10, 1);
        draw_vertical_line(98, EPD_WIDTH-10, EPD_WIDTH, 1);
        draw_vertical_line(99, EPD_WIDTH-10, EPD_WIDTH, 1);
        
        rotate = ROTATE_0;

        /*
         * sleep active
         */
        if(status->sleep_active) {
            ESP_LOGI(TAG, "draw sleep mode active");
            rotate = ROTATE_90;
            draw_string_in_grid_align_right(12, 11, 0, EPD_HEIGHT, 20, "zZzz", &Ubuntu16);
        }
        
        /*
         * draw BT logo
         */
        if(status->ble_active) {
            ESP_LOGI(TAG, "draw ble active");
            rotate = ROTATE_90;
            draw_bitmap_mono(EPD_HEIGHT-bt_logo.width-10, 10, &bt_logo);
        }

        /*
         * draw log active
         */
        if(status->logging_active) {
            ESP_LOGI(TAG, "draw logging active");
            rotate = ROTATE_90;
            draw_string_in_grid_align_right(12, 11, 0, EPD_HEIGHT, 30, "log", &Ubuntu16);
        }

        /*
         * draw name
         */
        if(status->name_owner) {
            ESP_LOGI(TAG, "draw owner ('%s')", status->name_owner);
            rotate = ROTATE_90;
            draw_string(status->name_owner, 0, 0, &Ubuntu16);
        }

        /*
         * IO
         */
        set_partial_window(frame_black, frame_red, 0,0,100,100);
        display_frame(frame_black, frame_red);

        ESP_LOGI(TAG, "done");
    }
    vTaskDelete(NULL);
}

void display_start_update_task(const tStatus * status) {
    xTaskCreate(_display_update, "display_update", 4 * 1024, (void *) status, 6, &update_task);
    while(eTaskGetState(update_task) != eSuspended);
}

void display_update() {
    vTaskResume(update_task);
}