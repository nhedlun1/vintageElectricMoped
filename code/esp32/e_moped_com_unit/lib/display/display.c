#include <string.h>
#include <ctype.h>
#include "buzzer_driver.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "common.h"
#include "display.h"
#include "io.h"
#include "login_code.h"

#define UART_2_RTS (UART_PIN_NO_CHANGE)
#define UART_2_CTS (UART_PIN_NO_CHANGE)
#define UART_2_PORT_NUM (2)
#define UART_2_BAUD_RATE (9600)
#define BUF_SIZE (1024)
#define DISPLAY_TASK_STACK_SIZE (1024)

static status_t display_status = STATUS_UNINITIALIZED;
static page_t curr_page = PAGE_LOGIN;
static bool is_unlocked = false;
static uint8_t code[4] = {0, 0, 0, 0};
static uint16_t temp_code = 0;
static bool data_available = false;

//Pointer to the vesc struct used to store data from vesc motor controller.
static Vesc_data_t *vesc = NULL;

//function prototypes:
void display_task(void *pvParameters);
void set_disp(const char *var, int msg);
void set_disp_message(const char *string);
void set_disp_txt(const char *string, int val);
void eval_in_data(char *in_data);

bool display_init(Vesc_data_t *vesc_ptr)
{
    bool return_val = false;
    int intr_alloc_flags = 0;
    vesc = vesc_ptr;

    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    static uart_config_t uart_config = {
        .baud_rate = UART_2_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    /*uart_driver_delete(UART_2_PORT_NUM) == ESP_OK &&*/ // TODO: REMOVE THIS LINE Removed as a test, seems like it's ok when using esp32_dev as board
    if (uart_driver_install(UART_2_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags) == ESP_OK &&
        uart_param_config(UART_2_PORT_NUM, &uart_config) == ESP_OK &&
        uart_set_pin(UART_2_PORT_NUM, UART_2_TXD_PIN, UART_2_RXD_PIN, UART_2_RTS, UART_2_CTS) == ESP_OK)
    {
        return_val = true;
    }

    set_disp_message("page login");

    return return_val;
}

status_t display_start_task()
{
    xTaskCreate(display_task, "display_task", DISPLAY_TASK_STACK_SIZE * 4, NULL, 10, NULL);
    vTaskDelay(100 / portTICK_PERIOD_MS); //delay to give task a chance to start and change status.
    return display_status;
}

void display_task(void *pvParameters)
{
    display_status = STATUS_OK;
    char *data = (char *)malloc(BUF_SIZE);
    static Vesc_data_t vesc_cpy;

    while (1)
    {
        // Read data from the UART
        uint16_t len = uart_read_bytes(UART_2_PORT_NUM, data, BUF_SIZE, 10 / portTICK_RATE_MS);

        if (len > 1)
        {
            data[len] = '\0';
            data_available = true;
            eval_in_data(data);
        }

        //try to take mutex and write data to the vesc struct
        if (xSemaphoreTake(vesc->mutex, (TickType_t)10) == pdTRUE)
        {
            vesc_cpy.current = vesc->current;
            vesc_cpy.current_in = vesc->current_in;
            vesc_cpy.duty = vesc->duty;
            vesc_cpy.erpm = vesc->erpm;
            vesc_cpy.pid_pos_now = vesc->pid_pos_now;
            vesc_cpy.tacho_value = vesc->tacho_value;
            vesc_cpy.temp_fet = vesc->temp_fet;
            vesc_cpy.temp_motor = vesc->temp_motor;
            vesc_cpy.voltage = vesc->voltage;
            vesc_cpy.watt_hours = vesc->watt_hours;
            vesc_cpy.watt_hours_charged = vesc->watt_hours_charged;
            xSemaphoreGive(vesc->mutex); //Release the mutex
        }
        else
        {
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
        switch (curr_page)
        {
        case PAGE_MAIN:
            set_disp(MAIN_BATT_VOLTS_DISP, vesc_cpy.voltage);
            set_disp(MAIN_BATT_AMP_DISP, vesc_cpy.current);
            set_disp(MAIN_SPEED_DISP, ((((vesc_cpy.erpm / MOTOR_POLEPAIRS) / GEAR_RATIO) * CIRCUMFERENCE) * MINUTES_IN_HOUR) / KILOMETER);
            set_disp(MAIN_RPM_DISP, vesc_cpy.erpm / MOTOR_POLEPAIRS);
            set_disp(MAIN_FET_TEMP_DISP, vesc_cpy.temp_fet);
            set_disp(MAIN_MOT_TEMP_DISP, vesc_cpy.temp_motor);
            break;
        case PAGE_LIST:
            set_disp(LIST_MOTOR_AMP_DISP, vesc_cpy.current);
            set_disp(LIST_ERPM_DISP, vesc_cpy.erpm);
            set_disp(LIST_AMPS_USED_DISP, vesc_cpy.amp_hours / 1000.0f);
            set_disp(LIST_AMPS_CHARGED_DISP, vesc_cpy.amp_hours_charged / 1000.0f);
            set_disp(LIST_WATTS_USED_DISP, vesc_cpy.watt_hours / 1000.0f);
            set_disp(LIST_WATTS_CHARGED_DISP, vesc_cpy.watt_hours_charged / 1000.0f);
            set_disp(LIST_TEMP_FETS_DISP, vesc_cpy.temp_fet);
            set_disp(LIST_TEMP_MOTOR_DISP, vesc_cpy.temp_motor);
            set_disp(LIST_BATT_AMPS_DISP, vesc_cpy.current_in);
            set_disp(LIST_PID_POS_DISP, vesc_cpy.pid_pos_now);
            set_disp(LIST_BATT_VOLT_DISP, vesc_cpy.voltage);
            set_disp(LIST_TACH_VAL_DISP, vesc_cpy.tacho_value / 10);
            break;
        case PAGE_BAT:
            break;
        case PAGE_LOGIN:
            break;
        default:
            break;
        }
    }
}

void display_set_page(page_t page)
{
    curr_page = page;
}

page_t display_get_page(void)
{
    return curr_page;
}

status_t display_get_status(void)
{
    return display_status;
}

void set_disp(const char *var, int msg)
{
    const char clear[3] = {0xFF, 0xFF, 0xFF};
    char temp[15] = {0};

    uart_write_bytes(UART_2_PORT_NUM, clear, 3);
    sprintf(temp, "%s.val=%d", var, msg);
    int size = strlen(temp);
    uart_write_bytes(UART_2_PORT_NUM, temp, size);
    uart_write_bytes(UART_2_PORT_NUM, clear, 3);
}

void set_disp_message(const char *string)
{
    const char clear[3] = {0xFF, 0xFF, 0xFF};
    char temp[15] = {0};

    uart_write_bytes(UART_2_PORT_NUM, clear, 3);
    sprintf(temp, "%s", string);
    int size = strlen(temp);
    uart_write_bytes(UART_2_PORT_NUM, temp, size);
    uart_write_bytes(UART_2_PORT_NUM, clear, 3);
}

void set_disp_txt(const char *string, int val)
{
    const char clear[3] = {0xFF, 0xFF, 0xFF};
    char temp[15] = {0};

    uart_write_bytes(UART_2_PORT_NUM, clear, 3);
    sprintf(temp, "%s\"%d\"", string, val);
    int size = strlen(temp);
    uart_write_bytes(UART_2_PORT_NUM, temp, size);
    uart_write_bytes(UART_2_PORT_NUM, clear, 3);
}

bool code_add_digit(char digit)
{
    static uint8_t digit_placement = 0;
    if (temp_code == 0)
    {
        digit_placement = 0;
    }

    if (digit_placement > 3)
    {
        digit_placement = 0;
        temp_code = 0;
    }

    code[digit_placement] = digit;
    temp_code = ((temp_code * 10) + (code[digit_placement] - '0'));
    printf("tempCode:%d\n", temp_code);
    set_disp_txt("t0.txt=", temp_code);
    digit_placement++;
    return (temp_code == LOGIN_CODE) ? true : false;
}

void eval_in_data(char *in_data)
{
    printf("in_data:%s\n", in_data);
    if (strcmp("b=light", in_data) == 0)
    {
        static uint8_t light_state = 0;
        light_state = (light_state == 0) ? 1 : 0;
        set_disp(MAIN_LIGHT_BTN_DISP, light_state);
        io_set_front_light(light_state);
    }
    else if (strcmp("p=main", in_data) == 0)
    {
        display_set_page(PAGE_MAIN);
    }
    else if (strcmp("p=list", in_data) == 0)
    {
        display_set_page(PAGE_LIST);
        buzzer_play_sound(0);
    }
    else if (strcmp("p=bat", in_data) == 0)
    {
        display_set_page(PAGE_BAT);
        // buzzer_play_sound(1);
    }

    else if (strcmp("b=lock", in_data) == 0)
    {
        if (is_unlocked)
        {
            is_unlocked = false;
            display_set_page(PAGE_LOGIN);
            set_disp_message("page login");
        }
    }
    else if (in_data[0] == 'b' && isdigit(in_data[2]))
    {
        code_add_digit(in_data[2]);
    }
    else if (strcmp("b=del", in_data) == 0)
    {
        temp_code = 0;
        set_disp_message("t0.txt=\" \"");
    }

    else if (strcmp("b=unlock", in_data) == 0)
    {
        if (temp_code == LOGIN_CODE)
        {
            is_unlocked = true;
            set_disp_message("page main");
        }
        else
        {
            temp_code = 0;
            set_disp_message("t0.txt=\" \"");
        }
    }
}

bool display_is_unlocked(void)
{
    return is_unlocked;
}

bool display_data_available()
{
    return data_available;
}

void display_get_data(char *buffer)
{
    //Todo: add code for getting the incoming data here.
}