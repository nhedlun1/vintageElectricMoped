#include <FlexCAN.h>
#include <kinetis_flexcan.h>
#include "can_msg.h"
#include "nextion_msg_names.h"
#include "Metro.h"

#define DEBUG_LVL 0

#define WHEEL_RADIUS 0.25f  //Diameter of rear wheel in m
#define CIRCUMFERENCE 1.57f //PI * 2 * WHEEL_RADIUS
#define MINUTES_IN_HOUR 60
#define KILOMETER 1000
#define GEAR_RATIO 16     //One turn of the rear wheel gives this many turns on motor.
#define MOTOR_POLEPAIRS 7 //14 motor poles gives 7 motor pole pairs.

#define debug Serial
#define display Serial1
#define LIGHT_PIN 5

#define IN_DATA_SIZE 50

//esp32 two wire automotive chapter 21 in esp32 technical reference manual

// create CAN object
FlexCAN CANReceiver(500000);
static CAN_message_t msg;

Metro Update_display = Metro(100); //Metro timer object, used to send data periodically to nextiondisplay.

void setDisp(char *msg);
void setDisp(const char *var, char *msg);
void setDisp(const char *var, int msg);
void evalInData(String in_data);

String g_in_data = "";
int g_curr_page = MAIN;
int8_t g_light_state = 0;

void setup()
{
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);

  g_in_data.reserve(50);

  debug.begin(115200);
  display.begin(9600);
  // setDisp("bauds=9600");

  CANReceiver.begin(); // init CAN bus
  delay(5000);
#if DEBUG_LVL == 0
  debug.println("CAN Receiver Initialized");
#endif
  setDisp(MAIN_SPEED_DISP, 999);
  setDisp(MAIN_RPM_DISP, 1234);
}

void loop()
{
  static volatile int16_t duty = 0;
  static volatile int16_t current = 0;
  static volatile int32_t erpm = 0;

  static volatile float amp_hours = 0.0f;
  static volatile float amp_hours_charged = 0.0f;

  static volatile float watt_hours = 0.0f;
  static volatile float watt_hours_charged = 0.0f;

  static volatile int16_t temp_fet = 0;
  static volatile int16_t temp_motor = 0;
  static volatile int16_t current_in = 0;
  static volatile int16_t pid_pos_now = 0;

  static volatile int16_t voltage = 0;
  static volatile int32_t tacho_value = 0;

  if (CANReceiver.available())
  {
    while (CANReceiver.read(msg))
    {
      static bool state = false;
      digitalWrite(LED_BUILTIN, state);
      state = !state;
      switch (msg.id)
      {
      case MSG_ID_1:
        erpm = (msg.buf[3] | (msg.buf[2] << 8) | (msg.buf[1] << 16) | (msg.buf[0] << 24)); // *((int32_t *)msg.buf);
        current = ((msg.buf[4] << 8) | msg.buf[5]);
        duty = ((msg.buf[6] << 8) | msg.buf[7]);
        break;

      case MSG_ID_2:
        amp_hours = (msg.buf[3] | (msg.buf[2] << 8) | (msg.buf[1] << 16) | (msg.buf[0] << 24));
        amp_hours_charged = (msg.buf[7] | (msg.buf[6] << 8) | (msg.buf[5] << 16) | (msg.buf[4] << 24));
        break;

      case MSG_ID_3:
        watt_hours = (msg.buf[3] | (msg.buf[2] << 8) | (msg.buf[1] << 16) | (msg.buf[0] << 24));
        watt_hours_charged = (msg.buf[7] | (msg.buf[6] << 8) | (msg.buf[5] << 16) | (msg.buf[4] << 24));
        break;

      case MSG_ID_4:
        temp_fet = (msg.buf[0] << 8) | msg.buf[1];
        temp_motor = (msg.buf[2] << 8) | msg.buf[3];
        current_in = (msg.buf[4] << 8) | msg.buf[5];
        pid_pos_now = ((msg.buf[6] << 8) | msg.buf[7]);
        break;

      case MSG_ID_5:
        voltage = ((msg.buf[4] << 8) | msg.buf[5]);
        tacho_value = (msg.buf[3] | (msg.buf[2] << 8) | (msg.buf[1] << 16) | (msg.buf[0] << 24));
        break;
      }
#if DEBUG_LVL == 2
      debug.printf("Status1 = erpm:% 7d  rpm:% 4d current:% 4.2f duty:% 3d%%\n", erpm, erpm / 7, (float)current / 10, duty / 10);
      debug.printf("Status2 = amp_hours:% 6.3f amp_hours_charged:% 6.3f\n", amp_hours / 10000, amp_hours_charged / 10000);
      debug.printf("Status3 = watt_hours:%.2f watt_hours_charged:%.2f\n", watt_hours / 10000, watt_hours_charged / 10000);
      debug.printf("Status4 = temp_fet:% 6.3f temp_motor: % 6.3f current_in: % 6.3f pid_pos_now: %5d\n", (float)temp_fet / 10.f, (float)temp_motor / 10.0f, (float)current_in / 10.0f, pid_pos_now / 50);
      debug.printf("Status5 = voltage:%.2f tacho_value:% 10d\n", (float)voltage / 10.0f, tacho_value);
#endif
    }
  }

  if (Update_display.check())
  {
#if DEBUG_LVL == 2
    debug.print("updating display");
#endif
    static int count = 0;
    switch (g_curr_page) //Switch on the page, only update visible values.
    {
    case MAIN:
      switch (count)
      {
      case 0:
        //50000erpm/7polepairs=7142rpm_motor 7142/16(gear ratio)=446rpm_rear wheel
        //446*1,57m = 700m/minute *60 = 42000m/hour / 1000 = 42km/h
        setDisp(MAIN_SPEED_DISP, ((((erpm / MOTOR_POLEPAIRS) / GEAR_RATIO) * CIRCUMFERENCE) * MINUTES_IN_HOUR) / KILOMETER);
        count++;
        break;

      case 1:
        setDisp(MAIN_BATT_AMP_DISP, current_in);
        count++;
        break;

      case 2:
        setDisp(MAIN_FET_TEMP_DISP, temp_fet);
        count++;
        break;

      case 3:
        setDisp(MAIN_BATT_VOLTS_DISP, voltage);
        count++;
        break;

      case 4:
        setDisp(MAIN_RPM_DISP, erpm / MOTOR_POLEPAIRS);
        count = 0;
        break;

      default:
        count = 0;
        break;
      }
      break;

    case LIST:
      switch (count)
      {
      case 0:
        //50000erpm/7polepairs=7142rpm_motor 7142/16(gear ratio)=446rpm_rear wheel
        //446*1,57m = 700m/minute *60 = 42000m/hour / 1000 = 42km/h,
        setDisp(LIST_DUTY_DISP, duty / 10);
        count++;
        break;

      case 1:
        setDisp(LIST_MOTOR_AMP_DISP, current);
        count++;
        break;

      case 2:
        setDisp(LIST_ERPM_DISP, erpm);
        count++;
        break;

      case 3:
        setDisp(LIST_AMPS_USED_DISP, amp_hours / 1000.0f);
        count++;
        break;

      case 4:
        setDisp(LIST_AMPS_CHARGED_DISP, amp_hours_charged / 1000.0f);
        count++;
        break;

      case 5:
        setDisp(LIST_WATTS_USED_DISP, watt_hours / 1000.0f);
        count++;
        break;

      case 6:
        setDisp(LIST_WATTS_CHARGED_DISP, watt_hours_charged / 1000.0f);
        count++;
        break;

      case 7:
        setDisp(LIST_TEMP_FETS_DISP, temp_fet);
        count++;
        break;

      case 8:
        setDisp(LIST_TEMP_MOTOR_DISP, temp_motor);
        count++;
        break;

      case 9:
        setDisp(LIST_BATT_AMPS_DISP, current_in);
        count++;
        break;

      case 10:
        setDisp(LIST_PID_POS_DISP, pid_pos_now);
        count++;
        break;

      case 11:
        setDisp(LIST_BATT_VOLT_DISP, voltage);
        count++;
        break;

      case 12:
        setDisp(LIST_TACH_VAL_DISP, tacho_value / 10);
        count = 0;
        break;

      default:
        count = 0;
        break;
      }
      break;

    case BATT:
      //TODO: IMPLEMENT DATA FROM BT CONNECTION TO ANT BMS SYSTEM HERE!
      break;
    }
  }

  if (display.available())
  {
    g_in_data = display.readStringUntil('\n');
#if DEBUG_LVL == 1
    debug.println(g_in_data);
#endif
    evalInData(g_in_data);
    g_in_data = "";
  }
}

void setDisp(const char *var, char *msg)
{
  for (int i = 0; i < 3; i++)
  {
    display.write(0xFF);
  }
  display.printf("%s.txt=\"%s\"", var, msg);

  for (int i = 0; i < 3; i++)
  {
    display.write(0xFF);
  }
}

void setDisp(const char *var, int msg)
{
  for (int i = 0; i < 3; i++)
  {
    display.write(0xFF);
  }
  display.printf("%s.val=%d", var, msg);

  for (int i = 0; i < 3; i++)
  {
    display.write(0xFF);
  }
}

void setDisp(char *msg)
{
  for (int i = 0; i < 3; i++)
  {
    display.write(0xFF);
  }
  display.print(msg);

  for (int i = 0; i < 3; i++)
  {
    display.write(0xFF);
  }
}

void evalInData(String in_data)
{
  in_data = in_data.trim();

  if (in_data.equals("b=light"))
  {
    g_light_state = (g_light_state == 0) ? 1 : 0;
    setDisp(MAIN_LIGHT_BTN_DISP, g_light_state);
    digitalWrite(LIGHT_PIN, g_light_state);
  }
  else if (in_data.equals("p=main"))
  {
    g_curr_page = MAIN;
  }
  else if (in_data.equals("p=list"))
  {
    g_curr_page = LIST;
  }
  else if (in_data.equals("p=bat"))
  {
    g_curr_page = BATT;
  }
}
