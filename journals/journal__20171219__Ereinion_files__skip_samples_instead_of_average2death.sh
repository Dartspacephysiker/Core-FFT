# 2017/12/19
# In the last journal I used NAvg=1024, which produced 26 pages of output. That was far too large, you see, so I'm trying something new.
# The idea is to only use 8 averages (or 4096*8=32,768 samples), and to skip the other 1016 averages (or 4096*1016=4,161,536 samples)
#
# RESULT: There is clearly some action between 5284 and 6134 s, and possibly as early as 4404 s.
#         The length of the file is 22189.97 s (assuming 2 MHz sampling).
#         Thus, I'm looking for the chunks that are 4404/22189.97*100 = 19.84% and 6134/22189.97*100 = 27.64%.
#         These are bytes 17631941807.3088–24563854413.0048
#         These are bytes 17631941792–24563854400
# size=88870674432
# nsamp=size/2
# Try this:
# 
# binDir='/SPENCEdata/Research/database/ground-based/Cluster-SPS/2016/southpole5/Ereinion/DataTrans0117'
# binFile='SPSusrp1-ch0-20160811-000002-bw2000000-cf1000000.dat'
# binOut='SPSusrp1-ch0-20160811-000002-bw2000000-cf1000000__20-100pct.dat'
# binFinal='SPSusrp1-ch0-20160811-000002-bw2000000-cf1000000__20-28pct.dat'
# 
# First try; wrong block sizes
# dd if=${binDir}/${binFile} bs=512k | { dd bs=17631941792 count=1 of=/dev/null; dd bs=512k of=${binDir}/${binOut}; } 
# dd if=${binDir}/${binOut} bs=512k | { dd bs=6931912608 count=1 of=${binDir}/${binFinal}; dd bs=512k of=/dev/null; } 
#
# Second try (these did the job)
# dd if=${binDir}/${binFile} bs=512k | { dd bs=512k count=33630 of=/dev/null; dd bs=512k of=${binDir}/${binOut}; } 
# dd if=${binDir}/${binOut} bs=512k | { dd bs=512k count=13222 of=${binDir}/${binFinal}; dd bs=512k of=/dev/null; } 
NFFTBins=4096			# Number of FFT bins
NAvg=4				# Number of FFTs to average
avgsPerTStamp=1024
# topFreq=1541000			# in Hz
# topFreq=666000
maxFreq=1000000
minFreq=0

#0  20160629-004501
#1  20160811-000002
#2  20160820-001005
#3  20160822-012002
#4  20160925-004002
datNum=1

DATE=(20160629-004501 20160811-000002 20160820-001005 20160822-012002 20160925-004002)
CHNUM=0

ngTitle="SPS USRP CH${CHNUM}, ${DATE[${datNum}]}"
ngSubtitle="AVG=${NAvg}, NBIN=${NFFTBins}"

NAVGSKIP=$(( avgsPerTStamp - NAvg ))
NSAMP_SKIP_AFTER_AVG=$(( NAVGSKIP * NFFTBins ))

sFreq=2000000
dtSamp=`echo "1 / ${sFreq}" | bc`
T_ADD_AFTER_AVG=`echo "${dtSamp} * ${NFFTBins} * ${avgsPerTStamp}" | bc`

file="/SPENCEdata/Research/database/ground-based/Cluster-SPS/2016/southpole5/Ereinion/DataTrans0117/SPSusrp1-ch${CHNUM}-${DATE[datNum]}-bw2000000-cf1000000.dat"
# outfile="`basename ${file}`"
outfile="SPSusrp1-ch${CHNUM}-${DATE[datNum]}-bw2MHz-cf1MHz.dat"
outer="${outfile%%.dat}-${NAvg}avg_${NFFTBins}FFT_skip${NAVGSKIP}avgs.data"
ngdefFile="${outfile%%.dat}-${NAvg}avg_${NFFTBins}FFT_skip${NAVGSKIP}avgs.data.ngdef"
pdfFile="${outfile%%.dat}-${NAvg}avg_${NFFTBins}FFT_skip${NAVGSKIP}avgs.pdf"

saveDir=/usr/src/Core-FFT/saves_output_etc
plotDir=/usr/src/Core-FFT/plots

../core-fft.py -N ${NFFTBins} \
	       --max-freq=${maxFreq} \
	       --min-freq=${minFreq} \
	       -a ${NAvg} \
	       --ngdef \
	       --ngtitle="${ngTitle}" \
	       --ngsubtitle="${ngSubtitle}" \
	       --skip-avg=${NSAMP_SKIP_AFTER_AVG} \
	       --time-avg=${T_ADD_AFTER_AVG} \
	        ${file} ${saveDir}/${outer} && \
    ../convert_core-fft_output_to_ps.sh --input ${saveDir}/${outer} -d ${saveDir}/${ngdefFile} --outdir ${plotDir}
    
    # ps2pdf gray.ps ${plotDir}/${pdfFile}
