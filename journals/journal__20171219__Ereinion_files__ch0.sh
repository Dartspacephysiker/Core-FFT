# Run from /usr/src/Core-FFT/
NFFTBins=4096			# Number of FFT bins
NAvg=1024				# Number of FFTs to average
# topFreq=1541000			# in Hz
# topFreq=666000
maxFreq=1000000
minFreq=0
ngTitle='SPS USRP CH0, 2016/08/11'
ngSubtitle="AVG=${NAvg}, NBIN=${NFFTBins}"

file='/SPENCEdata/Research/database/ground-based/Cluster-SPS/2016/southpole5/Ereinion/DataTrans0117/SPSusrp1-ch0-20160811-000002-bw2000000-cf1000000.dat' 
# outfile="`basename ${file}`"
outfile="SPSusrp1-ch0-20160811-000002-bw2MHz-cf1MHz.dat"
outer="${outfile%%.dat}-${NAvg}avg_${NFFTBins}FFT.data"
ngdefFile="${outfile%%.dat}-${NAvg}avg_${NFFTBins}FFT.data.ngdef"
pdfFile="${outfile%%.dat}-${NAvg}avg_${NFFTBins}FFT.pdf"

saveDir=/usr/src/Core-FFT/saves_output_etc
plotDir=/usr/src/Core-FFT/plots

../core-fft.py -N ${NFFTBins} --max-freq=${maxFreq} --min-freq=${minFreq} -a ${NAvg} --ngtitle="${ngTitle}" --ngsubtitle="${ngSubtitle}" --ngdef ${file} ${saveDir}/${outer} && \
    ../convert_core-fft_output_to_ps.sh --input ${saveDir}/${outer} -d ${saveDir}/${ngdefFile} --outdir ${plotDir}
    
    # ps2pdf gray.ps ${plotDir}/${pdfFile}
