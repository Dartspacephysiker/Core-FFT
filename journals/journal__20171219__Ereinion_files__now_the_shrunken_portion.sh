NFFTBins=4096			# Number of FFT bins
NAvg=16				# Number of FFTs to average
avgsPerTStamp=16
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
dtSamp=`echo "1 / ${sFreq}" | bc -l`
T_ADD_AFTER_AVG=`echo "${dtSamp} * ${NFFTBins} * ${avgsPerTStamp}" | bc -l`

# tStartString="2016-08-11/01:28:12.59"  #Can't do millisec in C (not easily), and file started two sec after 00:00:00
# 
tStartString="2016-08-11/01:28:13"

tString="m"

suff='20-28pct'
suff='24-28pct'

if [ "${DATE[datNum]}" = "20160811-000002" ]; then
    dir=/SPENCEdata/Research/database/ground-based/Cluster-SPS/2016/southpole5/Ereinion/DataTrans0117
else
    dir=/thelonious_data2/ground-based/Cluster-SPS/2016/southpole5/Ereinion/DataTrans0117/
fi
file="${dir}/SPSusrp1-ch${CHNUM}-${DATE[datNum]}-bw2000000-cf1000000.dat"
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
	       --time-units=${tString} \
	       --time-start-string=${tStartString} \
	        ${file} ${saveDir}/${outer} && \
    ../convert_core-fft_output_to_ps.sh --input ${saveDir}/${outer} -d ${saveDir}/${ngdefFile} --outdir ${plotDir}
    
