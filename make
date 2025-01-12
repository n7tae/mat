#/bin/bash
################################################################
#                                                              #
#                     M17 Audio Tools                          #
#                                                              #
#         Copyright (c) 2024 by Thomas A. Early N7TAE          #
#                                                              #
# See the LICENSE file for details about the software license. #
#                                                              #
################################################################

truncfile () {
	first=$(awk 'NR==2{print $2}' "${1}.index")
	lc=$( wc -l < "${1}.index" )
	last=$(awk -v ln=$lc 'NR==ln{print $3}' "${1}.index")
	count=$(( last - first + 1 ))
	dd bs=8 skip=$first count=$count if="${1}.dat" of="${1}.tmp"
}

for f in *.wav
do
	echo "Processing $f ..."
	sox "$f" -b 16 -c 1 -r 8000 "${f%.wav}.raw"
	../tools/index -l -t ${1:-20} "${f%.wav}.raw"
	../tools/c2enc 3200 "${f%.wav}.raw" "${f%.wav}.dat"
	truncfile "${f%.wav}"
	/bin/mv "${f%.wav}.tmp" "${f%.wav}.dat"
done

/bin/ls -l *.{wav,raw,dat,list,index}
