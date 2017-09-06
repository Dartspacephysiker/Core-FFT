#Core-FFT

###Things you might have trouble with
------------------------------------------------------------------------------------
Error message                                    | Solution                        |
-------------------------------------------------|---------------------------------|
fatal error: Python.h: No such file or directory | sudo apt-get install python-dev |
------------------------------------------------------------------------------------

Looking to run this show? Make sure you include the following on the command line:--bins [N], -F [freq], and both input and output files. Otherwise you will be in a world of hurt trying to figure out why nothing runs.

(2017/09/06) Here's an example using (I believe) real data, 1024 FFT bins, a 5-MHz sampling rate, and 12 FFT averages:
```
#First, FFTage (Micah's parlance)
./core-fft.py -N 1024 -F 5000000 -a 12 --ngdef $file "${outfile%%.data}--12avg.data"

#Now run gray and all that
./convert_core-fft_output_to_ps.sh --input ${file} -d "${outfile%%.data}--12avg.data.ngdef"
```
