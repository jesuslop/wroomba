#include "task_wroomba.h"

static const char* TAG = "wroomba";

void vATaskWroomba(void *pvParameters) {
    // Wait until we have a WiFi connection
    xEventGroupWaitBits(
      xWiFiEventGroup,
      (BIT_CONNECTED | BIT_DISCONNECTED),
      CLEAR_BITS_FALSE,
      WAIT_ALL_BITS_FALSE,
      SECONDS(10)
    );


    // Cancel our task if we have no WiFi connection
    EventBits_t eventGroupBits = xEventGroupGetBits(xWiFiEventGroup);
    if ((eventGroupBits & BIT_CONNECTED) == 0) {
      ESP_LOGE(TAG, "WiFi: Not found, cancelling!");
      vTaskDelete(NULL);
      return;
    } else {
      ESP_LOGI(TAG, "Module started successfully!");
    }
    

    // Set up mDNS broadcasting
    ESP_ERROR_CHECK(mdns_init(TCPIP_ADAPTER_IF_STA, &mdns));
    mdns_set_hostname(mdns, "wroomba");
    mdns_set_instance(mdns, "wroomba");


    // Set up UART transmission and receiving
    uart_config_t cfg = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .rx_flow_ctrl_thresh = 0
    };


    for(;;) {
      xEventGroupWaitBits(
        xCleanEventGroup,
        (BIT_START_CLEAN | BIT_STOP_CLEAN),
        CLEAR_BITS_FALSE,
        WAIT_ALL_BITS_FALSE,
        SECONDS(60)
      );


      EventBits_t eventGroupBits = xEventGroupGetBits(xCleanEventGroup);
      if ((eventGroupBits & BIT_START_CLEAN) == 1) {
        ESP_LOGI(TAG, "Sending Command: Start Cleaning!");
      }

      if ((eventGroupBits & BIT_STOP_CLEAN) == 1) {
        ESP_LOGI(TAG, "Sending Command: Stop Cleaning!");
      }

      xEventGroupClearBits(xCleanEventGroup, (BIT_START_CLEAN | BIT_STOP_CLEAN));
    }

    vTaskDelete(NULL);
}
