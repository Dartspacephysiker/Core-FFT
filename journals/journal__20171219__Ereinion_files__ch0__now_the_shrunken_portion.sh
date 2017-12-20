NFFTBins=4096			# Number of FFT bins
NAvg=16				# Number of FFTs to average
avgsPerTStamp=16
# topFreq=1541000			# in Hz
# topFreq=666000
maxFreq=1000000
minFreq=0

CHNUM=0

ngTitle="SPS USRP CH${CHNUM}, 2016/08/11"
ngSubtitle="AVG=${NAvg}, NBIN=${NFFTBins}"

NAVGSKIP=$(( avgsPerTStamp - NAvg ))
NSAMP_SKIP_AFTER_AVG=$(( NAVGSKIP * NFFTBins ))

sFreq=2000000
dtSamp=`echo "1 / ${sFreq}" | bc -l`
T_ADD_AFTER_AVG=`echo "${dtSamp} * ${NFFTBins} * ${avgsPerTStamp}" | bc -l`

tStartString="2016-08-11/00:00:02"

tString="m"

suff='20-28pct'
suff='24-28pct'

file="/SPENCEdata/Research/database/ground-based/Cluster-SPS/2016/southpole5/Ereinion/DataTrans0117/SPSusrp1-ch${CHNUM}-20160811-000002-bw2000000-cf1000000__${suff}.dat"
# outfile="`basename ${file}`"
outfile="SPSusrp1-ch${CHNUM}-20160811-000002-bw2MHz-cf1MHz__${suff}.dat"
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
    
