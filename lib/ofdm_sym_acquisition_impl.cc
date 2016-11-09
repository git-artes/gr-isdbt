/* -*- c++ -*- */
/*  
 * Copyright 2013,2014,2015 <Bogdan Diaconescu, yo3iiu@yo3iiu.ro>.
 * Copyright 2015, several modifications
 *   Federico "Larroca" La Rocca <flarroca@fing.edu.uy>
 *   Pablo Belzarena 
 *   Gabriel Gomez Sena 
 *   Pablo Flores Guridi 
 *  Victor Gonzalez Barbone
 *
 *  Instituto de Ingenieria Electrica, Facultad de Ingenieria,
 *  Universidad de la Republica, Uruguay.
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

#include <gnuradio/io_signature.h>
#include "ofdm_sym_acquisition_impl.h"
#include <complex>
#include <math.h>
#include <gnuradio/math.h>
#include <gnuradio/expj.h>
#include <stdio.h>
#include <volk/volk.h>
#include <gnuradio/fxpt.h>
#include <math.h>

namespace gr {
    namespace isdbt {

        const int ofdm_sym_acquisition_impl::d_total_segments = 13;
        const int ofdm_sym_acquisition_impl::d_carriers_per_segment_2k = 108;  

        void 
            ofdm_sym_acquisition_impl::peak_detect_init(float threshold_factor_rise, float alpha)
            {
                d_avg_alpha = alpha;
                d_threshold_factor_rise = threshold_factor_rise;
                // d_avg = 0;
                d_avg_max = - (float)INFINITY;
                d_avg_min =   (float)INFINITY;
            }

        int 
            ofdm_sym_acquisition_impl::peak_detect_process(const float * datain, const int datain_length, int * peak_pos)
            {
                #if VOLK_GT_122
                uint32_t peak_index = 0;
                #else
                unsigned int peak_index = 0;
                #endif
                bool success = true;

                volk_32f_index_max_16u(&peak_index, &datain[0], datain_length); 

                if (datain_length>=d_fft_length){
                    float min = datain[(peak_index + d_fft_length/2) % d_fft_length];
                    if(d_avg_min==(float)INFINITY){
                        d_avg_min = min;
                    }
                    else 
                    {
                        d_avg_min = d_avg_alpha*min + (1-d_avg_alpha)*d_avg_min;
                    }

                }

                if (d_avg_max==-(float)INFINITY)
                {
                    // I initialize the d_avg with the first value. 
                    d_avg_max = datain[peak_index];
                }
                else if ( datain[ peak_index ] > d_avg_max - d_threshold_factor_rise*(d_avg_max-d_avg_min) ) 
                {
                    d_avg_max = d_avg_alpha * datain[ peak_index ] + (1 - d_avg_alpha) * d_avg_max;
                }
                else
                {
                    success = false; 
                    //printf("gr-isdbt.OFDM_SYM_ACQ: peak under/over average! peak %f, avg_max %f, avg_min %f\n", datain[ peak_index ], d_avg_max, d_avg_min); 
                }

                //We now check whether the peak is in the border of the search interval. This would mean that 
                //the search interval is not correct, and it should be re-set. This happens for instance when the 
                //hardware dropped some samples. 
                //Our definition of "border of the search interval" depends if the search interval is "big" or not. 
                if ( datain_length < d_fft_length )
                {
                    if ( ( peak_index == 0 ) || ( peak_index == datain_length-1 ) )
                    {
                        success = false; 
                        //printf("gr-isdbt.OFDM_SYM_ACQ: peak at border! peak %f, avg_max %f, avg_min %f, peak_index: %i\n", datain[ peak_index ], d_avg_max, d_avg_min, peak_index); 
                    }
                }
                else
                {
                    if ( ( peak_index < 5 ) || ( peak_index > datain_length-5 ) )
                    {
                        success = false; 
                        //printf("gr-isdbt.OFDM_SYM_ACQ: peak at border! peak %f, avg_max %f, avg_min %f, peak_index: %i\n", datain[ peak_index ], d_avg_max, d_avg_min, peak_index); 
                    }
                }

                *peak_pos = peak_index;
                return (success);
 
           }

        int
            ofdm_sym_acquisition_impl::ml_sync(const gr_complex * in, int lookup_start, int lookup_stop, int * cp_pos, float * peak_epsilon)
            {

                assert(lookup_start >= lookup_stop);
                assert(lookup_stop >= (d_cp_length + d_fft_length - 1));

                int low, size;

                // Calculate norm
                low = lookup_stop - (d_cp_length + d_fft_length - 1);
                size = lookup_start - low + 1;
                volk_32fc_magnitude_squared_32f(&d_norm[low], &in[low], size);

                // Calculate gamma on each point
                //TODO check these boundaries!!!!!!!
                low = lookup_stop - (d_cp_length - 1);
                //low = lookup_stop - d_cp_length - 1;
                //size = lookup_start - (lookup_stop - d_cp_length - 1) + 1;
                size = lookup_start - low + 1;
                volk_32fc_x2_multiply_conjugate_32fc(&d_corr[low - d_fft_length], &in[low], &in[low - d_fft_length], size);

                // Calculate time delay and frequency correction
                float sum_aux = 0;
                for (int i = lookup_start - 1; i >= lookup_stop; i--)
                {
                    int k = i - lookup_stop;

                    //TODO no accumulator for complexes in VOLK for the moment.    
                    volk_32fc_32f_dot_prod_32fc(&d_gamma[k], &d_corr[i-d_cp_length+1-d_fft_length], d_ones, d_cp_length);
                    volk_32f_accumulator_s32f(&d_phi[k], &d_norm[i-d_cp_length+1], d_cp_length);
                    volk_32f_accumulator_s32f(&sum_aux, &d_norm[i-d_cp_length+1-d_fft_length], d_cp_length);
                    d_phi[k] += sum_aux;
                }

                // Init lambda with gamma
                low = 0;
                size = lookup_start - lookup_stop;
                volk_32fc_magnitude_32f(&d_lambda[low], &d_gamma[low], size);

                // Calculate lambda
                low = 0;
                size = lookup_start - lookup_stop;
                volk_32f_s32f_multiply_32f(&d_phi[low], &d_phi[low], d_rho / 2.0, size);
                volk_32f_x2_subtract_32f(&d_lambda[low], &d_lambda[low], &d_phi[low], size);

                int peak = 0;
                bool found_peak = true; 

                // Find peaks of lambda
                // We have found an end of symbol at peak_pos[0] + CP + FFT
                if ((found_peak = peak_detect_process(&d_lambda[0], (lookup_start - lookup_stop), &peak)))
                {
                    *cp_pos = peak + lookup_stop;

                    // Calculate frequency correction
                    *peak_epsilon = fast_atan2f(d_gamma[peak]);
                }
                return (found_peak);

           }

        // Derotates the signal 
        void 
            ofdm_sym_acquisition_impl::derotate(const gr_complex * in, gr_complex * out)
            {
                double sensitivity = (double)(-1) / (double)d_fft_length;
                d_phaseinc = sensitivity * d_peak_epsilon;

                gr_complex phase_increment = gr_complex(std::cos(d_phaseinc), std::sin(d_phaseinc)); 
                gr_complex phase_current = gr_complex(std::cos(d_phase), std::sin(d_phase)); 

                volk_32fc_s32fc_x2_rotator_32fc(&out[0], &in[0], phase_increment, &phase_current, d_fft_length) ; 
                d_phase = std::arg(phase_current); 
                d_phase = fmod(d_phase + d_phaseinc*d_cp_length, (float)2*M_PI);
           }


        void
            ofdm_sym_acquisition_impl::send_sync_start()
            {
                const uint64_t offset = this->nitems_written(0);
                pmt::pmt_t key = pmt::string_to_symbol("sync_start");
                pmt::pmt_t value = pmt::from_long(1);
                this->add_item_tag(0, offset, key, value);
            }


        ofdm_sym_acquisition::sptr
            ofdm_sym_acquisition::make(int fft_length, int cp_length, float snr)
            {
                return gnuradio::get_initial_sptr
                    (new ofdm_sym_acquisition_impl(fft_length, cp_length, snr));
            }

        /*
         * The private constructor
         */
        ofdm_sym_acquisition_impl::ofdm_sym_acquisition_impl(int fft_length, int cp_length, float snr)
            : gr::block("ofdm_sym_acquisition",
                    gr::io_signature::make(1, 1, sizeof (gr_complex) ),
                    gr::io_signature::make2(1, 2, sizeof (gr_complex) * fft_length,sizeof(float))),
            d_fft_length(fft_length), d_cp_length(cp_length), d_snr(snr),
            d_index(0), d_phase(0.0), d_phaseinc(0.0), d_cp_found(0), d_count(0), d_nextphaseinc(0), d_nextpos(0), \
              d_sym_acq_count(0),d_sym_acq_timeout(100), d_initial_aquisition(0), \
              d_freq_correction_count(0), d_freq_correction_timeout(0)
        {
            set_relative_rate(1.0 / (double) (d_cp_length + d_fft_length));

            printf("[gr-isdbt.OFDM_SYM_ACQ] You are using ofdm_sym_acquisition to perform coarse OFDM synchronization. Consider using ofdm_synchronization instead. \n");

            int mode = 3;
            if (d_fft_length==4096)
            {
                mode = 2;
            } 
            else if (d_fft_length==2048)
            {
                mode = 1;
            }
            d_active_carriers = (1+d_total_segments*d_carriers_per_segment_2k*pow(2.0,mode-1)); 
            d_snr = pow(10, d_snr / 10.0);
            d_rho = d_snr / (d_snr + 1.0);

            //VOLK alignment as recommended by GNU Radio's Manual. It has a similar effect 
            //than set_output_multiple(), thus we will generally get multiples of this value
            //as noutput_items. 
            const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
            set_alignment(std::max(1, alignment_multiple));

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

            d_phi = new float[d_fft_length];
            d_ones = new float[d_active_carriers];
            for(int i=0; i<d_active_carriers; i++)
            {
                d_ones[i] = 1.0;
            }

            d_peak_epsilon = 0;
            //peak_detect_init(0.2, 0.25, 30, 0.0005);
            // peak_detect_init(0.8, 0.9, 30, 0.9);
            peak_detect_init(0.3, 0.9);
        }

        /*
         * Our virtual destructor.
         */
        ofdm_sym_acquisition_impl::~ofdm_sym_acquisition_impl()
        {
            delete [] d_gamma;
            delete [] d_lambda;
            delete [] d_phi; 
            delete [] d_derot;
            delete [] d_conj;
            delete [] d_norm;
            delete [] d_corr;
            delete [] d_ones;
        }

        void
            ofdm_sym_acquisition_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
            {
                int ninputs = ninput_items_required.size ();
                // make sure we receive at least (symbol_length + fft_length)
                for (int i = 0; i < ninputs; i++)
                {
                    // ninput_items_required[i] = (2 * d_fft_length + d_cp_length) * noutput_items;
                    ninput_items_required[i] = ( d_cp_length + d_fft_length ) * (noutput_items + 1) ;
                }
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

                float *freq_error_out = (float *) output_items[1];
                bool freq_error_out_connected = output_items.size()>=2; 

                int low, size;

                d_consumed = 0;
                d_out = 0;

                //printf("is_unaligned():%s\n",is_unaligned() ? "True":"False");

                for (int i = 0; i < noutput_items ; i++) {
                    if (!d_initial_aquisition)
                    {
                        // If we are here it means that we have no idea where the CP may be. We thus 
                        // search it thoroughly
                        d_initial_aquisition = ml_sync(&in[d_consumed], 2 * d_fft_length + d_cp_length - 1, d_fft_length + d_cp_length - 1, &d_cp_start, &d_peak_epsilon);
                        d_cp_found = d_initial_aquisition; 
                    }
                    else
                    {
                        //If we are here it means that in the previous iteration we found the CP. We
                        //now thus only search near it. 
                        d_cp_found = ml_sync(&in[d_consumed], d_cp_start + 8, std::max(d_cp_start - 8,d_cp_length+d_fft_length-1), &d_cp_start, &d_peak_epsilon);
                        if ( !d_cp_found )
                        {
                            // We may have not found the CP because the smaller search range was too small (rare, but possible, in 
                            // particular when sampling time error are present). We thus re-try with a bigger search range and 
                            // update d_cp_start. 
                            d_cp_found = ml_sync(&in[d_consumed], d_cp_start+16, std::max(d_cp_start-16,d_cp_length+d_fft_length-1), &d_cp_start, &d_peak_epsilon );
                        }
                    }

                    if ( d_cp_found )
                    {
                        // safe-margin. Using a too adjusted CP position may result in taking samples from the NEXT ofdm 
                        // symbol. It is better to stay on the safe-side (plus, 10 samples is nothing in this context). 
                        d_cp_start_offset = -10;  

                        low = d_cp_start + d_cp_start_offset - d_fft_length + 1 ;
                        derotate(&in[d_consumed+low], &out[i*d_fft_length]);

                        if (freq_error_out_connected){
                            freq_error_out[i] = d_peak_epsilon/(2*3.14159); 
                        }

                        d_out++; 

                    }
                    else
                    {
                        // Send sync_start downstream
                        send_sync_start();
                        d_initial_aquisition = false;

                        // Restart with a half number so that we'll not endup with the same situation
                        // This will prevent peak_detect to not detect anything
                        d_consumed += (d_cp_length+d_fft_length)/2;
                        consume_each(d_consumed);
                        // Tell runtime system how many output items we produced.
                        return (d_out);
                    }

                    d_consumed += d_cp_length+d_fft_length;
                }

                // Tell runtime system how many input items we consumed on
                // each input stream.
                consume_each(d_consumed);

                // Tell runtime system how many output items we produced.
                return (d_out);
            }

    } /* namespace isdbt */
} /* namespace gr */

