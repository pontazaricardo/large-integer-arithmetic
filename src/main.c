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
    arrayToTest[0] = (uint64_t) 0b1111000011110000111100001111000010101010101010101010101010101010;
    arrayToTest[1] = (uint64_t) 0b1111000011110000111100001111000010101010101010101010101010101011;
    arrayToTest[2] = (uint64_t) 0b1111000011110000111100001111000010101010101010101010101010101010;
    arrayToTest[3] = (uint64_t) 0b1111000011110000111100001111000010101010101010101010101010101010;

    uint64_t expectedOutput[4];
    uint64_t modulo[4];

    modulo[0] = 15;

//    test_addition_03(expectedOutput,
//                     &arrayToTest[0], &arrayToTest[1],&arrayToTest[2], &arrayToTest[3],
//                     &arrayToTest[0], &arrayToTest[1],&arrayToTest[2], &arrayToTest[3],
//                     modulo);
    test_addition_04(expectedOutput,arrayToTest,arrayToTest,modulo);
    test_addition_02(expectedOutput,arrayToTest,arrayToTest,modulo);


    test_addition_constant_time_carry_on();


    uint64x2x2_t value_a;
    uint64x2x2_t value_b;

    value_a.val[0][0] = (uint64_t)0b1111111111111111111111111111111111111111111111111111111111111111;
    value_a.val[0][1] = (uint64_t)0b000000001;
    value_a.val[1][0] = (uint64_t)1000000000ull;
    value_a.val[1][1] = (uint64_t)1000000000ull;

    value_b.val[0][0] = (uint64_t)0b0000000001;
    value_b.val[0][1] = (uint64_t)1000000000ull;
    value_b.val[1][0] = (uint64_t)1000000000ull;
    value_b.val[1][1] = (uint64_t)1000000000ull;

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
