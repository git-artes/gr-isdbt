### ISDB-T receiver in GNU Radio

**An open source implementation of a receiver for the Digital Television standard ISDB-T (ARIB's STD-B31) in GNU Radio.**

In particular, here in Uruguay we use the so-called "international" version, or ISDB-Tb (ABNT NBR 15601). However, the transmission scheme is exactly the same as in the original version, thus both references are equivalent.  

We are in not-so-early (but still very active) development stages, and any help and/or feedback is still welcome. Since ISDB-T is similar to DVB-T, some of our blocks are heavily based on Bogdan Diaconescu's (BogdanDIA on git) DVB-T implementation (see https://github.com/BogdanDIA/gr-dvbt). 

**Notes and examples**

The receiver is now complete regarding operation. It is capable of demodulating the complete ISDB-T spectrum and its operation is such that a i5 PC is capable of showing the HD signal on-line. We foresee three modes of operation: 
- **Obtaining the transmission parameters.** ISDB-T has several transmission parameters which may be selected by the transmitter and may (and probably will) change among channels and segments. Our current implementation is not yet capable of automatically changing its parameters, and its up to the user to configure the receiver correctly. However, a simple example is provided (see examples/obtaining_parameters.grc) which may be used to obtain such parameters. Transmission mode and CP length should be tested until a clear peak is viewed (before using this flowgraph **for the first time**, open examples/sym_align_ofdm.grc, generate the code, open and close the companion). The TMCC decoder block should be configured to print the rest of the parameters. 
- **Vector Analyzer.** See examples/viewing_the_constellation.grc on how to view the received complex symbols. The receiver has been tested with mode-3 transmission (8k FFT) and several CP lengths. It should work with the other two modes, but we have not tested it yet.  
- **Complete decoding and displaying the video/audio.** The receiver is capable of displaying on-line whole segments. If a 1-seg transmission is present, the example in examples/rx_1seg_demo.grc may be used to decode it. Layer B is decoded in examples/rx_demo.grc. Note that the transmission's parameters should be set accordingly by using examples/obtaining_parameters.grc. The output of both flowgraphs is a Transport Stream file which may be played by ffplay or mplayer (for instance). If before executing the flowgraph we create the file as a pipe (mkfifo test_out.ts), it should display the video online.   

For more information (papers, signal recording of ISDB-T signals, etc.) please visit our webpage: http://iie.fing.edu.uy/investigacion/grupos/artes/gr-isdbt/.    

**Build instructions**

git clone https://github.com/git-artes/gr-isdbt.git  
cd gr-isdbt  
mkdir build  
cd build  
cmake ../  
make && sudo make install  

If installed for the first time:   
sudo ldconfig  

IIE Instituto de Ingeniería Eléctrica  
Facultad de Ingeniería  
Universidad de la República  
Montevideo, Uruguay  
http://iie.fing.edu.uy/investigacion/grupos/artes/  
  
Please refer to the LICENSE file for contact information and further credits.   
