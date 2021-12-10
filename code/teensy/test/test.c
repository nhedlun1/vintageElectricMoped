#include "unity.h"
#include <stdio.h>
#include <stdint.h>

typedef struct msg
{
    volatile int32_t erpm;
    volatile int16_t current;
    volatile int8_t duty;
} can_msg;

typedef struct bits
{
    int erpm : 32;
    int current : 16;
    int duty : 8;
} can_field;

typedef union
{
    uint64_t data;
    uint8_t byte[8];
    uint16_t word[4];
    uint32_t dword[2];
} can_msg_t;

uint8_t array[4] = {0x12, 0x34, 0x56, 0x78};
int8_t raw_can[] = {0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00001000, 0b00000000, 0b01111111};

can_msg test_msg = {0, 0, 0};
can_field test_field = {0, 0, 0};
int32_t value = 0;

uint64_t get_signal(uint8_t *buf, uint8_t start, uint8_t bytes)
{
    uint64_t value = 0;

    for (uint8_t i = start; i < bytes; i++)
    {
        value = (value << 8) | buf[i];
    }

    return value;
}

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    printf("\n\n0x%X\n", value);
    printf("struct   contains=0x%X,0x%X,0x%X\n", test_msg.erpm, test_msg.current, test_msg.duty);
    printf("bitfield contains=0x%X,0x%X,0x%X\n", test_field.erpm, test_field.current, test_field.duty);
    printf("msg contains=%d,%d,%d\n", test_msg.erpm, test_msg.current, test_msg.duty);
}

void test_structs(void)
{
    test_msg = *(can_msg *)raw_can;
    test_field = *(can_field *)raw_can;
    value = *(int32_t *)array;

    TEST_ASSERT_EQUAL(255, test_msg.erpm);
    TEST_ASSERT_EQUAL(8, test_msg.current);
    TEST_ASSERT_EQUAL(127, test_msg.duty);
}

void test_union(void)
{
    uint8_t buffer[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
    can_msg_t msg = {msg.data = (*(uint64_t *)buffer)};
    printf("word[2] = 0x%x\n", msg.word[1]);
    printf("byte = 0x%x\n", msg.byte[0]);
    printf("doubleWord = 0x%x", msg.dword[2]);
}

int main(void)
{
    UNITY_BEGIN();
    //RUN_TEST(test_structs);
    RUN_TEST(test_union);
    return UNITY_END();
}