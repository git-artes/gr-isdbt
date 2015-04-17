/* -*- c++ -*- */
/* 
 * Copyright 2014 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// #define DEBUG_OFDMSYM 1
// #define DEBUG_OFDMSYM_VERBOSE 1
// #define DEBUG_OFDMSYM_PERFORMANCE 1
#define USE_VOLK 1
#define USE_VOLK_ALIGN 1

//#define USE_POSIX_MEMALIGN 1

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <gnuradio/io_signature.h>
#include "ofdm_sym_acquisition_impl.h"
#include <complex>
#include <gnuradio/math.h>
#include <gnuradio/expj.h>
#include <volk/volk.h>
#include <gnuradio/fxpt.h>

#ifdef DEBUG_OFDMSYM
#define PRINTF(a...) printf(a)
#else
#define PRINTF(a...)
#endif

void print_float(float f)
{

    for (int j = 0; j < 4; j++)
        printf("%x", 0xff & ((char *)&f)[j]);
}

void print_double(float d)
{
    for (int j = 0; j < 8; j++)
        printf("%x", 0xff & ((char *)&d)[j]);
}


namespace gr {
    namespace isdbt  {

        int
            ofdm_sym_acquisition_impl::ml_sync(const gr_complex * in, int lookup_start, int lookup_stop, int * cp_pos, gr_complex * derot, int * to_consume, int * to_out )
            {

                assert(lookup_start >= lookup_stop);
                assert(lookup_stop >= (d_cp_length + d_fft_length - 1));

                int low, size;

                // Array to store peak positions
                int peak_pos[d_fft_length];
                float d_phi[d_fft_length];

                // Calculate norm
#ifdef USE_VOLK
                low = lookup_stop - (d_cp_length + d_fft_length - 1);
                size = lookup_start - (lookup_stop - (d_cp_length + d_fft_length - 1)) + 1;
                //printf("low: %i, size: %i\n", low, size);

                if(is_unaligned())
                    volk_32fc_magnitude_squared_32f_u(&d_norm[low], &in[low], size);
                else
#ifdef USE_VOLK_ALIGN
                    volk_32fc_magnitude_squared_32f_a(&d_norm[low], &in[low], size);
#else
                    volk_32fc_magnitude_squared_32f_u(&d_norm[low], &in[low], size);
#endif
                //TODO - fix the alignment
#else
                for (int i = lookup_start; i >= (lookup_stop - (d_cp_length + d_fft_length - 1)); i--)
                    d_norm[i] = std::norm(in[i]);
#endif

                // Calculate gamma on each point
#ifdef USE_VOLK
                low = lookup_stop - d_cp_length - 1;
                size = lookup_start - (lookup_stop - d_cp_length - 1) + 1;

                if (is_unaligned())
                    volk_32fc_x2_multiply_conjugate_32fc_u(&d_corr[low - d_fft_length], &in[low], &in[low - d_fft_length], size);
                else
#ifdef USE_VOLK_ALIGN
                    volk_32fc_x2_multiply_conjugate_32fc_a(&d_corr[low - d_fft_length], &in[low], &in[low - d_fft_length], size);
#else
                    volk_32fc_x2_multiply_conjugate_32fc_u(&d_corr[low - d_fft_length], &in[low], &in[low - d_fft_length], size);
#endif
                // TODO fix the alignment
#else
                for (int i = lookup_start; i >= (lookup_stop - d_cp_length - 1); i--)
                    d_corr[i - d_fft_length] = in[i] * std::conj(in[i - d_fft_length]);
#endif

                // Calculate time delay and frequency correction
                // This looks like spagetti code but it is fast
                for (int i = lookup_start - 1; i >= lookup_stop; i--)
                {
                    int k = i - lookup_stop;

                    d_phi[k] = 0.0;
                    d_gamma[k] = 0.0;

                    // Moving sum for calculating gamma and phi
                    for (int j = 0; j < d_cp_length; j++)
                    {
                        // Calculate gamma and store it
                        d_gamma[k] += d_corr[i - j - d_fft_length];
                        // Calculate phi and store it
                        d_phi[k] += d_norm[i - j] + d_norm[i - j - d_fft_length];
                    }
                }

                // Init lambda with gamma
#ifdef USE_VOLK
                low = 0;
                size = lookup_start - lookup_stop;

                if (is_unaligned())  
                    volk_32fc_magnitude_32f_u(&d_lambda[low], &d_gamma[low], size);
                else
#ifdef USE_VOLK_ALIGN
                    volk_32fc_magnitude_32f_a(&d_lambda[low], &d_gamma[low], size);
#else
                    volk_32fc_magnitude_32f_u(&d_lambda[low], &d_gamma[low], size);
#endif
                // TODO - fix alignment
#else
                for (int i = 0; i < (lookup_start - lookup_stop); i++)
                    d_lambda[i] = std::abs(d_gamma[i]);
#endif

                // Calculate lambda
#ifdef USE_VOLK
                low = 0;
                size = lookup_start - lookup_stop;

                if (is_unaligned())  
                {
                    volk_32f_s32f_multiply_32f_u(&d_phi[low], &d_phi[low], d_rho / 2.0, size);
                    volk_32f_x2_subtract_32f_u(&d_lambda[low], &d_lambda[low], &d_phi[low], size);
                }
                else
                {
#ifdef USE_VOLK_ALIGN
                    volk_32f_s32f_multiply_32f_a(&d_phi[low], &d_phi[low], d_rho / 2.0, size);
                    volk_32f_x2_subtract_32f_a(&d_lambda[low], &d_lambda[low], &d_phi[low], size);
#else
                    volk_32f_s32f_multiply_32f_u(&d_phi[low], &d_phi[low], d_rho / 2.0, size);
                    volk_32f_x2_subtract_32f_u(&d_lambda[low], &d_lambda[low], &d_phi[low], size);
#endif
                    //TODO - fix the alignment
                }
#else
                for (int i = 0; i < (lookup_start - lookup_stop); i++)
                    d_lambda[i] -= (d_phi[i] * d_rho / 2.0);
#endif

#if 0
                // Print input
                for (int i = 0; i < (2 * d_fft_length + d_cp_length); i++)
                    printf("in[%i].re: %.10f, in[%i].img: %.10f\n", i, in[i].real(), i, in[i].imag());
#endif

                int peak_length, peak, peak_max;
                unsigned int max_fede = 0; 
                // Find peaks of lambda
                // We have found an end of symbol at peak_pos[0] + CP + FFT

#ifdef USE_VOLK
                volk_32f_index_max_16u(&max_fede, &d_lambda[0], (lookup_start - lookup_stop)); 
#else
                for(int i=1; i<lookup_start-lookup_stop; i++){
                  if (d_lambda[i]>d_lambda[max_fede])
                     max_fede = i; 
                }
#endif

                peak_length = 1; 
                //si el máximo está en un extremo, quiere decir que no estoy en el medio del pico y hay que hacer una búsqueda exhaustiva.           
                if ( lookup_start-lookup_stop != d_fft_length )
                    if (max_fede==0 || max_fede==(unsigned int)(lookup_start-lookup_stop-1))
                     {
                       peak_length = 0;
                       // return( peak_length );
                     }
                    
                peak_max = 0; 
                peak_pos[peak_max] = (int)max_fede; 

                if (peak_length!=1)
                 {
                    PRINTF("WARNING: %d GG likelihood function peaks detected. OFDM signal at input?\n",peak_length); 
                 }
                if (peak_length==1)
                {
                    peak = peak_pos[peak_max] + lookup_stop;
#if 0
                    printf("peak: %i, peak_pos: %i, lambda[%i]: %f\n", peak, peak_pos[peak_max], peak_pos[peak_max], d_lambda[peak_pos[peak_max]]);
#endif
                    *cp_pos = peak;

                    // Calculate frequency correction
                    /*float peak_epsilon = gr_fast_atan2f(d_gamma[peak_pos[0]]);*/
                    float peak_epsilon = fast_atan2f(d_gamma[peak_pos[peak_max]]);
                    double sensitivity = (double)(-1) / (double)d_fft_length;

                    //printf("peak_epsilon: %.10f\n", peak_epsilon);
                    //printf("d_phaseinc before fft: %.10f\n", d_phaseinc);

                    // Store phases for derotating the signal
                    // We always process CP len + FFT len
                    for (int i = 0; i < (d_cp_length + d_fft_length); i++)
                    {
                        if (i == d_nextpos)
                            d_phaseinc = d_nextphaseinc;

                        // We are interested only in fft_length
                        d_phase += d_phaseinc;

                        while (d_phase > (float)M_PI)
                            d_phase -= (float)(2.0 * M_PI);
                        while (d_phase < (float)(-M_PI))
                            d_phase += (float)(2.0 * M_PI);

                        //printf("d_phase[%i]: %.10f, d_phaseinc: %.10f\n", d_index++, d_phase, d_phaseinc);

#ifdef USE_FIXEDARCTAN
                        float oq, oi;
                        gr_int32 angle = gr_fxpt::float_to_fixed (d_phase);
                        gr_fxpt::sincos(angle, &oq, &oi);

                        derot[i] = gr_complex(oi, oq);
#else
                        derot[i] = gr_expj(d_phase);
#endif
                    }

                    d_nextphaseinc = sensitivity * peak_epsilon;
                    d_nextpos = peak - (d_cp_length + d_fft_length);

                    //printf("d_phaseinc after fft: %.10f\n", d_phaseinc);
                    //printf("d_phase after fft: %f\n", d_phase);

                    *to_consume = d_cp_length + d_fft_length;
                    *to_out = 1;
                    return (peak_length);
                }
                else
                {
#if 0
                    printf("miss, phaseinc: %.10f, phase: %.10f, \n", d_phaseinc, d_phase);
#endif

#if 0
                    // Print input
                    for (int i = 0; i < (2 * d_fft_length + d_cp_length); i++)
                        printf("in[%i]: %.10f\n", i, in[i]);

                    // Print lambda
                    for (int i = 0; i < d_fft_length; i++)
                        printf("lambda[%i]: %.10f\n", i, d_lambda[i]);
#endif
                    for (int i = 0; i < (d_cp_length + d_fft_length); i++)
                    {
                        d_phase += d_phaseinc;

                        while (d_phase > (float)M_PI)
                            d_phase -= (float)(2.0 * M_PI);
                        while (d_phase < (float)(-M_PI))
                            d_phase += (float)(2.0 * M_PI);
                    }

                    // We consume only fft_length
                    *to_consume = d_cp_length + d_fft_length;
                    *to_out = 0;
                    return 0;
                }

            }

        int
            ofdm_sym_acquisition_impl::peak_detection( int * peak_pos, int lookup_start, int lookup_stop )
            {
                int peak_max;
                unsigned int peak_index = 0; 
                // Find peaks of lambda
                // We have found an end of symbol at peak_pos[0] + CP + FFT

#ifdef USE_VOLK
                volk_32f_index_max_16u(&peak_index, &d_lambda[0], (lookup_start - lookup_stop)); 
#else
                for(int i=1; i<lookup_start-lookup_stop; i++){
                    if (d_lambda[i]>d_lambda[peak_index])
                        peak_index = i; 
                }
#endif
                peak_max = 0; 
                peak_pos[peak_max] = (int)peak_index; 

                return( peak_index );
            }

        int
            ofdm_sym_acquisition_impl::rotation( unsigned int peak_index, int * peak_pos, int lookup_start, int lookup_stop, int * cp_pos, gr_complex * derot, int * to_consume, int * to_out)
            {
                int peak_length;
                int peak;
                //si el máximo está en un extremo, quiere decir que no estoy en el medio del pico y hay que hacer una búsqueda exhaustiva.           
                if (peak_index==0 || peak_index==(unsigned int)(lookup_start-lookup_stop-1))
                    peak_length = 0;
                else
                    peak_length = 1; 

                if (peak_length!=1)
                    printf("WARNING: %d likelihood function peaks detected. OFDM signal at input?\n",peak_length); 
                if (peak_length==1)
                {
                    // peak = peak_pos[peak_max] + lookup_stop;
                    peak = peak_pos[0] + lookup_stop;

                    *cp_pos = peak;

                    // Calculate frequency correction
                    /*float peak_epsilon = gr_fast_atan2f(d_gamma[peak_pos[0]]);*/
                    // float peak_epsilon = fast_atan2f(d_gamma[peak_pos[peak_max]]);
                    float peak_epsilon = fast_atan2f(d_gamma[peak_pos[0]]);
                    double sensitivity = (double)(-1) / (double)d_fft_length;

                    // Store phases for derotating the signal
                    // We always process CP len + FFT len
                    for (int i = 0; i < (d_cp_length + d_fft_length); i++)
                    {
                        if (i == d_nextpos)
                            d_phaseinc = d_nextphaseinc;

                        // We are interested only in fft_length
                        d_phase += d_phaseinc;

                        while (d_phase > (float)M_PI)
                            d_phase -= (float)(2.0 * M_PI);
                        while (d_phase < (float)(-M_PI))
                            d_phase += (float)(2.0 * M_PI);

                        derot[i] = gr_expj(d_phase);
                    }

                    d_nextphaseinc = sensitivity * peak_epsilon;
                    d_nextpos = peak - (d_cp_length + d_fft_length);

                    *to_consume = d_cp_length + d_fft_length;
                    *to_out = 1;
                    return (peak_length);
                }
                else
                {
                    for (int i = 0; i < (d_cp_length + d_fft_length); i++)
                    {
                        d_phase += d_phaseinc;

                        while (d_phase > (float)M_PI)
                            d_phase -= (float)(2.0 * M_PI);
                        while (d_phase < (float)(-M_PI))
                            d_phase += (float)(2.0 * M_PI);
                    }

                    // We consume only fft_length
                    *to_consume = d_cp_length + d_fft_length;
                    *to_out = 0;
                    return 0;
                }

            }

        void
            ofdm_sym_acquisition_impl::send_sync_start()
            {
                const uint64_t offset = this->nitems_written(0);
                pmt::pmt_t key = pmt::string_to_symbol("sync_start");
                pmt::pmt_t value = pmt::from_long(1);
                this->add_item_tag(0, offset, key, value);
                printf("OFDM symbol acqu: restarting acquisition\n"); 
            }

        ofdm_sym_acquisition::sptr
            ofdm_sym_acquisition::make(int fft_length, int cp_length, float snr)
            {
                return gnuradio::get_initial_sptr (new ofdm_sym_acquisition_impl(fft_length, cp_length, snr));
            }

        /*
         * The private constructor
         */
        ofdm_sym_acquisition_impl::ofdm_sym_acquisition_impl(int fft_length, int cp_length, float snr)
            : block("ofdm_sym_acquisition",
                    io_signature::make(1, 1, sizeof (gr_complex) ),
                    io_signature::make(1, 1, sizeof (gr_complex) * fft_length)),
            d_fft_length(fft_length), d_cp_length(cp_length), d_snr(snr),
            d_index(0), d_phase(0.0), d_phaseinc(0.0), d_cp_found(0), d_count(0), d_nextphaseinc(0), d_nextpos(0), \
              d_sym_acq_count(0),d_sym_acq_timeout(100), d_initial_aquisition(0), \
              d_freq_correction_count(0), d_freq_correction_timeout(0)
        {
            set_relative_rate(1.0 / (double) (d_cp_length + d_fft_length));

            d_snr = pow(10, d_snr / 10.0);
            d_rho = d_snr / (d_snr + 1.0);

#ifdef DEBUG_OFDMSYM_PERFORMANCE
            printf( "start!: %i\n", d_count );
            outfile.open("file.dat", std::ios::app );
#ifdef USE_VOLK
            outfile << "USE_VOLK: 1" << std::endl;
#else
            outfile << "USE_VOLK: 0" << std::endl;
#endif
#ifdef USE_VOLK_ALIGN
            outfile << "USE_VOLK_ALIGN: 1" << std::endl;
#else
            outfile << "USE_VOLK_ALIGN: 0" << std::endl;
#endif
            time_t now = time(0);
            // char* dt = ctime(&now);
            outfile << "start time: " << now << std::endl;
#endif

            PRINTF("OFDM sym acq: fft_length: %i\n", fft_length);
            PRINTF("OFDM sym acq: SNR: %f\n", d_snr);

            const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
            set_alignment(std::max(1, alignment_multiple));

            const int alignment = volk_get_alignment();

#ifdef USE_POSIX_MEMALIGN
            //if (posix_memalign((void **)&d_gamma, alignment, sizeof(gr_complex) * d_fft_length))
            if (posix_memalign((void **)&d_gamma, alignment, sizeof(gr_complex) * d_fft_length))
                std::cout << "cannot allocate memory: d_gamma" << std::endl;

            //if (posix_memalign((void **)&d_lambda, alignment, sizeof(float) * d_fft_length))
            if (posix_memalign((void **)&d_lambda, alignment, sizeof(float) * d_fft_length))
                std::cout << "cannot allocate memory: d_lambda" << std::endl;

            //if (posix_memalign((void **)&d_derot, alignment, sizeof(gr_complex) * (d_fft_length + d_cp_length)))
            if (posix_memalign((void **)&d_derot, alignment, sizeof(gr_complex) * (d_fft_length + d_cp_length)))
                std::cout << "cannot allocate memory: d_derot" << std::endl;

            //if (posix_memalign((void **)&d_conj, alignment, sizeof(gr_complex) * (2 * d_fft_length + d_cp_length)))
            if (posix_memalign((void **)&d_conj, alignment, sizeof(gr_complex) * (2 * d_fft_length + d_cp_length)))
                std::cout << "cannot allocate memory: d_conj" << std::endl;

            //if (posix_memalign((void **)&d_norm, alignment, sizeof(float) * (2 * d_fft_length + d_cp_length)))
            if (posix_memalign((void **)&d_norm, alignment, sizeof(float) * (2 * d_fft_length + d_cp_length)))
                std::cout << "cannot allocate memoryi: d_norm" << std::endl;

            //if (posix_memalign((void **)&d_corr, alignment, sizeof(gr_complex) * (2 * d_fft_length + d_cp_length)))
            if (posix_memalign((void **)&d_corr, alignment, sizeof(gr_complex) * (2 * d_fft_length + d_cp_length)))
                std::cout << "cannot allocate memoryi: d_corr" << std::endl;
#else
            d_gamma = new gr_complex[d_fft_length];
            if (d_gamma == NULL)
                std::cout << "cannot allocate memory: d_gamma" << std::endl;

            d_lambda = new float[d_fft_length];
            if (d_lambda == NULL)
                std::cout << "cannot allocate memory: d_lambda" << std::endl;

            d_derot = new gr_complex[d_cp_length + d_fft_length];
            if (d_derot == NULL)
                std::cout << "cannot allocate memory: d_derot" << std::endl;

            d_conj = new gr_complex[2 * d_fft_length + d_cp_length];
            if (d_conj == NULL)
                std::cout << "cannot allocate memory: d_conj" << std::endl;

            d_norm = new float[2 * d_fft_length + d_cp_length];
            if (d_norm == NULL)
                std::cout << "cannot allocate memory: d_norm" << std::endl;

            d_corr = new gr_complex[2 * d_fft_length + d_cp_length];
            if (d_corr == NULL)
                std::cout << "cannot allocate memory: d_corr" << std::endl;
#endif
        }

        /*
         * Our virtual destructor.
         */
        ofdm_sym_acquisition_impl::~ofdm_sym_acquisition_impl()
        {
#ifdef USE_POSIX_MEMALIGN
            free(d_gamma);
            free(d_lambda);
            free(d_derot);
            free(d_conj);
            free(d_norm);
            free(d_corr);
#else
            delete [] d_gamma;
            delete [] d_lambda;
            delete [] d_derot;
            delete [] d_conj;
            delete [] d_norm;
            delete [] d_corr;
#endif
        }

        void
            ofdm_sym_acquisition_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
            {
                int ninputs = ninput_items_required.size ();

                // make sure we receive at least (symbol_length + fft_length)
                for (int i = 0; i < ninputs; i++)
                    ninput_items_required[i] = (2 * d_fft_length + d_cp_length) * noutput_items;
            }

        /*
         * ML Estimation of Time and Frequency Offset in OFDM systems
         * Jan-Jaap van de Beck
         */

        int
            ofdm_sym_acquisition_impl::general_work (int noutput_items,
                    gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                const gr_complex *in = (const gr_complex *) input_items[0];
                gr_complex *out = (gr_complex *) output_items[0];

                int low, size;

                // This is initial aquisition of symbol start
                // TODO - make a FSM
                if (!d_initial_aquisition)
                {
                    d_initial_aquisition = ml_sync(in, 2 * d_fft_length + d_cp_length - 1, d_fft_length + d_cp_length - 1, \
                            &d_cp_start, &d_derot[0], &d_to_consume, &d_to_out );

                    // Send sync_start downstream
                    send_sync_start();

                    PRINTF("initial_acq: %i, d_cp_start: %i, d_to_consume,: %i, d_to_out: %i\n", d_initial_aquisition, d_cp_start, d_to_consume, d_to_out);
                }

                // This is fractional frequency correction (pre FFT)
                // It is also calle coarse frequency correction
                if (d_initial_aquisition)
                {
                    // d_cp_found = ml_sync(in, 2 * d_fft_length + d_cp_length - 1, d_fft_length + d_cp_length - 1, \
                            &d_cp_start, &d_derot[0], &d_to_consume, &d_to_out );
                    int prev_d_cp_start = d_cp_start;
                    d_cp_found = ml_sync(in, d_cp_start + 8, d_cp_start - 8, \
                            &d_cp_start, &d_derot[0], &d_to_consume, &d_to_out );
                    if ( !d_cp_found )
                     {
                        // int prev_d_to_out = d_to_out;
                        d_cp_found = ml_sync(in, 2 * d_fft_length + d_cp_length - 1, d_fft_length + d_cp_length - 1, \
                            &prev_d_cp_start, &d_derot[0], &d_to_consume, &d_to_out );
                        PRINTF("r" );
#if 0
                        if ( ( d_to_out == 0 ) && ( prev_d_to_out == 0 ) )
                         {
                            PRINTF("no peaks after retry!: initial_acq: %i, d_cp_start: %i, d_to_consume,: %i, d_to_out: %i\n", d_initial_aquisition, d_cp_start, d_to_consume, d_to_out);
                            // Print input
                            for (int i = 0; i < (2 * d_fft_length + d_cp_length); i++)
                                PRINTF("in[%i].re: %.10f, in[%i].img: %.10f\n", i, in[i].real(), i, in[i].imag());

                            // Print lambda
                            for (int i = 0; i < d_fft_length; i++)
                                PRINTF("lambda[%i]: %.10f\n", i, d_lambda[i]);
                         }
#endif
                     }

#ifdef DEBUG_OFDMSYM_VERBOSE
                    PRINTF("short_acq: %i, d_cp_start: %i, d_to_consume: %i, d_to_out: %i\n", d_cp_found, d_cp_start, d_to_consume, d_to_out);
#endif

                    if (d_cp_found)
                    {
                        d_freq_correction_count = 0;

                        // Derotate the signal and out
#ifdef USE_VOLK
                        low = d_cp_start - d_fft_length + 1;
                        size = d_cp_start - (d_cp_start - d_fft_length + 1) + 1;

                        if(is_unaligned())
                            volk_32fc_x2_multiply_32fc_u(&out[0], &d_derot[0], &in[low], size);
                        else
#ifdef USE_VOLK_ALIGN
                            // broken GG??, change kernel to generic in ~/.volk/volk_config or use _u version
                            // volk_32fc_x2_multiply_32fc_a(&out[0], &d_derot[0], &in[low], size);
                            volk_32fc_x2_multiply_32fc_u(&out[0], &d_derot[0], &in[low], size);
#else
                            volk_32fc_x2_multiply_32fc_u(&out[0], &d_derot[0], &in[low], size);
#endif
                        // TODO - fix the alignment
#else
                        int j = 0;
                        for (int i = (d_cp_start - d_fft_length + 1); i <= d_cp_start; i++)
                        {
                            out[j] = d_derot[j] * in[i];
                            j++;
                        }
#endif
                    }
                    else
                    {
                        // If we have a number of consecutive misses then we restart aquisition
                        if (++d_freq_correction_count > d_freq_correction_timeout)
                        {
                            d_initial_aquisition = 0;
                            d_freq_correction_count = 0;

                            printf("restart aquisition\n");

                            // Restart wit a half number so that we'll not endup with the same situation
                            // This will prevent peak_detect to not detect anything
                            d_to_consume = d_to_consume / 2;
                        }
                    }
                }
#ifdef DEBUG_OFDMSYM_PERFORMANCE
                time_t now = time(0);
                outfile << "run time: " << now << std::endl;
#endif

                // Tell runtime system how many input items we consumed on
                // each input stream.
                consume_each(d_to_consume);

                // Tell runtime system how many output items we produced.
                return (d_to_out);
            }
    }/* namespace isdbt */
} /* namespace gr */

