ISDB-T receiver in GNU Radio

An opensource implementation of Digital Television standard ISDB-T
(in particular the so-called "international" version, or ISDB-Tb)
receiver in GNU Radio.

We are in early development stages, any help is welcomed.
Our first blocks are heavily based on Bogdan Diaconescu's (BogdanDIA on git) DVB-T implementation (see https://github.com/BogdanDIA/gr-dvbt). 

Notes:
    The isdb-t receiver supports all constellation types QPSK, QAM16, QAM64 and rates: 1/2, 2/3, 3/4, 5/6, 7/8.
    All combinations of rates and constellations are tested and working for 2k OFDM.

Examples:
    examples/rx_demo.grc
    examples/viewing_the_constellation.grc

Build instructions:
    mkdir build
    cd build
    cmake ../
    make
    sudo make install
    sudo ldconfig

IIE Instituto de Ingeniería Eléctrica
Facultad de Ingeniería
Universidad de la República
Montevideo, Uruguay
http://iie.fing.edu.uy/investigacion/grupos/artes/

