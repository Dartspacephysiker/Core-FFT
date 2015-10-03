#2015/10/03
#Trying to figure out why the FFTing doesn't work
set args ./core-fft.py --ngdef --nffts=100 --header=0 -a 4 --bins=1024 2014_Andoya_cals_processed/parsed_TM123_measurements/17--parsed_and_combined/17--TM1--binaryVLF--VLF-A.out 2014_Andoya_cals_processed/FFTs/17/17--TM1--binaryVLF--VLF-A.gray

#convenience
set $iCount = 0

#Now do a breakpoint & stuff
set breakpoint pending on 

########################
#For breaking inside loop where input vals are converted 
# break uint16-1chan.c:157
# break uint16-1chan.c:163 if ( $iCount % 133 ) == 0

# commands 1
# silent
# set $iCount = $iCount + 1
# continue
# end
############################
