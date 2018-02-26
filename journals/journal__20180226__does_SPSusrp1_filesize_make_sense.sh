#!/bin/bash
# 2018/02/26
# Time to figure out if the file sizes are correct

NFFTBins=4096			# Number of FFT bins
NAvg=8				# Number of FFTs to average
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
datNum=4
CHNUM=0


DATE=(20160629-004501 20160811-000002 20160820-001005 20160822-012002 20160925-004002)
# printf -v prefStr "%02d-CH0%1d-" $datNum $CHNUM
dateStr="${DATE[datNum]:0:4}-${DATE[datNum]:4:2}-${DATE[datNum]:6:2}"
tStartString="${dateStr}/${DATE[datNum]:9:2}:${DATE[datNum]:11:2}:${DATE[datNum]:13:2}"

ngTitle="SPS USRP CH${CHNUM}, ${dateStr}"
ngSubtitle="AVG=${NAvg}, NBIN=${NFFTBins}"

NAVGSKIP=$(( avgsPerTStamp - NAvg ))
NSAMP_SKIP_AFTER_AVG=$(( NAVGSKIP * NFFTBins ))

sFreq=2000000
dtSamp=`echo "1 / ${sFreq}" | bc -l`
T_ADD_AFTER_AVG=`echo "${dtSamp} * ${NFFTBins} * ${avgsPerTStamp}" | bc -l`
T_ADD_AFTER_AVG=${T_ADD_AFTER_AVG:0:10} #Trim from a million digits to 10 total

for k in $(seq 0 4); do 
datNum=k

if [ "${DATE[datNum]}" = "20160811-000002" ]; then
    dir=/SPENCEdata/Research/database/ground-based/Cluster-SPS/2016/southpole5/Ereinion/DataTrans0117
else
    dir=/thelonious_data2/ground-based/Cluster-SPS/2016/southpole5/Ereinion/DataTrans0117/
fi

file="${dir}/SPSusrp1-ch${CHNUM}-${DATE[datNum]}-bw2000000-cf1000000.dat"
outfile="SPSusrp1-ch${CHNUM}-${DATE[datNum]}-bw2MHz-cf1MHz.dat"
outer="${outfile%%.dat}-${NAvg}avg_${NFFTBins}FFT_skip${NAVGSKIP}avgs.data"
ngdefFile="${outfile%%.dat}-${NAvg}avg_${NFFTBins}FFT_skip${NAVGSKIP}avgs.data.ngdef"
pdfFile="${outfile%%.dat}-${NAvg}avg_${NFFTBins}FFT_skip${NAVGSKIP}avgs.pdf"


filesize=$(ls -la ${file} | cut -f 5 --delimiter=" ")
sampPerByte=2
nSamp=$((filesize/sampPerByte))
nSec=$(bc -l <<< ${nSamp}/${sFreq})
nMin=$(bc -l <<< ${nSamp}/${sFreq}/60)
nHour=$(bc -l <<< ${nSamp}/${sFreq}/3600)
nSec=${nSec:0:12}
nMin=${nMin:0:10}
nHour=${nHour:0:6}
printf "Filename : %s\n" $(basename ${file})
printf "Dir      : %s\n" $(dirname  ${file})
printf "Filesize : %d (%.03f MB, %.03f GB)\n" ${filesize} $(bc -l <<< ${filesize}/1024/1024) $(bc -l <<< ${filesize}/1024/1024/1024)
printf "nSamp    : %d\n" ${nSamp}
printf "sFreq    : %d\n" ${sFreq}
printf "nSec     : %.03f\n" ${nSec}
printf "nMin     : %.03f\n" ${nMin}
printf "nHour    : %.03f\n" ${nHour}
printf "\n"
done
