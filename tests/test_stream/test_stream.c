#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lwjson/lwjson.h"
#include "test.h"

#define RUN_TEST(exp_res_must_be_true)                                                                                 \
    if ((exp_res_must_be_true)) {                                                                                      \
        ++test_passed;                                                                                                 \
    } else {                                                                                                           \
        ++test_failed;                                                                                                 \
        printf("Test failed on line %d\r\n", __LINE__);                                                                \
    }

/* Not implemented yet */

/**
 * \brief           Setup the data type to hold values
 */
typedef struct {
    int numbers_array[3];
    int numbers_obj_num1;
    int numbers_obj_num2;
    int numbers_array_array[2][3];

    float numbers_real;

    char long_string[512];
} parsed_data_t;

#define LONG_STRING_TEST                                                                                               \
    "this is a very long string because why we wouldn't do it if we can and because this has to be tested tested"

/* JSON string to parse */
static const char* json_stream_to_parse = "\
{\
    \"numbers_array\": [123, -123, 987],\
    \"numbers_obj\": {\
        \"num1\": 123, \
        \"num2\": 456, \
    },\
    \"numbers_arr2\": [\
        [1, 2, 3],\
        [4, 5, 6]\
    ],\
    \"numbers_real\": 3.5,\
    \"long_string\":\"" LONG_STRING_TEST "\"\
}\
";

static lwjson_stream_parser_t parser;
static parsed_data_t parsed_data;

/**
 * \brief           Parser callback to process user data
 * 
 * \param           jsp 
 * \param           type 
 */
static void
prv_parser_callback(struct lwjson_stream_parser* jsp, lwjson_stream_type_t type) {
    /* To process array values */
    if (jsp->stack_pos == 3 && lwjson_stack_seq_3(jsp, 0, OBJECT, KEY, ARRAY)) {
        if (type == LWJSON_STREAM_TYPE_NUMBER) {
            if (strcmp(jsp->stack[1].meta.name, "numbers_array") == 0) {
                parsed_data.numbers_array[jsp->stack[2].meta.index] = strtol(jsp->data.prim.buff, NULL, 0);
            }
        }
    }

    /* Process keys */
    if (jsp->stack_pos == 4 && lwjson_stack_seq_4(jsp, 0, OBJECT, KEY, OBJECT, KEY)
        && type == LWJSON_STREAM_TYPE_NUMBER) {
        if (strcmp(jsp->stack[1].meta.name, "numbers_obj") == 0) {
            if (strcmp(jsp->stack[3].meta.name, "num1") == 0) {
                parsed_data.numbers_obj_num1 = strtol(jsp->data.prim.buff, NULL, 0);
            } else if (strcmp(jsp->stack[3].meta.name, "num2") == 0) {
                parsed_data.numbers_obj_num2 = strtol(jsp->data.prim.buff, NULL, 0);
            }
        }
    }

    /* Process the object that has 2 arrays with numbers*/
    if (jsp->stack_pos == 4 && lwjson_stack_seq_4(jsp, 0, OBJECT, KEY, ARRAY, ARRAY)
        && type == LWJSON_STREAM_TYPE_NUMBER) {
        if (strcmp(jsp->stack[1].meta.name, "numbers_arr2") == 0) {
            /* Store the value to the group array */
            parsed_data.numbers_array_array[jsp->stack[2].meta.index][jsp->stack[3].meta.index] =
                strtol(jsp->data.prim.buff, NULL, 0);
        }
    }

    if (jsp->stack_pos == 2 && lwjson_stack_seq_2(jsp, 0, OBJECT, KEY)) {
        /* Parse number */
        if (type == LWJSON_STREAM_TYPE_NUMBER) {
            if (strcmp(jsp->stack[1].meta.name, "numbers_real") == 0) {
                parsed_data.numbers_real = (float)strtod(jsp->data.prim.buff, NULL);
            }
#if 1
        } else if (type == LWJSON_STREAM_TYPE_STRING) {
            if (strcmp(jsp->stack[1].meta.name, "long_string") == 0) {
                strncat(parsed_data.long_string, jsp->data.str.buff, jsp->data.str.buff_pos);
            }
#endif
        }
    }
}

int
test_run(void) {
    size_t test_failed = 0, test_passed = 0;

    printf("---\r\nTest JSON stream\r\n");

    memset(&parsed_data, 0x00, sizeof(parsed_data));
    lwjson_stream_init(&parser, prv_parser_callback);

    /* Run the parser through all */
    const char* ptr = json_stream_to_parse;
    printf("Starting the parser...\r\n");
    while (*ptr) {
        lwjsonr_t res = lwjson_stream_parse(&parser, *ptr++);
        if (res == lwjsonSTREAMDONE) {
            break;
        } else if (res == lwjsonERR) {
            printf("Stream returned error\r\n");
            break;
        }
    }
    printf("Parser completed\r\n");
    printf("Analyze the data here\r\n");

    /* Simple array */
    RUN_TEST(parsed_data.numbers_array[0] == 123);
    RUN_TEST(parsed_data.numbers_array[1] == -123);
    RUN_TEST(parsed_data.numbers_array[2] == 987);

    /* A value in a key */
    RUN_TEST(parsed_data.numbers_obj_num1 == 123);
    RUN_TEST(parsed_data.numbers_obj_num2 == 456);

    /* 2 array set */
    RUN_TEST(parsed_data.numbers_array_array[0][0] == 1);
    RUN_TEST(parsed_data.numbers_array_array[0][1] == 2);
    RUN_TEST(parsed_data.numbers_array_array[0][2] == 3);
    RUN_TEST(parsed_data.numbers_array_array[1][0] == 4);
    RUN_TEST(parsed_data.numbers_array_array[1][1] == 5);
    RUN_TEST(parsed_data.numbers_array_array[1][2] == 6);

    /* Check for real number */
    RUN_TEST(FLOAT_IS_EQUAL(parsed_data.numbers_real, 3.5f));

    /* Check string */
    RUN_TEST(strcmp(LONG_STRING_TEST, parsed_data.long_string) == 0);

    return test_failed > 0 ? -1 : 0;
}