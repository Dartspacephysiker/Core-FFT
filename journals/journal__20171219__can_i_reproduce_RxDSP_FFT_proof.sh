# Run from /usr/src/Core-FFT/
NFFTBins=4096			# Number of FFT bins
NAvg=2				# Number of FFTs to average
# topFreq=1541000			# in Hz
# topFreq=666000
maxFreq=666000
minFreq=333000
ngTitle='RxDSP Master (signals at 500 and 510 kHz)'
ngSubtitle="AVG=${NAvg}, NBIN=${NFFTBins}"

file='/SPENCEdata/Research/database/CAPER/test_Spence_08142017_RxDSP_fscc_acq_510kHz_w_500kHz-20170814-222327-p00.data' 
outfile="`basename ${file}`"
outer="${outfile%%.data}--${NAvg}avg_${NFFTBins}FFT.data"
ngdefFile="${outfile%%.data}--${NAvg}avg_${NFFTBins}FFT.data.ngdef"
pdfFile="${outfile%%.data}--${NAvg}avg_${NFFTBins}FFT.pdf"

saveDir=/usr/src/Core-FFT/saves_output_etc
plotDir=/usr/src/Core-FFT/plots

../core-fft.py -C -N ${NFFTBins} --header=2 --max-freq=${maxFreq} --min-freq=${minFreq} -a ${NAvg} --ngtitle="${ngTitle}" --ngsubtitle="${ngSubtitle}" --ngdef ${file} ${saveDir}/${outer} && \
    ../convert_core-fft_output_to_ps.sh --input ${saveDir}/${outer} -d ${saveDir}/${ngdefFile}
    
    ps2pdf gray.ps ${plotDir}/${pdfFile}
