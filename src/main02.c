//
// Created by liwuen on 27/06/24.
//

#include <stdint.h>
#include <arm_neon.h>
#include <stdio.h>
#include <limits.h>




void printbits_64(uint64_t n){
    uint64_t i;
    i = 1UL<<(sizeof(n)*CHAR_BIT-1);
    int sizeOf = sizeof(n);
    printf("[");
    while(i>0){
        if(n&i)
            printf("1");
        else
            printf("0");
        i >>= 1;
    }
    printf("]");
}

void printbits_64x1_t(uint64x1_t n){
    printbits_64((uint64_t) n);
}

void printbits_32(uint32_t n){
    uint32_t i;
    i = 1UL<<(sizeof(n)*CHAR_BIT-1);
    printf("[");
    while(i>0){
        if(n&i)
            printf("1");
        else
            printf("0");
        i >>= 1;
    }
    printf("]");
}

//void test_addition_03(uint64_t* output,
//                      const uint64_t *input01_01, const uint64_t *input01_02, const uint64_t *input01_03, const uint64_t *input01_04,
//                      const uint64_t *input02_01, const uint64_t *input02_02, const uint64_t *input02_03, const uint64_t *input02_04,
//                      const uint64_t *modulo) {
//
//    uint64x1_t zero = {0};
//    uint64x1_t input01_01_loaded = vld1_u64(input01_01);
//    uint64x1_t input01_02_loaded = vld1_u64(input01_02);
//    uint64x1_t input01_03_loaded = vld1_u64(input01_03);
//    uint64x1_t input01_04_loaded = vld1_u64(input01_04);
//
//    uint64x1_t input01_01_high = vsli_n_u64(input01_01_loaded, zero, 32);
//    uint64x1_t input01_01_low = vsri_n_u64(zero, input01_01_loaded, 32);
//    uint64x1_t input01_02_high = vsli_n_u64(input01_02_loaded, zero, 32);
//    uint64x1_t input01_02_low = vsri_n_u64(zero, input01_02_loaded, 32);
//
//    printbits_64x1_t(input01_01_high);
//    printbits_64x1_t(input01_01_low);
//    printbits_64x1_t(input01_02_high);
//    printbits_64x1_t(input01_02_low);
//}

/*void test_addition_05(uint64_t* output, const uint64_t *input01, const uint64_t *input02, const uint64_t *modulo){

    uint64x1_t zero = {0};
    uint64x1x4_t input01_loaded; // Number of words, i.e., 4 words

    input01_loaded = vld4_u64(input01); //Creates one uint64x1x4_t out of 4 words in the input01.

    uint64x1_t input01_01_high = vsli_n_u64(input01_loaded.val[0],zero,32);
    uint64x1_t input01_01_low = vsri_n_u64(zero,input01_loaded.val[0], 32);
    uint64x2_t input01_01_high_low = vcombine_u64(input01_01_high, input01_01_low);


    uint64x1_t input01_02_high = vsli_n_u64(input01_loaded.val[1],zero,32);
    uint64x1_t input01_02_low = vsri_n_u64(zero,input01_loaded.val[1], 32);
    uint64x2_t input01_02_high_low = vcombine_u64(input01_02_high, input01_02_low);

    vst1q_u64(output, input01_01_high_low);
    vst1q_u64(output+128, input01_02_high_low);
}*/

void test_addition_06(uint64_t* output, const uint64_t *input01, const uint64_t *input02, const uint64_t *modulo) {

    uint64x1_t zero = {0};

    // INPUT 01

    uint64x1x4_t input01_loaded = vld4_u64(input01); //Creates one uint64x1x4_t out of 4 words in the input01.

    //To separate the 32 HIGH and LOW bits of a 64-bits word, we will use:
    //  1. SLI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsli_n_u64
    //  2. SRI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsri_n_u64

    uint64x1_t input01_high_low[4][2];  //Each word is of the form XX..XYY..Y, where XX...X are the 32 HIGH bits, and YY...Y are the 32 LOW bits.
    input01_high_low[0][0] = vsli_n_u64(input01_loaded.val[0], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[0][1] = vsri_n_u64(zero, input01_loaded.val[0],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_high_low[1][0] = vsli_n_u64(input01_loaded.val[1], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[1][1] = vsri_n_u64(zero, input01_loaded.val[1],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_high_low[2][0] = vsli_n_u64(input01_loaded.val[2], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[2][1] = vsri_n_u64(zero, input01_loaded.val[2],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_high_low[3][0] = vsli_n_u64(input01_loaded.val[3], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[3][1] = vsri_n_u64(zero, input01_loaded.val[3],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    uint64x2_t input01_words_padded[4]; //We combine two 64x1_t words (low - high) into a single 64x2_t word (high - low) - https://developer.arm.com/architectures/instruction-sets/intrinsics/vcombine_u64
    input01_words_padded[0] = vcombine_u64(input01_high_low[0][1],
                                           input01_high_low[0][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input01_words_padded[1] = vcombine_u64(input01_high_low[1][1],
                                           input01_high_low[1][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input01_words_padded[2] = vcombine_u64(input01_high_low[2][1],
                                           input01_high_low[2][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input01_words_padded[3] = vcombine_u64(input01_high_low[3][1],
                                           input01_high_low[3][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]


    // INPUT 02

    uint64x1x4_t input02_loaded = vld4_u64(input02); //Creates one uint64x1x4_t out of 4 words in the input01.

    //To separate the 32 HIGH and LOW bits of a 64-bits word, we will use:
    //  1. SLI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsli_n_u64
    //  2. SRI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsri_n_u64

    uint64x1_t input02_high_low[4][2];  //Each word is of the form XX..XYY..Y, where XX...X are the 32 HIGH bits, and YY...Y are the 32 LOW bits.
    input02_high_low[0][0] = vsli_n_u64(input02_loaded.val[0], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input02_high_low[0][1] = vsri_n_u64(zero, input02_loaded.val[0],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input02_high_low[1][0] = vsli_n_u64(input02_loaded.val[1], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input02_high_low[1][1] = vsri_n_u64(zero, input02_loaded.val[1],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input02_high_low[2][0] = vsli_n_u64(input02_loaded.val[2], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input02_high_low[2][1] = vsri_n_u64(zero, input02_loaded.val[2],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input02_high_low[3][0] = vsli_n_u64(input02_loaded.val[3], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input02_high_low[3][1] = vsri_n_u64(zero, input02_loaded.val[3],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    uint64x2_t input02_words_padded[4]; //We combine two 64x1_t words (low - high) into a single 64x2_t word (high - low) - https://developer.arm.com/architectures/instruction-sets/intrinsics/vcombine_u64
    input02_words_padded[0] = vcombine_u64(input02_high_low[0][1],
                                           input02_high_low[0][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input02_words_padded[1] = vcombine_u64(input02_high_low[1][1],
                                           input02_high_low[1][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input02_words_padded[2] = vcombine_u64(input02_high_low[2][1],
                                           input02_high_low[2][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input02_words_padded[3] = vcombine_u64(input02_high_low[3][1],
                                           input02_high_low[3][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]

    // FIRST ADDITION: ADD IN PARALLEL

    // We need 4 additions (each word_padded is 64x2 - input01 + input02 - and we have four words)
    // We use:
    //      uint64x2_t vaddq_u64 (uint64x2_t a, uint64x2_t b)
    // https://developer.arm.com/architectures/instruction-sets/intrinsics/vaddq_u64

    uint64x2_t addition_in_parallel[4];

    addition_in_parallel[0] = vaddq_u64(input01_words_padded[0], input02_words_padded[0]);
    addition_in_parallel[1] = vaddq_u64(input01_words_padded[1], input02_words_padded[1]);
    addition_in_parallel[2] = vaddq_u64(input01_words_padded[2], input02_words_padded[2]);
    addition_in_parallel[3] = vaddq_u64(input01_words_padded[3], input02_words_padded[3]);

    uint32x4_t addition_in_parallel_result_recast_reversed[4];

    addition_in_parallel_result_recast_reversed[0]=vreinterpretq_u32_u64(addition_in_parallel[0]);
    addition_in_parallel_result_recast_reversed[1]=vreinterpretq_u32_u64(addition_in_parallel[1]);
    addition_in_parallel_result_recast_reversed[2]=vreinterpretq_u32_u64(addition_in_parallel[2]);
    addition_in_parallel_result_recast_reversed[3]=vreinterpretq_u32_u64(addition_in_parallel[3]);

    uint32x4_t addition_in_parallel_result_recast[4];
    addition_in_parallel_result_recast[0]=vrev64q_u32(addition_in_parallel_result_recast_reversed[0]);
    addition_in_parallel_result_recast[1]=vrev64q_u32(addition_in_parallel_result_recast_reversed[1]);
    addition_in_parallel_result_recast[2]=vrev64q_u32(addition_in_parallel_result_recast_reversed[2]);
    addition_in_parallel_result_recast[3]=vrev64q_u32(addition_in_parallel_result_recast_reversed[3]);

    uint32_t addition_in_parallel_streamed[17]={0}; //17 = 2*8+1 = 2*(4 words of 64 bits split in 2 words of 32 bits)+1 word of 32 bits leading zero
    vst1q_u32(addition_in_parallel_streamed+1,addition_in_parallel_result_recast[0]);
    vst1q_u32(addition_in_parallel_streamed+5,addition_in_parallel_result_recast[1]);
    vst1q_u32(addition_in_parallel_streamed+9,addition_in_parallel_result_recast[2]);
    vst1q_u32(addition_in_parallel_streamed+13,addition_in_parallel_result_recast[3]);

//    for(int i=0;i<17;i++){
//        addition_in_parallel_streamed[i]=i;
//    }


    // SECOND ADDITION: ADD PAIR-WISE

    uint32x4_t addition_words_shifted[4];
    addition_words_shifted[0] = vld1q_u32(addition_in_parallel_streamed);
    addition_words_shifted[1] = vld1q_u32(addition_in_parallel_streamed+4);
    addition_words_shifted[2] = vld1q_u32(addition_in_parallel_streamed+8);
    addition_words_shifted[3] = vld1q_u32(addition_in_parallel_streamed+12);

    uint32x4_t addition_in_pairs[2];
    addition_in_pairs[0]=vpaddq_u32(addition_words_shifted[0],addition_words_shifted[1]);
    addition_in_pairs[1]=vpaddq_u32(addition_words_shifted[2],addition_words_shifted[3]);

    uint32_t result_halvess[10]={0}; // Index 0 = 0 ; Index 1 = carry-bit; Index 2-3 = result-word-1; Index 4-5 = result-word-2; Index 6-7 = result-word-3; Index 8-9 = result-word-4
    vst1q_u32(result_halvess+1, addition_in_pairs[0]);
    vst1q_u32(result_halvess+5, addition_in_pairs[1]);
    result_halvess[9]=addition_in_parallel_streamed[16];





}

void TII_KYBER_LW_pointwise_sub(int16_t *r, const int16_t *a, const int16_t *b) {
    int16x8x4_t rr, aa, bb;  // 16
    unsigned int i;

    for (i = 0; i < 25; i += 32) {
        aa = vld1q_s16_x4(&a[i]);
        bb = vld1q_s16_x4(&b[i]);

        // r = a - b;
        rr.val[0] = vsubq_s16(aa.val[0], bb.val[0]);
        rr.val[1] = vsubq_s16(aa.val[1], bb.val[1]);
        rr.val[2] = vsubq_s16(aa.val[2], bb.val[2]);
        rr.val[3] = vsubq_s16(aa.val[3], bb.val[3]);

        vst1q_s16_x4(&r[i], rr);
    }
}

void addition_256bits_carrying_03(uint64_t* output, uint64_t* carryOn,const uint64_t *input01, const uint64_t *input02) {

    uint64x1_t zero = {0};

    uint64x2_t input01_words_padded[4]; //We combine two 64x1_t words (low - high) into a single 64x2_t word (high - low) - https://developer.arm.com/architectures/instruction-sets/intrinsics/vcombine_u64

    {
        // INPUT 01
        uint64x1x4_t input01_loaded = vld4_u64(input01); //Creates one uint64x1x4_t out of 4 words in the input01.

        //To separate the 32 HIGH and LOW bits of a 64-bits word, we will use:
        //  1. SLI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsli_n_u64
        //  2. SRI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsri_n_u64

        uint64x1_t input01_high_low[4][2];  //Each word is of the form XX..XYY..Y, where XX...X are the 32 HIGH bits, and YY...Y are the 32 LOW bits.
        input01_high_low[0][0] = vsli_n_u64(input01_loaded.val[0], zero,
                                            32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
        input01_high_low[0][1] = vsri_n_u64(zero, input01_loaded.val[0],
                                            32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

        input01_high_low[1][0] = vsli_n_u64(input01_loaded.val[1], zero,
                                            32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
        input01_high_low[1][1] = vsri_n_u64(zero, input01_loaded.val[1],
                                            32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

        input01_high_low[2][0] = vsli_n_u64(input01_loaded.val[2], zero,
                                            32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
        input01_high_low[2][1] = vsri_n_u64(zero, input01_loaded.val[2],
                                            32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

        input01_high_low[3][0] = vsli_n_u64(input01_loaded.val[3], zero,
                                            32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
        input01_high_low[3][1] = vsri_n_u64(zero, input01_loaded.val[3],
                                            32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y


        input01_words_padded[0] = vcombine_u64(input01_high_low[0][1],
                                               input01_high_low[0][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
        input01_words_padded[1] = vcombine_u64(input01_high_low[1][1],
                                               input01_high_low[1][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
        input01_words_padded[2] = vcombine_u64(input01_high_low[2][1],
                                               input01_high_low[2][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
        input01_words_padded[3] = vcombine_u64(input01_high_low[3][1],

                                               input01_high_low[3][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    }
    // INPUT 02

    uint64x2_t input02_words_padded[4]; //We combine two 64x1_t words (low - high) into a single 64x2_t word (high - low) - https://developer.arm.com/architectures/instruction-sets/intrinsics/vcombine_u64

    {
        uint64x1x4_t input02_loaded = vld4_u64(input02); //Creates one uint64x1x4_t out of 4 words in the input01.

        //To separate the 32 HIGH and LOW bits of a 64-bits word, we will use:
        //  1. SLI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsli_n_u64
        //  2. SRI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsri_n_u64

        uint64x1_t input02_high_low[4][2];  //Each word is of the form XX..XYY..Y, where XX...X are the 32 HIGH bits, and YY...Y are the 32 LOW bits.
        input02_high_low[0][0] = vsli_n_u64(input02_loaded.val[0], zero,
                                            32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
        input02_high_low[0][1] = vsri_n_u64(zero, input02_loaded.val[0],
                                            32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

        input02_high_low[1][0] = vsli_n_u64(input02_loaded.val[1], zero,
                                            32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
        input02_high_low[1][1] = vsri_n_u64(zero, input02_loaded.val[1],
                                            32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

        input02_high_low[2][0] = vsli_n_u64(input02_loaded.val[2], zero,
                                            32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
        input02_high_low[2][1] = vsri_n_u64(zero, input02_loaded.val[2],
                                            32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

        input02_high_low[3][0] = vsli_n_u64(input02_loaded.val[3], zero,
                                            32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
        input02_high_low[3][1] = vsri_n_u64(zero, input02_loaded.val[3],
                                            32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y


        input02_words_padded[0] = vcombine_u64(input02_high_low[0][1],
                                               input02_high_low[0][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
        input02_words_padded[1] = vcombine_u64(input02_high_low[1][1],
                                               input02_high_low[1][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
        input02_words_padded[2] = vcombine_u64(input02_high_low[2][1],
                                               input02_high_low[2][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
        input02_words_padded[3] = vcombine_u64(input02_high_low[3][1],
                                               input02_high_low[3][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    }
    // FIRST ADDITION: ADD IN PARALLEL

    // We need 4 additions (each word_padded is 64x2 - input01 + input02 - and we have four words)
    // We use:
    //      uint64x2_t vaddq_u64 (uint64x2_t a, uint64x2_t b)
    // https://developer.arm.com/architectures/instruction-sets/intrinsics/vaddq_u64

    uint64x2_t addition_in_parallel[4];

    addition_in_parallel[0] = vaddq_u64(input01_words_padded[0], input02_words_padded[0]);
    addition_in_parallel[1] = vaddq_u64(input01_words_padded[1], input02_words_padded[1]);
    addition_in_parallel[2] = vaddq_u64(input01_words_padded[2], input02_words_padded[2]);
    addition_in_parallel[3] = vaddq_u64(input01_words_padded[3], input02_words_padded[3]);

    uint32x4_t addition_in_parallel_result_recast_reversed[4];

    addition_in_parallel_result_recast_reversed[0]=vreinterpretq_u32_u64(addition_in_parallel[0]);
    addition_in_parallel_result_recast_reversed[1]=vreinterpretq_u32_u64(addition_in_parallel[1]);
    addition_in_parallel_result_recast_reversed[2]=vreinterpretq_u32_u64(addition_in_parallel[2]);
    addition_in_parallel_result_recast_reversed[3]=vreinterpretq_u32_u64(addition_in_parallel[3]);

    uint32x4_t addition_in_parallel_result_recast[4];
    addition_in_parallel_result_recast[0]=vrev64q_u32(addition_in_parallel_result_recast_reversed[0]);
    addition_in_parallel_result_recast[1]=vrev64q_u32(addition_in_parallel_result_recast_reversed[1]);
    addition_in_parallel_result_recast[2]=vrev64q_u32(addition_in_parallel_result_recast_reversed[2]);
    addition_in_parallel_result_recast[3]=vrev64q_u32(addition_in_parallel_result_recast_reversed[3]);

    uint32_t addition_in_parallel_streamed[17]={0}; //17 = 2*8+1 = 2*(4 words of 64 bits split in 2 words of 32 bits)+1 word of 32 bits leading zero
    vst1q_u32(addition_in_parallel_streamed+1,addition_in_parallel_result_recast[0]);
    vst1q_u32(addition_in_parallel_streamed+5,addition_in_parallel_result_recast[1]);
    vst1q_u32(addition_in_parallel_streamed+9,addition_in_parallel_result_recast[2]);
    vst1q_u32(addition_in_parallel_streamed+13,addition_in_parallel_result_recast[3]);

    // SECOND ADDITION: ADD PAIR-WISE

    uint32x4_t addition_words_shifted[4];
    addition_words_shifted[0] = vld1q_u32(addition_in_parallel_streamed);
    addition_words_shifted[1] = vld1q_u32(addition_in_parallel_streamed+4);
    addition_words_shifted[2] = vld1q_u32(addition_in_parallel_streamed+8);
    addition_words_shifted[3] = vld1q_u32(addition_in_parallel_streamed+12);

    uint32x4_t addition_in_pairs[2];
    addition_in_pairs[0]=vpaddq_u32(addition_words_shifted[0],addition_words_shifted[1]);
    addition_in_pairs[1]=vpaddq_u32(addition_words_shifted[2],addition_words_shifted[3]);

    uint32_t result_halvess[10]={0}; // Index 0 = 0 ; Index 1 = carry-bit; Index 2-3 = result-word-1; Index 4-5 = result-word-2; Index 6-7 = result-word-3; Index 8-9 = result-word-4
    vst1q_u32(result_halvess+1, addition_in_pairs[0]);
    vst1q_u32(result_halvess+5, addition_in_pairs[1]);
    result_halvess[9]=addition_in_parallel_streamed[16];    // Last position is the 32 least significant bits of the original 4x64-bits parallel addition.

    /*uint32x2_t result_concat[5];
    result_concat[0]=vld1_u32(result_halvess); // For uint32x2_t, use vld1_u32, for uint32x4_t, use uint32x4_t vld1q_u32(uint32_t const *ptr)
    result_concat[1]=vld1_u32(result_halvess+2);
    result_concat[2]=vld1_u32(result_halvess+4);
    result_concat[3]=vld1_u32(result_halvess+6);
    result_concat[4]=vld1_u32(result_halvess+8);

    uint64_t result_recast[5];
    result_recast[0]=vreinterpret_u64_u32(result_concat[0]);
    result_recast[1]=vreinterpret_u64_u32(result_concat[1]);
    result_recast[2]=vreinterpret_u64_u32(result_concat[2]);
    result_recast[3]=vreinterpret_u64_u32(result_concat[3]);
    result_recast[4]=vreinterpret_u64_u32(result_concat[4]);

    uint64_t results_combined[5];
    results_combined[0]=(uint64_t) result_halvess[0]<<32 | result_halvess[1];
    results_combined[1]=(uint64_t) result_halvess[2]<<32 | result_halvess[3];
    results_combined[2]=(uint64_t) result_halvess[4]<<32 | result_halvess[5];
    results_combined[3]=(uint64_t) result_halvess[6]<<32 | result_halvess[7];
    results_combined[4]=(uint64_t) result_halvess[8]<<32 | result_halvess[9];*/

//    *(output+0)=(uint64_t) result_halvess[0]<<32 | result_halvess[1];
//    *(output+1)=(uint64_t) result_halvess[2]<<32 | result_halvess[3];
//    *(output+2)=(uint64_t) result_halvess[4]<<32 | result_halvess[5];
//    *(output+3)=(uint64_t) result_halvess[6]<<32 | result_halvess[7];
//    *(output+4)=(uint64_t) result_halvess[8]<<32 | result_halvess[9];

    uint32x2_t result_concat[5];
    result_concat[0]=vld1_u32(result_halvess); // For uint32x2_t, use vld1_u32, for uint32x4_t, use uint32x4_t vld1q_u32(uint32_t const *ptr)
    result_concat[1]=vld1_u32(result_halvess+2);
    result_concat[2]=vld1_u32(result_halvess+4);
    result_concat[3]=vld1_u32(result_halvess+6);
    result_concat[4]=vld1_u32(result_halvess+8);

    uint64x1_t carryOnWord = vreinterpret_u64_u32(result_concat[0]);
    vst1_u64(carryOn,carryOnWord);

//    uint64x1_t outputWords[4];
//    outputWords[0]=vreinterpret_u64_u32(result_concat[0]);
//    outputWords[1]=vreinterpret_u64_u32(result_concat[1]);
//    outputWords[2]=vreinterpret_u64_u32(result_concat[2]);
//    outputWords[3]=vreinterpret_u64_u32(result_concat[3]);



    //vst4_u64(output, result_recast);
    vst1_u64(&output[0], vreinterpret_u64_u32(result_concat[0]));
    vst1_u64(&output[1], vreinterpret_u64_u32(result_concat[1]));
    vst1_u64(&output[2], vreinterpret_u64_u32(result_concat[2]));
    vst1_u64(&output[3], vreinterpret_u64_u32(result_concat[3]));


//    uint64x1x4_t result_recast;
//    //result_recast[0]=vreinterpret_u64_u32(result_concat[0]); // Skip first word, carry on
//    result_recast.val[0]=vreinterpret_u64_u32(result_concat[1]);
//    result_recast.val[1]=vreinterpret_u64_u32(result_concat[2]);
//    result_recast.val[2]=vreinterpret_u64_u32(result_concat[3]);
//    result_recast.val[3]=vreinterpret_u64_u32(result_concat[4]);
//
//    //vst4_u64(output, result_recast);
//    vst1_u64(output, result_recast.val[0]);
//    vst1_u64(output+1, result_recast.val[1]);
//    vst1_u64(output+2, result_recast.val[2]);
//    vst1_u64(output+3, result_recast.val[3]);






}


void addition_256bits_carrying(uint64_t* output, uint64_t* carryOn,const uint64_t *input01, const uint64_t *input02) {

    uint64x1_t zero = {0};

    // INPUT 01

    uint64x1x4_t input01_loaded = vld4_u64(input01); //Creates one uint64x1x4_t out of 4 words in the input01.

    //To separate the 32 HIGH and LOW bits of a 64-bits word, we will use:
    //  1. SLI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsli_n_u64
    //  2. SRI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsri_n_u64

    uint64x1_t input01_high_low[4][2];  //Each word is of the form XX..XYY..Y, where XX...X are the 32 HIGH bits, and YY...Y are the 32 LOW bits.
    input01_high_low[0][0] = vsli_n_u64(input01_loaded.val[0], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[0][1] = vsri_n_u64(zero, input01_loaded.val[0],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_high_low[1][0] = vsli_n_u64(input01_loaded.val[1], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[1][1] = vsri_n_u64(zero, input01_loaded.val[1],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_high_low[2][0] = vsli_n_u64(input01_loaded.val[2], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[2][1] = vsri_n_u64(zero, input01_loaded.val[2],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_high_low[3][0] = vsli_n_u64(input01_loaded.val[3], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[3][1] = vsri_n_u64(zero, input01_loaded.val[3],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    uint64x2_t input01_words_padded[4]; //We combine two 64x1_t words (low - high) into a single 64x2_t word (high - low) - https://developer.arm.com/architectures/instruction-sets/intrinsics/vcombine_u64
    input01_words_padded[0] = vcombine_u64(input01_high_low[0][1],
                                           input01_high_low[0][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input01_words_padded[1] = vcombine_u64(input01_high_low[1][1],
                                           input01_high_low[1][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input01_words_padded[2] = vcombine_u64(input01_high_low[2][1],
                                           input01_high_low[2][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input01_words_padded[3] = vcombine_u64(input01_high_low[3][1],
                                           input01_high_low[3][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]

    // INPUT 02

    uint64x1x4_t input02_loaded = vld4_u64(input02); //Creates one uint64x1x4_t out of 4 words in the input01.

    //To separate the 32 HIGH and LOW bits of a 64-bits word, we will use:
    //  1. SLI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsli_n_u64
    //  2. SRI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsri_n_u64

    uint64x1_t input02_high_low[4][2];  //Each word is of the form XX..XYY..Y, where XX...X are the 32 HIGH bits, and YY...Y are the 32 LOW bits.
    input02_high_low[0][0] = vsli_n_u64(input02_loaded.val[0], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input02_high_low[0][1] = vsri_n_u64(zero, input02_loaded.val[0],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input02_high_low[1][0] = vsli_n_u64(input02_loaded.val[1], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input02_high_low[1][1] = vsri_n_u64(zero, input02_loaded.val[1],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input02_high_low[2][0] = vsli_n_u64(input02_loaded.val[2], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input02_high_low[2][1] = vsri_n_u64(zero, input02_loaded.val[2],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input02_high_low[3][0] = vsli_n_u64(input02_loaded.val[3], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input02_high_low[3][1] = vsri_n_u64(zero, input02_loaded.val[3],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    uint64x2_t input02_words_padded[4]; //We combine two 64x1_t words (low - high) into a single 64x2_t word (high - low) - https://developer.arm.com/architectures/instruction-sets/intrinsics/vcombine_u64
    input02_words_padded[0] = vcombine_u64(input02_high_low[0][1],
                                           input02_high_low[0][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input02_words_padded[1] = vcombine_u64(input02_high_low[1][1],
                                           input02_high_low[1][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input02_words_padded[2] = vcombine_u64(input02_high_low[2][1],
                                           input02_high_low[2][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input02_words_padded[3] = vcombine_u64(input02_high_low[3][1],
                                           input02_high_low[3][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]

    // FIRST ADDITION: ADD IN PARALLEL

    // We need 4 additions (each word_padded is 64x2 - input01 + input02 - and we have four words)
    // We use:
    //      uint64x2_t vaddq_u64 (uint64x2_t a, uint64x2_t b)
    // https://developer.arm.com/architectures/instruction-sets/intrinsics/vaddq_u64

    uint64x2_t addition_in_parallel[4];

    addition_in_parallel[0] = vaddq_u64(input01_words_padded[0], input02_words_padded[0]);
    addition_in_parallel[1] = vaddq_u64(input01_words_padded[1], input02_words_padded[1]);
    addition_in_parallel[2] = vaddq_u64(input01_words_padded[2], input02_words_padded[2]);
    addition_in_parallel[3] = vaddq_u64(input01_words_padded[3], input02_words_padded[3]);

    uint32x4_t addition_in_parallel_result_recast_reversed[4];

    addition_in_parallel_result_recast_reversed[0]=vreinterpretq_u32_u64(addition_in_parallel[0]);
    addition_in_parallel_result_recast_reversed[1]=vreinterpretq_u32_u64(addition_in_parallel[1]);
    addition_in_parallel_result_recast_reversed[2]=vreinterpretq_u32_u64(addition_in_parallel[2]);
    addition_in_parallel_result_recast_reversed[3]=vreinterpretq_u32_u64(addition_in_parallel[3]);

    uint32x4_t addition_in_parallel_result_recast[4];
    addition_in_parallel_result_recast[0]=vrev64q_u32(addition_in_parallel_result_recast_reversed[0]);
    addition_in_parallel_result_recast[1]=vrev64q_u32(addition_in_parallel_result_recast_reversed[1]);
    addition_in_parallel_result_recast[2]=vrev64q_u32(addition_in_parallel_result_recast_reversed[2]);
    addition_in_parallel_result_recast[3]=vrev64q_u32(addition_in_parallel_result_recast_reversed[3]);

    uint32_t addition_in_parallel_streamed[17]={0}; //17 = 2*8+1 = 2*(4 words of 64 bits split in 2 words of 32 bits)+1 word of 32 bits leading zero
    vst1q_u32(addition_in_parallel_streamed+1,addition_in_parallel_result_recast[0]);
    vst1q_u32(addition_in_parallel_streamed+5,addition_in_parallel_result_recast[1]);
    vst1q_u32(addition_in_parallel_streamed+9,addition_in_parallel_result_recast[2]);
    vst1q_u32(addition_in_parallel_streamed+13,addition_in_parallel_result_recast[3]);

    // SECOND ADDITION: ADD PAIR-WISE

    uint32x4_t addition_words_shifted[4];
    addition_words_shifted[0] = vld1q_u32(addition_in_parallel_streamed);
    addition_words_shifted[1] = vld1q_u32(addition_in_parallel_streamed+4);
    addition_words_shifted[2] = vld1q_u32(addition_in_parallel_streamed+8);
    addition_words_shifted[3] = vld1q_u32(addition_in_parallel_streamed+12);

    uint32x4_t addition_in_pairs[2];
    addition_in_pairs[0]=vpaddq_u32(addition_words_shifted[0],addition_words_shifted[1]);
    addition_in_pairs[1]=vpaddq_u32(addition_words_shifted[2],addition_words_shifted[3]);

    uint32_t result_halvess[10]={0}; // Index 0 = 0 ; Index 1 = carry-bit; Index 2-3 = result-word-1; Index 4-5 = result-word-2; Index 6-7 = result-word-3; Index 8-9 = result-word-4
    vst1q_u32(result_halvess+1, addition_in_pairs[0]);
    vst1q_u32(result_halvess+5, addition_in_pairs[1]);
    result_halvess[9]=addition_in_parallel_streamed[16];    // Last position is the 32 least significant bits of the original 4x64-bits parallel addition.

    /*uint32x2_t result_concat[5];
    result_concat[0]=vld1_u32(result_halvess); // For uint32x2_t, use vld1_u32, for uint32x4_t, use uint32x4_t vld1q_u32(uint32_t const *ptr)
    result_concat[1]=vld1_u32(result_halvess+2);
    result_concat[2]=vld1_u32(result_halvess+4);
    result_concat[3]=vld1_u32(result_halvess+6);
    result_concat[4]=vld1_u32(result_halvess+8);

    uint64_t result_recast[5];
    result_recast[0]=vreinterpret_u64_u32(result_concat[0]);
    result_recast[1]=vreinterpret_u64_u32(result_concat[1]);
    result_recast[2]=vreinterpret_u64_u32(result_concat[2]);
    result_recast[3]=vreinterpret_u64_u32(result_concat[3]);
    result_recast[4]=vreinterpret_u64_u32(result_concat[4]);

    uint64_t results_combined[5];
    results_combined[0]=(uint64_t) result_halvess[0]<<32 | result_halvess[1];
    results_combined[1]=(uint64_t) result_halvess[2]<<32 | result_halvess[3];
    results_combined[2]=(uint64_t) result_halvess[4]<<32 | result_halvess[5];
    results_combined[3]=(uint64_t) result_halvess[6]<<32 | result_halvess[7];
    results_combined[4]=(uint64_t) result_halvess[8]<<32 | result_halvess[9];*/

//    *(output+0)=(uint64_t) result_halvess[0]<<32 | result_halvess[1];
//    *(output+1)=(uint64_t) result_halvess[2]<<32 | result_halvess[3];
//    *(output+2)=(uint64_t) result_halvess[4]<<32 | result_halvess[5];
//    *(output+3)=(uint64_t) result_halvess[6]<<32 | result_halvess[7];
//    *(output+4)=(uint64_t) result_halvess[8]<<32 | result_halvess[9];

    uint32x2_t result_concat[5];
    result_concat[0]=vld1_u32(result_halvess); // For uint32x2_t, use vld1_u32, for uint32x4_t, use uint32x4_t vld1q_u32(uint32_t const *ptr)
    result_concat[1]=vld1_u32(result_halvess+2);
    result_concat[2]=vld1_u32(result_halvess+4);
    result_concat[3]=vld1_u32(result_halvess+6);
    result_concat[4]=vld1_u32(result_halvess+8);

    uint64x1_t carryOnWord = vreinterpret_u64_u32(result_concat[0]);
    vst1_u64(carryOn,carryOnWord);

//    uint64x1_t outputWords[4];
//    outputWords[0]=vreinterpret_u64_u32(result_concat[0]);
//    outputWords[1]=vreinterpret_u64_u32(result_concat[1]);
//    outputWords[2]=vreinterpret_u64_u32(result_concat[2]);
//    outputWords[3]=vreinterpret_u64_u32(result_concat[3]);



    //vst4_u64(output, result_recast);
    vst1_u64(&output[0], vreinterpret_u64_u32(result_concat[0]));
    vst1_u64(&output[1], vreinterpret_u64_u32(result_concat[1]));
    vst1_u64(&output[2], vreinterpret_u64_u32(result_concat[2]));
    vst1_u64(&output[3], vreinterpret_u64_u32(result_concat[3]));


//    uint64x1x4_t result_recast;
//    //result_recast[0]=vreinterpret_u64_u32(result_concat[0]); // Skip first word, carry on
//    result_recast.val[0]=vreinterpret_u64_u32(result_concat[1]);
//    result_recast.val[1]=vreinterpret_u64_u32(result_concat[2]);
//    result_recast.val[2]=vreinterpret_u64_u32(result_concat[3]);
//    result_recast.val[3]=vreinterpret_u64_u32(result_concat[4]);
//
//    //vst4_u64(output, result_recast);
//    vst1_u64(output, result_recast.val[0]);
//    vst1_u64(output+1, result_recast.val[1]);
//    vst1_u64(output+2, result_recast.val[2]);
//    vst1_u64(output+3, result_recast.val[3]);






}

void load_high_low_04_words(uint64_t *output, const uint64_t *input){
    uint64x1_t zero = {0};
    uint64x1x4_t input01_loaded = vld4_u64(input); //Creates one uint64x1x4_t out of 4 words in the input01.

    //To separate the 32 HIGH and LOW bits of a 64-bits word, we will use:
    //  1. SLI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsli_n_u64
    //  2. SRI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsri_n_u64

    uint64x1_t input01_high_low[4][2];  //Each word is of the form XX..XYY..Y, where XX...X are the 32 HIGH bits, and YY...Y are the 32 LOW bits.
    input01_high_low[0][0] = vsli_n_u64(input01_loaded.val[0], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[0][1] = vsri_n_u64(zero, input01_loaded.val[0],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    vst1q_u64(&output[0], vcombine_u64(input01_high_low[0][1],
                                       input01_high_low[0][0]));  // Function is of the form vcombine_u64(low, high), returning [high:low]




    input01_high_low[1][0] = vsli_n_u64(input01_loaded.val[1], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[1][1] = vsri_n_u64(zero, input01_loaded.val[1],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    vst1q_u64(&output[2], vcombine_u64(input01_high_low[1][1],
                                       input01_high_low[1][0]));  // Function is of the form vcombine_u64(low, high), returning [high:low]





    input01_high_low[2][0] = vsli_n_u64(input01_loaded.val[2], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[2][1] = vsri_n_u64(zero, input01_loaded.val[2],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    vst1q_u64(&output[4],  vcombine_u64(input01_high_low[2][1],
                                        input01_high_low[2][0]));  // Function is of the form vcombine_u64(low, high), returning [high:low]






    input01_high_low[3][0] = vsli_n_u64(input01_loaded.val[3], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[3][1] = vsri_n_u64(zero, input01_loaded.val[3],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    vst1q_u64(&output[6], vcombine_u64(input01_high_low[3][1],
                                       input01_high_low[3][0]));  // Function is of the form vcombine_u64(low, high), returning [high:low]
}


void addition_256bits_carrying_02(uint64_t* output, uint64_t* carryOn,const uint64_t *input01, const uint64_t *input02) {

    uint64x1_t zero = {0};
    uint64x1_t input01_high_low[4][2];  //Each word is of the form XX..XYY..Y, where XX...X are the 32 HIGH bits, and YY...Y are the 32 LOW bits.
    uint64x2_t input01_words_padded[4]; //We combine two 64x1_t words (low - high) into a single 64x2_t word (high - low) - https://developer.arm.com/architectures/instruction-sets/intrinsics/vcombine_u64

    // INPUT 01

    uint64_t input01_hig_low_words[4];
    load_high_low_04_words(input01_hig_low_words, input01);








    uint64x1x4_t input01_loaded = vld4_u64(input01); //Creates one uint64x1x4_t out of 4 words in the input01.

    //To separate the 32 HIGH and LOW bits of a 64-bits word, we will use:
    //  1. SLI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsli_n_u64
    //  2. SRI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsri_n_u64


    input01_high_low[0][0] = vsli_n_u64(input01_loaded.val[0], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[0][1] = vsri_n_u64(zero, input01_loaded.val[0],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_words_padded[0] = vcombine_u64(input01_high_low[0][1],
                                           input01_high_low[0][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]




    input01_high_low[1][0] = vsli_n_u64(input01_loaded.val[1], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[1][1] = vsri_n_u64(zero, input01_loaded.val[1],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_words_padded[1] = vcombine_u64(input01_high_low[1][1],
                                           input01_high_low[1][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]





    input01_high_low[2][0] = vsli_n_u64(input01_loaded.val[2], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[2][1] = vsri_n_u64(zero, input01_loaded.val[2],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_words_padded[2] = vcombine_u64(input01_high_low[2][1],
                                           input01_high_low[2][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]






    input01_high_low[3][0] = vsli_n_u64(input01_loaded.val[3], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[3][1] = vsri_n_u64(zero, input01_loaded.val[3],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_words_padded[3] = vcombine_u64(input01_high_low[3][1],
                                           input01_high_low[3][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]

    // INPUT 02

    uint64x1x4_t input02_loaded = vld4_u64(input02); //Creates one uint64x1x4_t out of 4 words in the input01.

    //To separate the 32 HIGH and LOW bits of a 64-bits word, we will use:
    //  1. SLI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsli_n_u64
    //  2. SRI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsri_n_u64

    uint64x1_t input02_high_low[4][2];  //Each word is of the form XX..XYY..Y, where XX...X are the 32 HIGH bits, and YY...Y are the 32 LOW bits.
    input02_high_low[0][0] = vsli_n_u64(input02_loaded.val[0], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input02_high_low[0][1] = vsri_n_u64(zero, input02_loaded.val[0],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input02_high_low[1][0] = vsli_n_u64(input02_loaded.val[1], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input02_high_low[1][1] = vsri_n_u64(zero, input02_loaded.val[1],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input02_high_low[2][0] = vsli_n_u64(input02_loaded.val[2], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input02_high_low[2][1] = vsri_n_u64(zero, input02_loaded.val[2],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input02_high_low[3][0] = vsli_n_u64(input02_loaded.val[3], zero,
                                        32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input02_high_low[3][1] = vsri_n_u64(zero, input02_loaded.val[3],
                                        32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    uint64x2_t input02_words_padded[4]; //We combine two 64x1_t words (low - high) into a single 64x2_t word (high - low) - https://developer.arm.com/architectures/instruction-sets/intrinsics/vcombine_u64
    input02_words_padded[0] = vcombine_u64(input02_high_low[0][1],
                                           input02_high_low[0][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input02_words_padded[1] = vcombine_u64(input02_high_low[1][1],
                                           input02_high_low[1][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input02_words_padded[2] = vcombine_u64(input02_high_low[2][1],
                                           input02_high_low[2][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input02_words_padded[3] = vcombine_u64(input02_high_low[3][1],
                                           input02_high_low[3][0]);  // Function is of the form vcombine_u64(low, high), returning [high:low]

    // FIRST ADDITION: ADD IN PARALLEL

    // We need 4 additions (each word_padded is 64x2 - input01 + input02 - and we have four words)
    // We use:
    //      uint64x2_t vaddq_u64 (uint64x2_t a, uint64x2_t b)
    // https://developer.arm.com/architectures/instruction-sets/intrinsics/vaddq_u64

    uint64x2_t addition_in_parallel[4];

    addition_in_parallel[0] = vaddq_u64(input01_words_padded[0], input02_words_padded[0]);
    addition_in_parallel[1] = vaddq_u64(input01_words_padded[1], input02_words_padded[1]);
    addition_in_parallel[2] = vaddq_u64(input01_words_padded[2], input02_words_padded[2]);
    addition_in_parallel[3] = vaddq_u64(input01_words_padded[3], input02_words_padded[3]);

    uint32x4_t addition_in_parallel_result_recast_reversed[4];

    addition_in_parallel_result_recast_reversed[0]=vreinterpretq_u32_u64(addition_in_parallel[0]);
    addition_in_parallel_result_recast_reversed[1]=vreinterpretq_u32_u64(addition_in_parallel[1]);
    addition_in_parallel_result_recast_reversed[2]=vreinterpretq_u32_u64(addition_in_parallel[2]);
    addition_in_parallel_result_recast_reversed[3]=vreinterpretq_u32_u64(addition_in_parallel[3]);

    uint32x4_t addition_in_parallel_result_recast[4];
    addition_in_parallel_result_recast[0]=vrev64q_u32(addition_in_parallel_result_recast_reversed[0]);
    addition_in_parallel_result_recast[1]=vrev64q_u32(addition_in_parallel_result_recast_reversed[1]);
    addition_in_parallel_result_recast[2]=vrev64q_u32(addition_in_parallel_result_recast_reversed[2]);
    addition_in_parallel_result_recast[3]=vrev64q_u32(addition_in_parallel_result_recast_reversed[3]);

    uint32_t addition_in_parallel_streamed[17]={0}; //17 = 2*8+1 = 2*(4 words of 64 bits split in 2 words of 32 bits)+1 word of 32 bits leading zero
    vst1q_u32(addition_in_parallel_streamed+1,addition_in_parallel_result_recast[0]);
    vst1q_u32(addition_in_parallel_streamed+5,addition_in_parallel_result_recast[1]);
    vst1q_u32(addition_in_parallel_streamed+9,addition_in_parallel_result_recast[2]);
    vst1q_u32(addition_in_parallel_streamed+13,addition_in_parallel_result_recast[3]);

    // SECOND ADDITION: ADD PAIR-WISE

    uint32x4_t addition_words_shifted[4];
    addition_words_shifted[0] = vld1q_u32(addition_in_parallel_streamed);
    addition_words_shifted[1] = vld1q_u32(addition_in_parallel_streamed+4);
    addition_words_shifted[2] = vld1q_u32(addition_in_parallel_streamed+8);
    addition_words_shifted[3] = vld1q_u32(addition_in_parallel_streamed+12);

    uint32x4_t addition_in_pairs[2];
    addition_in_pairs[0]=vpaddq_u32(addition_words_shifted[0],addition_words_shifted[1]);
    addition_in_pairs[1]=vpaddq_u32(addition_words_shifted[2],addition_words_shifted[3]);

    uint32_t result_halvess[10]={0}; // Index 0 = 0 ; Index 1 = carry-bit; Index 2-3 = result-word-1; Index 4-5 = result-word-2; Index 6-7 = result-word-3; Index 8-9 = result-word-4
    vst1q_u32(result_halvess+1, addition_in_pairs[0]);
    vst1q_u32(result_halvess+5, addition_in_pairs[1]);
    result_halvess[9]=addition_in_parallel_streamed[16];    // Last position is the 32 least significant bits of the original 4x64-bits parallel addition.

    /*uint32x2_t result_concat[5];
    result_concat[0]=vld1_u32(result_halvess); // For uint32x2_t, use vld1_u32, for uint32x4_t, use uint32x4_t vld1q_u32(uint32_t const *ptr)
    result_concat[1]=vld1_u32(result_halvess+2);
    result_concat[2]=vld1_u32(result_halvess+4);
    result_concat[3]=vld1_u32(result_halvess+6);
    result_concat[4]=vld1_u32(result_halvess+8);

    uint64_t result_recast[5];
    result_recast[0]=vreinterpret_u64_u32(result_concat[0]);
    result_recast[1]=vreinterpret_u64_u32(result_concat[1]);
    result_recast[2]=vreinterpret_u64_u32(result_concat[2]);
    result_recast[3]=vreinterpret_u64_u32(result_concat[3]);
    result_recast[4]=vreinterpret_u64_u32(result_concat[4]);

    uint64_t results_combined[5];
    results_combined[0]=(uint64_t) result_halvess[0]<<32 | result_halvess[1];
    results_combined[1]=(uint64_t) result_halvess[2]<<32 | result_halvess[3];
    results_combined[2]=(uint64_t) result_halvess[4]<<32 | result_halvess[5];
    results_combined[3]=(uint64_t) result_halvess[6]<<32 | result_halvess[7];
    results_combined[4]=(uint64_t) result_halvess[8]<<32 | result_halvess[9];*/

//    *(output+0)=(uint64_t) result_halvess[0]<<32 | result_halvess[1];
//    *(output+1)=(uint64_t) result_halvess[2]<<32 | result_halvess[3];
//    *(output+2)=(uint64_t) result_halvess[4]<<32 | result_halvess[5];
//    *(output+3)=(uint64_t) result_halvess[6]<<32 | result_halvess[7];
//    *(output+4)=(uint64_t) result_halvess[8]<<32 | result_halvess[9];

    uint32x2_t result_concat[5];
    result_concat[0]=vld1_u32(result_halvess); // For uint32x2_t, use vld1_u32, for uint32x4_t, use uint32x4_t vld1q_u32(uint32_t const *ptr)
    result_concat[1]=vld1_u32(result_halvess+2);
    result_concat[2]=vld1_u32(result_halvess+4);
    result_concat[3]=vld1_u32(result_halvess+6);
    result_concat[4]=vld1_u32(result_halvess+8);

    uint64x1_t carryOnWord = vreinterpret_u64_u32(result_concat[0]);
    vst1_u64(carryOn,carryOnWord);

//    uint64x1_t outputWords[4];
//    outputWords[0]=vreinterpret_u64_u32(result_concat[0]);
//    outputWords[1]=vreinterpret_u64_u32(result_concat[1]);
//    outputWords[2]=vreinterpret_u64_u32(result_concat[2]);
//    outputWords[3]=vreinterpret_u64_u32(result_concat[3]);



    //vst4_u64(output, result_recast);
    vst1_u64(&output[0], vreinterpret_u64_u32(result_concat[0]));
    vst1_u64(&output[1], vreinterpret_u64_u32(result_concat[1]));
    vst1_u64(&output[2], vreinterpret_u64_u32(result_concat[2]));
    vst1_u64(&output[3], vreinterpret_u64_u32(result_concat[3]));


//    uint64x1x4_t result_recast;
//    //result_recast[0]=vreinterpret_u64_u32(result_concat[0]); // Skip first word, carry on
//    result_recast.val[0]=vreinterpret_u64_u32(result_concat[1]);
//    result_recast.val[1]=vreinterpret_u64_u32(result_concat[2]);
//    result_recast.val[2]=vreinterpret_u64_u32(result_concat[3]);
//    result_recast.val[3]=vreinterpret_u64_u32(result_concat[4]);
//
//    //vst4_u64(output, result_recast);
//    vst1_u64(output, result_recast.val[0]);
//    vst1_u64(output+1, result_recast.val[1]);
//    vst1_u64(output+2, result_recast.val[2]);
//    vst1_u64(output+3, result_recast.val[3]);




}



void test_addition_05(uint64_t* output, const uint64_t *input01, const uint64_t *input02, const uint64_t *modulo){

    uint64x1_t zero = {0};

    uint64x1x4_t input01_loaded = vld4_u64(input01); //Creates one uint64x1x4_t out of 4 words in the input01.

    //To separate the 32 HIGH and LOW bits of a 64-bits word, we will use:
    //  1. SLI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsli_n_u64
    //  2. SRI: https://developer.arm.com/architectures/instruction-sets/intrinsics/vsri_n_u64

    uint64x1_t input01_high_low[4][2];  //Each word is of the form XX..XYY..Y, where XX...X are the 32 HIGH bits, and YY...Y are the 32 LOW bits.
    input01_high_low[0][0] = vsli_n_u64(input01_loaded.val[0],zero,32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[0][1] = vsri_n_u64(zero,input01_loaded.val[0], 32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_high_low[1][0] = vsli_n_u64(input01_loaded.val[1],zero,32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[1][1] = vsri_n_u64(zero,input01_loaded.val[1], 32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_high_low[2][0] = vsli_n_u64(input01_loaded.val[2],zero,32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[2][1] = vsri_n_u64(zero,input01_loaded.val[2], 32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y

    input01_high_low[3][0] = vsli_n_u64(input01_loaded.val[3],zero,32);     //  Save HIGH bits of first word with 32 leading 0's padded, i.e., 00..0XX..X
    input01_high_low[3][1] = vsri_n_u64(zero,input01_loaded.val[3], 32);    //  Save LOW bits of first word with 32 leading 0's padded, i.e., 00..0YY...Y






    uint64x2_t input01_01_high_low = vcombine_u64(input01_high_low[0][0], input01_high_low[0][1]);


    input01_high_low[1][0] = vsli_n_u64(input01_loaded.val[1],zero,32);
    input01_high_low[1][1] = vsri_n_u64(zero,input01_loaded.val[1], 32);
    uint64x2_t input01_02_high_low = vcombine_u64(input01_high_low[1][0], input01_high_low[1][1]);



    uint64x1x4_t input02_loaded; // Number of words, i.e., 4 words



    vst1q_u64(output, input01_01_high_low);
    vst1q_u64(output+128, input01_02_high_low);
}



void test_addition_04(uint64_t* output, const uint64_t *input01, const uint64_t *input02, const uint64_t *modulo){

    uint64x1_t zero = {0};
    uint64x1x4_t input01_loaded; // Number of words, i.e., 4 words

    input01_loaded = vld4_u64(input01); //Creates one uint64x1x4_t out of 4 words in the input01.

    uint64x1_t input01_01_high = vsli_n_u64(input01_loaded.val[0],zero,32);
    uint64x1_t input01_01_low = vsri_n_u64(zero,input01_loaded.val[0], 32);

    uint64x1_t input01_02_high = vsli_n_u64(input01_loaded.val[1],zero,32);
    uint64x1_t input01_02_low = vsri_n_u64(zero,input01_loaded.val[1], 32);

    uint64x2_t input01_high_low = vcombine_u64(input01_01_high, input01_01_low);

    vst1q_u64(output, input01_high_low);
}

void test_addition_02(uint64_t* output, const uint64_t *input01, const uint64_t *input02, const uint64_t *modulo){

    uint64x1_t zero = {0};
    uint64x1_t input01_01_loaded = vld1_u64(input01);
    uint64x1_t input01_02_loaded = vld1_u64(input01+64);
    uint64x1_t input01_03_loaded = vld1_u64(input01+128);
    uint64x1_t input01_04_loaded = vld1_u64(input01+192);

    uint64x1_t input01_01_high = vsli_n_u64(input01_01_loaded,zero,32);
    uint64x1_t input01_01_low = vsri_n_u64(zero,input01_01_loaded, 32);
    uint64x1_t input01_02_high = vsli_n_u64(input01_02_loaded,zero,32);
    uint64x1_t input01_02_low = vsri_n_u64(zero,input01_02_loaded, 32);

    printbits_64x1_t(input01_01_high);
    printbits_64x1_t(input01_01_low);
    printbits_64x1_t(input01_02_high);
    printbits_64x1_t(input01_02_low);






    // working version
    /*
    uint64x1_t zero = {0};
    uint64x1x4_t input01_loaded; // Number of words, i.e., 4 words

    input01_loaded = vld4_u64(input01); //Creates one uint64x1x4_t out of 4 words in the input01.

    uint64x1_t input01_01_high = vsli_n_u64(input01_loaded.val[0],zero,32);
    uint64x1_t input01_01_low = vsri_n_u64(zero,input01_loaded.val[0], 32);

    uint64x1_t input01_02_high = vsli_n_u64(input01_loaded.val[1],zero,32);
    uint64x1_t input01_02_low = vsri_n_u64(zero,input01_loaded.val[1], 32);

    printbits_64x1_t(input01_01_high);
    printbits_64x1_t(input01_01_low);
    printbits_64x1_t(input01_02_high);
    printbits_64x1_t(input01_02_low);
     */

    // working version
    /*uint64x1_t input01_high_low[4][2]; //Split the words in pieces

    input01_high_low[0][0] = vsli_n_u64(input01_loaded.val[0],zero,32);    // zero[0:31] && input01_loaded.val[0], i.e., lower part of input01_loaded.val[0]
    input01_high_low[0][1] = vsri_n_u64(zero,input01_loaded.val[0], 32);

    input01_high_low[1][0] = vsli_n_u64(input01_loaded.val[1],zero,32);    // zero[0:31] && input01_loaded.val[0], i.e., lower part of input01_loaded.val[0]
    input01_high_low[1][1] = vsri_n_u64(zero,input01_loaded.val[1], 32);

    input01_high_low[2][0] = vsli_n_u64(input01_loaded.val[2],zero,32);    // zero[0:31] && input01_loaded.val[0], i.e., lower part of input01_loaded.val[0]
    input01_high_low[2][1] = vsri_n_u64(zero,input01_loaded.val[2], 32);

    input01_high_low[3][0] = vsli_n_u64(input01_loaded.val[3],zero,32);    // zero[0:31] && input01_loaded.val[0], i.e., lower part of input01_loaded.val[0]
    input01_high_low[3][1] = vsri_n_u64(zero,input01_loaded.val[3], 32);

    //printf("\n -------  test_addition_02 -------\n");
    printbits_64x1_t(input01_high_low[0][0]);
    //printf("\n ------- \n");
    */

//    uint64x2_t zero = {0};
//    uint64x2_t input01_loaded[2];
//
//    input01_loaded[0] = vld1q_u64(input01);     //  Words 1-2
//    input01_loaded[1] = vld1q_u64(input01+128); //  Words 3-4
//
//    uint64x2_t test;
//
//    test = vextq_u64(input01_loaded[0], zero,1);
//
//    printf("\n -------  test_addition_02 -------\n");
//    //printbits_64(test);
//    printf("\n ------- \n");


    /*uint32x2_t input_01_load_a[4];
    input_01_load_a[0] = vcreata_u32(input01+0);
    input_01_load_a[0] = vcreate_u32(input01+64);




    uint64x1x4_t input_01_loaded;
    input_01_loaded= vld4_u64(input01);

    uint64x2x4_t input_01_by_words;
    input_01_by_words.val[0] = vextq_u64(zero, )*/

//    uint64x2_t input_01_loaded[4];
//    uint64x2_t input_02_loaded[4];
//
//    input_01_loaded[0]= vld1q_u64(input01);
//    input_01_loaded[1]= vld1q_u64(input01);
//
//    input_01_loaded[0]= vld1q_u64(output);
//
//    uint64x2_t input_01b_loaded[8]; //Split every word of 64 bits into 2 words of 32 bits, but padding 32 leading 0 in each variable, e.g. 8 words in total
//    uint64x2_t input_02b_loaded[8];

}

void test_addition_constant_time_carry_on(){
    //  We will use vaddl_u32 as it is of the form
    //  uint64x2_t vaddl_u32(uint32x2_t a, uint32x2_t b); // VADDL.U32 q0,d0,d0
    //  https://developer.arm.com/documentation/dui0472/m/Using-NEON-Support/NEON-intrinsics-for-additionr

    //  Important notes: https://stackoverflow.com/questions/28868367/getting-the-high-part-of-64-bit-integer-multiplication

    uint64_t input_01[4];
    uint64_t input_02[4];
    uint64_t output[5]; //output[0],output[1],output[2],output[3] store the output, and output[4] stores the carry on bit

    input_01[0]=(uint64_t)0b1111000011110000111100001111000010101010101010101010101010101010;
    input_01[1]=(uint64_t)0b1111000011110000111100001111000010101010101010101010101010101010;
    input_01[2]=(uint64_t)0b1111000011110000111100001111000010101010101010101010101010101010;
    input_01[3]=(uint64_t)0b1111000011110000111100001111000010101010101010101010101010101010;

    input_02[0]=(uint64_t)0b1111000011110000111100001111000010101010101010101010101010101010;
    input_02[1]=(uint64_t)0b1111000011110000111100001111000010101010101010101010101010101010;
    input_02[2]=(uint64_t)0b1111000011110000111100001111000010101010101010101010101010101010;
    input_02[3]=(uint64_t)0b1111000011110000111100001111000010101010101010101010101010101010;

    output[0] = 0;
    output[1] = 0;
    output[2] = 0;
    output[3] = 0;
    output[4] = 0;

    uint32_t input_01_loaded[4][2];
    vst1_u32(input_01_loaded[0], (uint32x2_t)input_01[0]);
    vst1_u32(input_01_loaded[1], (uint32x2_t)input_01[1]);
    vst1_u32(input_01_loaded[2], (uint32x2_t)input_01[2]);
    vst1_u32(input_01_loaded[3], (uint32x2_t)input_01[3]);

    printbits_64(input_01[0]);
    printf("\n ------- \n");
//    printbits_32(input_01_high);
//    printbits_32(input_01_low);
    printf("\n ------- \n");

    printbits_32(input_01_loaded[0][1]);
    printbits_32(input_01_loaded[0][0]);

    printbits_32(input_01_loaded[1][1]);
    printbits_32(input_01_loaded[1][0]);

    printbits_32(input_01_loaded[2][1]);
    printbits_32(input_01_loaded[2][0]);

    printbits_32(input_01_loaded[3][1]);
    printbits_32(input_01_loaded[3][0]);


    printf("\n ------- \n");








    //uint32x2_t input_01_loaded = vld1_u32((uint32_t const*)(input_01[0]+0));
    //uint32x2_t input_02_loaded = vld1_u32((uint32_t const*)(input_02[0]+32));

    // Working code: load of variables by pieces (2 pieces)
    /*uint32_t input_01_high = input_01[0]>>32;
    uint32_t input_01_low = (uint32_t) (input_01[0]);*/

    /*uint32_t input_01_loaded[4][2];
    vst1_u32(input_01_loaded[0], (uint32x2_t)input_01[0]);

    //https://developer.arm.com/architectures/instruction-sets/intrinsics/vst1_u32

    printbits_64(input_01[0]);
    printf("\n ------- \n");
//    printbits_32(input_01_high);
//    printbits_32(input_01_low);
    printf("\n ------- \n");
    printbits_32(input_01_loaded[0][1]);
    printbits_32(input_01_loaded[0][0]);*/

    //uint32_t input_01_loaded[8];
    //vst4_u32(input_01_loaded, (uint32x2x4_t) {input_01[0], input_01[1], input_01[2], input_01[3]});

    //https://developer.arm.com/architectures/instruction-sets/intrinsics/vst1_u32

//    printbits_64(input_01[0]);
//    printf("\n ------- \n");
////    printbits_32(input_01_high);
////    printbits_32(input_01_low);
//    printf("\n ------- \n");
//    printbits_32(input_01_loaded[0]);
//    printbits_32(input_01_loaded[1]);
//
//    printbits_32(input_01_loaded[2]);
//    printbits_32(input_01_loaded[3]);
//
//    printbits_32(input_01_loaded[4]);
//    printbits_32(input_01_loaded[5]);
//
//    printbits_32(input_01_loaded[6]);
//    printbits_32(input_01_loaded[7]);
//
//    printf("\n ------- \n");


}


int main(int argc) {

    uint64_t arrayToTest[4];
//    arrayToTest[0] = (uint64_t) 0b1111000011110000111100001111000010101010101010101010101010101010;
//    arrayToTest[1] = (uint64_t) 0b1111000011110000111100001111000010101010101010101010101010101011;
//    arrayToTest[2] = (uint64_t) 0b1111000011110000111100001111000010101010101010101010101010101010;
//    arrayToTest[3] = (uint64_t) 0b1111000011110000111100001111000010101010101010101010101010101010;

    arrayToTest[0] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    arrayToTest[1] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    arrayToTest[2] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    arrayToTest[3] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;

    uint64_t expectedOutput[4]; // One extra word
    uint64_t carryOn = 0;
    uint64_t modulo[4];

    modulo[0] = 15;

//    test_addition_03(expectedOutput,
//                     &arrayToTest[0], &arrayToTest[1],&arrayToTest[2], &arrayToTest[3],
//                     &arrayToTest[0], &arrayToTest[1],&arrayToTest[2], &arrayToTest[3],
//                     modulo);
    addition_256bits_carrying(expectedOutput, &carryOn,  arrayToTest, arrayToTest);
    test_addition_06(expectedOutput,arrayToTest,arrayToTest,modulo);
    test_addition_02(expectedOutput,arrayToTest,arrayToTest,modulo);


    test_addition_constant_time_carry_on();


    uint64x2x2_t value_a;
    uint64x2x2_t value_b;

    value_a.val[0][0] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    value_a.val[0][1] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    value_a.val[1][0] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    value_a.val[1][1] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;

    value_b.val[0][0] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    value_b.val[0][1] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    value_b.val[1][0] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    value_b.val[1][1] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;

    uint64x2x2_t addition;
    addition.val[0] = vaddq_u64(value_a.val[0], value_b.val[0]);
    addition.val[1] = vaddq_u64(value_a.val[1], value_b.val[1]);

    printf("\n===========\n");

    printbits_64(value_a.val[1][1]);
    printbits_64(value_a.val[1][0]);
    printbits_64(value_a.val[0][1]);
    printbits_64(value_a.val[0][0]);

    printf("\n");

    printbits_64(value_b.val[1][1]);
    printbits_64(value_b.val[1][0]);
    printbits_64(value_b.val[0][1]);
    printbits_64(value_b.val[0][0]);

    printf("\n");

    printbits_64(addition.val[1][1]);
    printbits_64(addition.val[1][0]);
    printbits_64(addition.val[0][1]);
    printbits_64(addition.val[0][0]);

    printf("\n");

    //printf("[%llu][%ull][%ull][%ull]\n",  value_a.val[1][1],  value_a.val[1][0],  value_a.val[0][1],  value_a.val[0][0]);
    //printf("[%llu][%ull][%ull][%ull]\n",  value_b.val[1][1],  value_b.val[1][0],  value_b.val[0][1],  value_b.val[0][0]);

    //printf("[%llu][%ull][%ull][%ull]\n",  addition.val[1][1],  addition.val[1][0],  addition.val[0][1],  addition.val[0][0]);
    return 0;
}
