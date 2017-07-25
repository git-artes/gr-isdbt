#!/usr/bin/python

# Author: 
#   Pablo Flores Guridi
#   pablof@fing.edu.uy
#   Instituto de Ingenieria Electrica
#   Facultad de Ingenieria
#   Universidad de la Republica
#   Montevideo, Uruguay
#
# Date:
#   July 2, 2017

from collections import deque

# For printing the current configuration
VERBOSE = 0

# System configuration
mode = 1
cp = 1.0/8
segment_clock_periods = 96*2**(mode-1)
symbol_clock_periods = int(2**(10+mode)*(1+cp))

# 1 frame - X packets
# 1 frame - Y clocks
# Y/X = 408
clock_periods_per_tsp = 408.0

# Bytes per TSP = 408 (after depuncturing)
bytes_per_tsp = 408.0

# Mother FEC rate
mother_fec = 1.0/2

# Switch intervals per frame
switch_intervals_per_frame = int(2**(mode-1))

#Intervals for switching S3 and S4
switch_intervals = int(204.0/switch_intervals_per_frame)

# Delay between switching S3 and S4
switch_delay = 3
#----

# Layer A
mod_scheme_A = 6
fec_A = 7.0/8
num_segments_A = 13
clock_periods_A = num_segments_A*segment_clock_periods
#----

# Layer B
mod_scheme_B = 6
fec_B = 3.0/4
num_segments_B = 0
clock_periods_B = num_segments_B*segment_clock_periods
#----

# Layer C
mod_scheme_C = 6
fec_C = 7.0/8
num_segments_C = 0
clock_periods_C = num_segments_C*segment_clock_periods
#-----

# Global variables
hier_buffer_A = 0.0
hier_buffer_B = 0.0
hier_buffer_C = 0.0
ts_buffer1 = deque([])
ts_buffer2 = deque([])
#----

for k in range(0,switch_intervals_per_frame):
  for i in range(0,switch_intervals):
    for j in range(0,symbol_clock_periods):
      
      # We store the samples corresponding to layer A
      if (j<clock_periods_A):
        hier_buffer_A = hier_buffer_A + mod_scheme_A

      # We store the samples corresponding to layer B
      elif (clock_periods_A <= j < clock_periods_A + clock_periods_B):
        hier_buffer_B = hier_buffer_B + mod_scheme_B 
      
      # We store the samples corresponding to layer C
      elif (clock_periods_A + clock_periods_B <= j < clock_periods_A + clock_periods_B + clock_periods_C):
        hier_buffer_C = hier_buffer_C  + mod_scheme_C 

      # If the stored data in layer A buffer reaches the amount of one TSP
      if (hier_buffer_A >= bytes_per_tsp*8*mother_fec/fec_A):
        hier_buffer_A = hier_buffer_A - bytes_per_tsp*8*mother_fec/fec_A
        # we see if we should store the TSP in TS Reproduction Unit number 1
        if ((k % 2) ==0):
          ts_buffer1.append("A")
        # If not, we store the TSP in TS Reproduction Unit number 2
        else:
          ts_buffer2.append("A")
      
      # If the stored data in layer B buffer reaches the amount of one TSP
      if (hier_buffer_B >= bytes_per_tsp*8*mother_fec/fec_B):
        hier_buffer_B = hier_buffer_B - bytes_per_tsp*8*mother_fec/fec_B
        # we see if we should store the TSP in TS Reproduction Unit number 1
        if ((k % 2) == 0):
          ts_buffer1.append("B")
        # If not, we store the TSP in TS Reproduction Unit number 2
        else:	
          ts_buffer2.append("B")
     
      # If the stored data in layer C buffer reaches the amount of one TSP
      if (hier_buffer_C >= bytes_per_tsp*8*mother_fec/fec_C):
        hier_buffer_C = hier_buffer_C - bytes_per_tsp*8*mother_fec/fec_C
        # we see if we should store the TSP in TS Reproduction Unit number 1
        if ((k % 2) == 0):
          ts_buffer1.append("C")
        # If not, we store the TSP in TS Reproduction Unit number 2
        else:
          ts_buffer2.append("C")
      
      # If it is time to read a TSP
      if (((j + i*symbol_clock_periods) % (clock_periods_per_tsp) == (clock_periods_per_tsp-1))):
        # See if we should read from TS Reproduction Unit number 1
        if((((k % 2) == 0) and (j+i*symbol_clock_periods)>=(switch_delay*clock_periods_per_tsp-1)) or (((k % 2) == 1) and (j+i*symbol_clock_periods)<(switch_delay*clock_periods_per_tsp-1))):
          # If there is something to read in the buffer we read it
          if(len(ts_buffer1)>0):
            print ts_buffer1.popleft(),
          # If not, we read a null packet
          else:
            print "N",
        # If we shouldn't read from Reproduction Unit number 1, we read from TS Reproduction Unit number 2
        else:
          # If there is something to read in the buffer we read it
          if(len(ts_buffer2)>0):
            print ts_buffer2.popleft(),
          # If not, we read a null packet
          else:
            print "N",


if (VERBOSE):
  FEC_VALUES = {1.0/2: '1/2',  2.0/3: '2/3', 3.0/4: '3/4', 5.0/6: '5/6', 7.0/8: '7/8'}
  CP_VALUES = {1.0/4: '1/4', 1.0/8: '1/8', 1.0/16: '1/16', 1.0/32: '1/32'}
  MOD_VALUES = {2: 'QPSK', 4: '16QAM', 6: '64QAM'}
  print "\nCONFIGURATION:"
  print "Mode:", mode
  print "CP:", CP_VALUES[cp]
  print "Layer A:" 
  print "\tModulation scheme:", MOD_VALUES[mod_scheme_A]
  print "\tFEC:", FEC_VALUES[fec_A]
  print "\tNumber of segments:", num_segments_A
  print "Layer B:"
  print "\tModulation scheme:", MOD_VALUES[mod_scheme_B]
  print "\tFEC:", FEC_VALUES[fec_B]
  print "\tNumber of segments:", num_segments_B
  print "Layer C:"
  print "\tModulation scheme:", MOD_VALUES[mod_scheme_C]
  print "\tFEC:", FEC_VALUES[fec_C]
  print "\tNumber of segments:", num_segments_C
