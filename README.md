### ISDB-T receiver in GNU Radio

**An open source implementation of a receiver for the Digital Television standard ISDB-T (ARIB's STD-B31) in GNU Radio.**

In particular, here in Uruguay we use the so-called "international" version, or ISDB-Tb (ABNT NBR 15601). However, the transmission scheme is exactly the same as in the original version, thus both references are equivalent.  

We are in early (but very active) development stages, and any help and/or feedback is welcome. Since ISDB-T is similar to DVB-T, some of our blocks are heavily based on Bogdan Diaconescu's (BogdanDIA on git) DVB-T implementation (see https://github.com/BogdanDIA/gr-dvbt). 

**Notes and examples**

Roughly speaking, the receiver may be currently separated in two segments regarding operation: 
- *Up to equalization and TMCC decoding.* The receiver has been tested with mode-3 transmission (8k FFT) and several CP lengths. It works online (we used USRP B100 and BladeRF X115) and is able to display the complete constellation. It should work with the other two modes, but we have not tested it yet. See examples/viewing_the_constellation.grc. 
- *After TMCC decoding.* The receiver is capable of displaying on-line the 1-segment transmission if present. See examples/rx_1seg_demo.grc. The output of the flowgraph is a Transport Stream file which may be played by mplayer (for instance). If before executing the flowgraph we create the file as a pipe (mkfifo test_out.ts), it should display the video online.   

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
