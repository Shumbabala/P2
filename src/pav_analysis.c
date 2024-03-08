#include <math.h>
#include <stdarg.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N)
{
    /*---INPUTS---

        (1) *x -> float array del qual extreiem els float values per poder calcular la potencia

        (2) N -> integer positiu que ens diu el nombre de valors que extreure i multiplicar del float array x

    ---OUTPUTS---

        (1) el valor de potència mitjana en dBs (com un float)

    ---DESCRIPCIO---

        (1) Multiplica tots els elements (almenys el N primers) del float array x amb sí mateixos (operació d'elevar al quadrat), fa el sumatori
        i ho passa a dBs. Normalment els valors que treu aquesta funció, almenys amb el PAV_P1.wav d'aquesta pràctica, haurien de rondar
        entre els -40 dB -- -90 dB (sobretot ser negatius)
    */
    // Calculate the squared sum of the first N values in the array
    float squared_sum = 0.0;
    for (unsigned int i = 0; i < N; ++i)
    {
        squared_sum += x[i] * x[i];
    }

    // Divide the squared sum by N
    float average = squared_sum / N;

    // Convert the average to dB using 10 * log10()
    float result_dB = 10.0 * log10(average);

    return result_dB;
}

float compute_hamming_power(const float *hamming, unsigned int N, const int num, ...)
{
    /*---INPUTS---

        (1) *hamming -> float array dels coeficients de la finestra de Hamming

        (2) N -> Integer positiu que representa el nombre de multiplicacions a fer

        (3) num -> nombre que ens indica la quantitat de arguments que s'han entrat

        (4) ... -> els arguments variables: pot agafar 2 formes diferents:

            (1) quan actuem en mono, representa només el vector x* de floats (les samples de les quals hem de calcular la potència)

            (2) quan actuem en stereo, representa el seguent conjunt d'arguments: [*left_samples, *right_samples, *left_channel_power, *right_channel_power]

    ---OUTPUTS---

        ***Quan estem en mode mono, retorna la potència de l'unic canal

        ***Quan estem en stereo, retorna la potència dels 2 canals però a través de punters (return -1 però no se li fa cas)

    ---DESCRIPCIO---

        Implementació de la funció expressa en el document d'exercisi 2 d'ampliació de la pràctica
    */
    if (num > 1) // <-- in case variable arguments are forwarded
    {
        va_list args;        // Declare a va_list variable
        va_start(args, num); // Initialize the va_list | 4 because the args are: left_samples*, right_samples*, left_samples_power, right_samples_power
        float results[2];    // array to store float results

        // Loop through the arguments using va_arg
        for (int i = 0; i < num / 2 /*usually will just be 2, can be hardcoded*/; ++i)
        {
            float *samples = va_arg(args, float *); // Extract the next float array from arguments
            // compute the power of this samples array
            //  Initialize the numerator and denominator sums
            float numerator_sum = 0.0;
            float denominator_sum = 0.0;

            // Perform the summation over the first N elements of the arrays
            for (unsigned int n = 0; n < N / 2; ++n)
            {
                float term = samples[n] * hamming[n];
                numerator_sum += term * term;
                denominator_sum += hamming[n] * hamming[n];
            }

            // Calculate the final result using 10 * log10()
            float result = 10.0 * log10(numerator_sum / denominator_sum);
            results[i] = result;
        }
        // input result into the float pointer passed to this function at 3rd or 4th index
        // float *power_pointer = extractAtIndex(args, (i == 0) ? 2 : 3);
        for (int i = 0; i < 2; i++)
        {
            float *samples = va_arg(args, float *);
            *samples = results[i];
        }

        va_end(args); // Cleanup: This is necessary to free resources associated with va_list

        // return NULL in the case of multiple channels (results have already been returned using pointers)
        return -1; // <-- implement NULL checkers in main code (p1.c)
    }
    else // in case only 1 variable argument is forwarded
    {
        va_list args;        // Declare a va_list variable
        va_start(args, num); // Initialize the va_list

        // Initialize the numerator and denominator sums
        float numerator_sum = 0.0;
        float denominator_sum = 0.0;

        float *samples = va_arg(args, float *); // Extract the next float array from arguments

        // Perform the summation over the first N elements of the arrays
        for (unsigned int n = 0; n < N; ++n)
        {
            float term = samples[n] * hamming[n];
            numerator_sum += term * term;
            denominator_sum += hamming[n] * hamming[n];
        }

        // Calculate the final result using 10 * log10()
        float result = 10.0 * log10(numerator_sum / denominator_sum);

        return result;
    }
}

float compute_am(const float *x, unsigned int N)
{
    /*---INPUTS---

        (1) *x -> float array que conté els valors del senyal sobre el que operar

        (2) N -> Integer positiu que representa el nombre de multiplicacions a fer

    ---OUTPUTS---

        (1) resultat de la operacio (valor mitja del valor absolut del senyal x)

    ---DESCRIPCIO---

        Implementacio de la segona funcio (calcul del valor mitja del valor absolut del senyal) de les funcions del document de pràctiques
    */
    // Calculate the sum of absolute values of the first N elements in the array
    float absolute_sum = 0.0;
    for (unsigned int i = 0; i < N; ++i)
    {
        absolute_sum += fabs(x[i]);
    }

    // Divide the absolute sum by N
    float result = absolute_sum / N;

    return result;
}

float compute_zcr(const float *x, unsigned int N, float fm)
{
    /*--INPUTS---

        1) *x -> float array que conté els valors del senyal sobre el que operar

        (2) N -> Integer positiu que representa el nombre de multiplicacions a fer

        (3) fm -> valor float utilitzat en la operacio (sample rate)

    --OUTPUTS---

        (1) resultat de la operacio (nombre de 0 crossings trobats en x)

    ---DESCRIPTION---

        Impleemntacio de la tercera funcio del document de practiques
    */
    // Initialize the sum of sign changes
    int sign_change_sum = 0;

    // Calculate the sum of sign changes for n going from 0 to N-1
    for (unsigned int n = 1; n < N; ++n)
    {
        if ((x[n] >= 0 && x[n - 1] < 0) || (x[n] < 0 && x[n - 1] >= 0))
        {
            // If the sign of consecutive values is different, increment the sum
            sign_change_sum++;
        }
    }

    // Calculate the final result
    float result = (float)sign_change_sum * fm / (2 * (N - 1));

    return result; // <-- ha d'estar entre 0 i 8000 aprox, que sempre es compleix
}