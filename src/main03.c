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

void __attribute__((noinline)) load_high_low_04_words(uint64_t *output, const uint64_t *input){
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


void __attribute__((noinline)) load_high_low_04_words_02(uint64_t output[8], const uint64_t input[4]){
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

void __attribute__((noinline)) addition_pointwise(uint32_t output[17], const uint64_t input01[8], const uint64_t input02[8]){
    uint64x2_t input01_words_padded[4]; //We combine two 64x1_t words (low - high) into a single 64x2_t word (high - low) - https://developer.arm.com/architectures/instruction-sets/intrinsics/vcombine_u64
    uint64x2_t input02_words_padded[4]; //We combine two 64x1_t words (low - high) into a single 64x2_t word (high - low) - https://developer.arm.com/architectures/instruction-sets/intrinsics/vcombine_u64

    input01_words_padded[0] = vld1q_u64(&input01[0]);  // Function is of the form vld1q_u64
    input01_words_padded[1] = vld1q_u64(&input01[2]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input01_words_padded[2] = vld1q_u64(&input01[4]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input01_words_padded[3] = vld1q_u64(&input01[6]);  // Function is of the form vcombine_u64(low, high), returning [high:low]

    input02_words_padded[0] = vld1q_u64(&input02[0]);  // Function is of the form vld1q_u64
    input02_words_padded[1] = vld1q_u64(&input02[2]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input02_words_padded[2] = vld1q_u64(&input02[4]);  // Function is of the form vcombine_u64(low, high), returning [high:low]
    input02_words_padded[3] = vld1q_u64(&input02[6]);  // Function is of the form vcombine_u64(low, high), returning [high:low]

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

    vst1q_u32(&output[1],addition_in_parallel_result_recast[0]);
    vst1q_u32(&output[5],addition_in_parallel_result_recast[1]);
    vst1q_u32(&output[9],addition_in_parallel_result_recast[2]);
    vst1q_u32(&output[13],addition_in_parallel_result_recast[3]);

}

void __attribute__((noinline)) addition_pairwise(uint32_t output[10], const uint32_t addition_in_parallel_streamed[17]) {
    uint32x4_t addition_words_shifted[4];
    addition_words_shifted[0] = vld1q_u32(addition_in_parallel_streamed);
    addition_words_shifted[1] = vld1q_u32(&addition_in_parallel_streamed[4]);
    addition_words_shifted[2] = vld1q_u32(&addition_in_parallel_streamed[8]);
    addition_words_shifted[3] = vld1q_u32(&addition_in_parallel_streamed[12]);

    uint32x4_t addition_in_pairs[2];
    addition_in_pairs[0]=vpaddq_u32(addition_words_shifted[0],addition_words_shifted[1]);
    addition_in_pairs[1]=vpaddq_u32(addition_words_shifted[2],addition_words_shifted[3]);

    vst1q_u32(&output[1], addition_in_pairs[0]);
    vst1q_u32(&output[5], addition_in_pairs[1]);
}

void __attribute__((noinline)) concatenate(uint64_t output[5], const uint32_t result_halvess[10]){
    uint32x2_t result_concat[5];

    result_concat[0] = vld1_u32(&result_halvess[0]); // For uint32x2_t, use vld1_u32, for uint32x4_t, use uint32x4_t vld1q_u32(uint32_t const *ptr)
    result_concat[1] = vld1_u32(&result_halvess[2]);
    result_concat[2] = vld1_u32(&result_halvess[4]);
    result_concat[3] = vld1_u32(&result_halvess[6]);
    result_concat[4] = vld1_u32(&result_halvess[8]);

    uint64x1_t carryOnNeon =  vreinterpret_u64_u32(result_concat[0]);
    vst1_u64(&output[0], carryOnNeon);

    uint64x1_t outputWords[4];
    outputWords[0]=vreinterpret_u64_u32(result_concat[1]);
    outputWords[1]=vreinterpret_u64_u32(result_concat[2]);
    outputWords[2]=vreinterpret_u64_u32(result_concat[3]);
    outputWords[3]=vreinterpret_u64_u32(result_concat[4]);

    vst1_u64(&output[1],  outputWords[0]);
    vst1_u64(&output[2],  outputWords[1]);
    vst1_u64(&output[3],  outputWords[2]);
    vst1_u64(&output[4],  outputWords[3]);
}


void __attribute__((noinline)) addition_256bits_carrying_02(uint64_t output[5],const uint64_t input01[4], const uint64_t input02[4]) {

    uint64_t input01_hig_low_words[8];
    load_high_low_04_words_02(input01_hig_low_words, input01);

    uint64_t input02_hig_low_words[8];
    load_high_low_04_words_02(input02_hig_low_words, input02);

    // FIRST ADDITION: ADD IN PARALLEL

    // We need 4 additions (each word_padded is 64x2 - input01 + input02 - and we have four words)
    // We use:
    //      uint64x2_t vaddq_u64 (uint64x2_t a, uint64x2_t b)
    // https://developer.arm.com/architectures/instruction-sets/intrinsics/vaddq_u64

    uint32_t addition_in_parallel_streamed[17];
    addition_in_parallel_streamed[0] = 0;
    addition_pointwise(addition_in_parallel_streamed, input01_hig_low_words, input02_hig_low_words);


    // SECOND ADDITION: ADD PAIR-WISE

    uint32_t result_halvess[10]; // Index 0 = 0 ; Index 1 = carry-bit; Index 2-3 = result-word-1; Index 4-5 = result-word-2; Index 6-7 = result-word-3; Index 8-9 = result-word-4
    result_halvess[0] = 0;
    addition_pairwise(result_halvess, addition_in_parallel_streamed);
    result_halvess[9] = addition_in_parallel_streamed[16];    // Last position is the 32 least significant bits of the original 4x64-bits parallel addition.

    concatenate(output,  result_halvess);

}


void test_addition(){
    uint64_t arrayToTest[4];

    arrayToTest[0] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    arrayToTest[1] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    arrayToTest[2] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    arrayToTest[3] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;

    uint64_t expectedOutput[5]; // One extra word

    addition_256bits_carrying_02(expectedOutput, arrayToTest, arrayToTest);

    printf("Carry on: ");
    printbits_64(expectedOutput[0]);

    printf("\n--------\n");
    printf("Word 1: ");
    printbits_64(expectedOutput[1]);
    printf("\n");
    printf("Word 2: ");
    printbits_64(expectedOutput[2]);
    printf("\n");
    printf("Word 3: ");
    printbits_64(expectedOutput[3]);
    printf("\n");
    printf("Word 4: ");
    printbits_64(expectedOutput[4]);
    printf("\n");
}

int main() {

    test_addition();

    return 0;
}
