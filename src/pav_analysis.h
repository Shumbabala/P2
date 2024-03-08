#ifndef PAV_ANALYSIS_H
#define PAV_ANALYSIS_H

float compute_power(const float *x, unsigned int N);
float compute_am(const float *x, unsigned int N);
float compute_zcr(const float *x, unsigned int N, float fm);
// float compute_hamming_power(const float *x, const float *hamming, unsigned int N);
float compute_hamming_power(const float *hamming, unsigned int N, const int num, ...); // <-- "..." stand for "multiple arguments"
#endif /* PAV_ANALYSIS_H	*/
