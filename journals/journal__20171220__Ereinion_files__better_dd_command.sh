# 2017/12/20
# Third try 

# There is clearly some action between 5284 and 6134 s, and possibly as early as 4404 s.
#         The length of the file is 22189.97 s (assuming 2 MHz sampling).
#         Thus, I'm looking for the chunks that are 4404/22189.97*100 = 19.84% and 6134/22189.97*100 = 27.64%.
#         These are bytes 17631941807.3088–24563854413.0048
#         These are bytes 17631941792–24563854400
# bc action

size=88870674432 		# ch0
size=88870666240		# ch 1
start=4404
start=5284
stop=6134
totlen=22217.668608

doBC=1

if [ "${doBC}" = "1" ]; then
    startStopPct=($(echo "size=${size}
    start=${start}
    stop=${stop}
    totlen=${totlen}
    start/totlen*100
    stop/totlen*100
    " | bc -l | tr [:blank:] "\n"))

    startStopByte=($(echo "size=${size}
    start=${start}
    stop=${stop}
    totlen=${totlen}
    start/totlen*size
    stop/totlen*size
    " | bc -l))

    i=0
    for this in "${startStopPct[@]}"
    do
	# echo ${this}
	startStopRnd[${i}]=$(echo "($this+0.5)/1" | bc)
	# echo ${startStopRnd[${i}]}
	i=$((i+1))
    done

    startStopByte[0]=$(echo "${startStopByte[0]}/2*2" | bc)
    startStopByte[1]=$(echo "${startStopByte[1]}/2*2" | bc)

    echo "start percentage/byte:"
    printf '%.5f / %.1f\n' ${startStopPct[0]} ${startStopByte[0]}
    echo ""
    echo "stop percentage/byte:"
    printf '%.5f / %.1f\n' ${startStopPct[1]} ${startStopByte[1]}
fi

bs=1024

echo "######################
# Determine start count for bs=${bs}:"
startCount=$(echo "
${startStopByte[0]}/${bs}/${bs}
" | bc -l)
echo $startCount

echo "
# round up"
startCountRound=$(echo "(${startStopByte[0]}+0.5)/${bs}/${bs}
" | bc)
startByteRound=$(echo "(${startStopByte[0]}+0.5)/${bs}/${bs}*${bs}*${bs}
" | bc)
echo "$startCountRound * ${bs} * ${bs} = ${startByteRound}"
echo ""

echo "######################
# Determine stop count for bs=${bs}:"
stopCount=$(echo "
${startStopByte[1]}/${bs}/${bs}
" | bc -l)
echo $stopCount

echo "
# round up"
stopCountRound=$(echo "(${startStopByte[1]}+0.5)/${bs}/${bs}
" | bc)
stopByteRound=$(echo "(${startStopByte[1]}+0.5)/${bs}/${bs}*${bs}*${bs}
" | bc)
echo "$stopCountRound * ${bs} * ${bs} = ${stopByteRound}"


# 
# nsamp=size/2
# Try this:
# 
CHNUM=1

binDir="/SPENCEdata/Research/database/ground-based/Cluster-SPS/2016/southpole5/Ereinion/DataTrans0117"
binFile="SPSusrp1-ch${CHNUM}-20160811-000002-bw2000000-cf1000000.dat"
# binOut="SPSusrp1-ch${CHNUM}-20160811-000002-bw2000000-cf1000000__20-100pct.dat"
binFinal="SPSusrp1-ch${CHNUM}-20160811-000002-bw2000000-cf1000000__${startStopRnd[0]}-${startStopRnd[1]}pct.dat"

echo ""
echo $binFinal

# This time we only read in such and such an amount
echo "dd if=${binDir}/${binFile} bs=${bs}k count=${stopCountRound} | { dd bs=${bs}k count=${startCountRound} of=/dev/null; dd bs=1024k of=${binDir}/${binFinal}; } "
# dd if=${binDir}/${binOut} bs=512k | { dd bs=512k count=13222 of=${binDir}/${binFinal}; dd bs=512k of=/dev/null; } 
