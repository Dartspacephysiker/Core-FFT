#!/usr/bin/python2.7

import ConfigParser
from optparse import OptionParser
import coreFFT
import ggseHeader
import sys
from datetime import date

def ngwrite(fn, nyq, yhigh, ylow, yinc, blevel, wlevel, title, subtitle=" "):
    ngfile = open(fn+".ngdef","w+")

    ngfile.write("y_data_num        " + str(nyq) + "\n")
    ngfile.write("y_data_min        " + str(ylow) + "\n")
    ngfile.write("y_data_max        " + str(yhigh) + "\n")
    ngfile.write("y_label_min       " + str(ylow) + "\n")
    ngfile.write("y_label_max       " + str(yhigh) + "\n")
    ngfile.write("y_label_increment " + str(yinc) + "\n")
    ngfile.write("data_black_level  " + str(blevel) + "\n")
    ngfile.write("data_white_level  " + str(wlevel) + "\n")
    ngfile.write("title             " + str(title) + "\n")
    ngfile.write("sub_title         " + str(subtitle) + "\n")
    ngfile.write("x_axis_label      " + " " + "\n")
    ngfile.write("y_axis_label      " + " " + "\n")
    ngfile.write("use_magic_stamps  " + "no" + "\n")
    ngfile.write("magic_char        " + "*" + "\n")

    ngfile.close()
    
    return 0

parser = OptionParser(usage="""pyfft: command line fftage,

    %prog [options] <input file> [output file]""")

parser.set_defaults(avg=0,phases=False,ng=False,X=0,agc_bin=0,agc_lvl=0, \
                    time_stop=0,time_avg=0,time_nfft=0,time_fftmod=0,skip=0, \
                    bl_last=-1,bl_first=-1,nchan=1,complex=False,header=0, \
                    skip_avg=0,skip_fft=0,timefile="",verbose=False,threads=1, \
                    uint8=False,windiv=1,ss=0,overlap=0,time_start=0)

parser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                help="print status messages to stdout [default: quiet].")
parser.add_option("-C", "--complex", action="store_true", dest="complex",
                help="input is complex (series of QI, short ints) [not].")
parser.add_option("-U", "--uint8", action="store_true", dest="uint8",
                help="input is uint8 [not].")
parser.add_option("-D", "--float", dest="float", type='int',
                help="floating point inputs (4 or 8-byte) [0].")
parser.add_option("-H", "--header", dest="header", type='int', 
                help="parse a header from the input file [0=No];1=Head;4=Foot.")
parser.add_option("-N", "--bins", dest="N", type='int', 
                help="number of fft bins.")
parser.add_option("-O", "--overlap", dest="overlap", type='int', 
                help="number of bins to overlap FFTs by [0].")
parser.add_option("-W", "--windiv", dest="windiv", type='int',
                help="# of times to cycle window over N [1].")
parser.add_option("-a", "--avg", dest="avg", type='int', 
                help="number of ffts to average.")
parser.add_option("--time-avg", dest="time_avg", type='float', 
                help="time to add after each avg [0].")
parser.add_option("--time-fft", dest="time_fft", type='float', 
                help="time to add after each fft [0].")
parser.add_option("--skip-avg", dest="skip_avg", type='int', 
                help="number of samples to skip after each avg [0].")
parser.add_option("--skip-fft", dest="skip_fft", type='int', 
                help="number of samples to skip after each fft [0].")
parser.add_option("--bl-last", dest="bl_last", type='int', 
                help="last bin to output to file [nyq].")
parser.add_option("--bl-first", dest="bl_first", type='int', 
                help="first bin to output to file [0].")
parser.add_option("--agc-bin", dest="agc_bin", type='int', 
                help="bin containing AGC frequency.")
parser.add_option("--agc-lvl","--agc-level", dest="agc_lvl", type='float', 
                help="level to AGC to.")
parser.add_option("--ngdef", action="store_true", dest="ng", 
                help="write timestamps, and ngray.def after completion [False].")
parser.add_option("--tf", "--timefile", dest="timefile", type="string", 
                help="file containing timestamps (one per avg) [none].")
parser.add_option("-n", "--nchan", dest="nchan", type='int',
                help="number of channels [1].")
parser.add_option("-T", "--threads", dest="threads", type='int',
                help="maximum number of FFTW threads [1].")
parser.add_option("-p", "--phase", action="store_true", dest="phases", 
                help="write phases after magnitudes [True].")
parser.add_option("-F", "--freq", "--frequency", dest="freq", type='float', 
                help="frequency for timestamps [10MHz].")
parser.add_option("-S", "--start", "--start-sample", dest="ss", type='int', 
                help="sample to start at [0].")
parser.add_option("-X", "--nffts", dest="X", type='int', 
                help="number of ffts to take [inf].")

(o, args) = parser.parse_args()

if (o.agc_bin or o.agc_lvl) and not (o.agc_bin and o.agc_lvl):
    sys.stderr.write("ggse-fft: agc-bin and agc-lvl must both be set.\n")
    parser.print_help()
    sys.exit()

if len(args) < 2: 
    sys.stderr.write("ggse-fft: Need input file, output string.\n")
    parser.print_help()
    sys.exit()
else:
    o.infile = str(args[0])
    o.outfile = str(args[1])
print o.infile
print o.outfile
# Read in config file
# read in clo

# read in header
#if o.header == 1:
#    head = ggseHeader.ff(o.infile,mode='head')
#elif o.header == 4:
#    head = ggseHeader.ff(o.infile,mode='tail')

# fill option struct
options = coreFFT.core_param()

options.infile = o.infile
options.outfile = o.outfile
options.timefile = o.timefile
options.N = o.N #if bool(o.N) else head.getint('ggse_header','chipsize')
options.overlap = o.overlap
options.windiv = o.windiv
options.bl_last = o.bl_last
options.bl_first = o.bl_first
options.skip_avg = o.skip_avg
options.skip_fft = o.skip_fft
options.avg = o.avg if o.avg > 0 else 1 #head.getint('ggse_header','chipblock')
options.n_chan = o.nchan #if bool(o.nchan) else head.getint('ggse_header','n_chan')
options.n_ffts = o.X
options.agc_bin = o.agc_bin
options.agc_level = o.agc_lvl

if o.freq > 0:
    options.frequency = o.freq
#elif o.header:
#    head.getfloat('ggse_header','frequency')
    
if o.time_start != 0:
    options.time_start = o.time_start
#elif o.header:
#    options.time_start = head.getint('ggse_header','time')
else:
    options.time_start = 0
     
options.time_stop = o.time_stop
options.time_avg = o.time_avg
options.time_nfft = o.time_nfft
options.time_fftmod = o.time_fftmod
options.window = "hann"

options.n_startsample = o.ss
options.verbose = o.verbose
options.threads = o.threads
options.continuous = False
options.phases = o.phases
options.binary = False
options.linear = False
options.oldheader = False
options.newheader = coreFFT.COREH_NONE

#print options.N

retstr = coreFFT.core_return()

if o.nchan == 1:
    if o.complex:
        print "Complex, 1-Channel"
        ret = coreFFT.complex_1chan(options,retstr)
    elif o.uint8:
        print "uint8, 1-Channel"
        ret = coreFFT.uint8_1chan(options,retstr)
    elif o.float == 4:
        print "float4, 1-Channel"
        ret = coreFFT.float4_1chan(options,retstr)
    elif o.float == 8:
        print "float8, 1-Channel"
        ret = coreFFT.float8_1chan(options,retstr)
    else:
        print "Real, 1-Channel"
        ret = coreFFT.uint16_1chan(options,retstr)
else:
    if (not o.complex) and (not o.uint8):
        print "Real, N-Channel"
        ret = coreFFT.real_nchan(options,retstr)
    else:
        print "Error: multichannel not implemented."

if ret == 0:
    if o.header > 0:
        newh = ggseHeader.ggseHeader('fft')
        
        ggseHeader.cpSectionCopy(head,'ggse_header',newh,'ggse_header')
        
        newh.set('ggse_header','fft_bins',options.N)
        if o.skip_avg > 0: newh.set('ggse_header','fft_skip_avg',o.skip_avg)
        if o.skip_fft > 0: newh.set('ggse_header','fft_skip_fft',o.skip_fft)
        newh.set('ggse_header','fft_avg',options.avg)
        newh.set('ggse_header','fft_phases',False)
        newh.set('ggse_header','fft_total',retstr.nfft)
        
        if o.agc_bin: newh.set('ggse_header','fft_agc',True)
    
    if o.nchan == 1:
        if o.header > 0:
            newh.set('ggse_header','fft_channel',1)
            ggseHeader.tf(o.outfile,newh,mode='append',replace=False)
        
        if o.ng:
            tinc = 2.0
            
            ylow = 0.0 if o.bl_first == -1 else o.freq/o.N*o.bl_first*0.001
            yhigh = o.freq/2000.0 if o.bl_last == -1 else o.freq/o.N*o.bl_last*0.001
            yinc = (yhigh-ylow)/20.0
        
            if o.bl_first != -1 and o.bl_last != -1:
                nyq = o.bl_last-o.bl_first
            elif o.bl_first != -1:
                nyq = o.N/2-o.bl_first
            elif o.bl_last != -1:
                nyq = o.bl_last
            elif o.complex:
                nyq = o.N
            else:
                nyq = o.N/2
        
            ngwrite(o.outfile, nyq, yhigh, ylow, yinc, \
                    retstr.max[0], retstr.min[0], \
                    o.outfile)

    else:
        for i in range(0,options.n_chan):
            outfn = "%s-ch%02i.fft" % (o.outfile,i+1)
            dt = date.today()
            dt = "{0}{1:02d}{2:02d}".format(int(dt.year),int(dt.month),int(dt.day))

            if o.header > 0:
                newh.set('ggse_header','fft_channel',i)                
                ggseHeader.tf(outfn,newh,mode='append',replace=False)
                date = head.get('ggse_header','date')
            
            if o.ng:
                ttime = retstr.time_total
        #        tdiv = 
                tinc = 2.0
                
                ylow = 0.0 if o.bl_first == -1 else o.freq/o.N*o.bl_first
                yhigh = o.freq/2000.0 if o.bl_last == -1 else o.freq/o.N*o.bl_last
                yinc = (yhigh-ylow)/20.0
            
                ngwrite(outfn, o.N/2, yhigh, ylow, yinc, \
                        retstr.max[i], retstr.min[i], \
                        dt, \
                        subtitle = "Channel " + str(i+1))
                
else:
    print "FFTing failed!"
    
