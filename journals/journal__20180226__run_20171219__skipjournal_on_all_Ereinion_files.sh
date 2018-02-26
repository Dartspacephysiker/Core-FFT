for filNum in $(seq 0 4); do
    for chNum in $(seq 0 1); do
	echo "./journal__20171219__Ereinion_files__skip_samples_instead_of_average2death.sh ${filNum} ${chNum}"
	./journal__20171219__Ereinion_files__skip_samples_instead_of_average2death.sh ${filNum} ${chNum}
    done
done
