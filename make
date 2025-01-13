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
	local first=$(awk 'NR==2{print $2}' "${1}.index")
	local lc=$( wc -l < "${1}.index" )
	local last=$(awk -v ln=$lc 'NR==ln{print $3}' "${1}.index")
	local count=$(( last - first + 1 ))
	dd bs=320 skip=$first count=$count if="${1}.raw" of="${1}.tmp"
}

for file in *.wav
do
	echo "Processing $file ..."
	f=${file%.wav}
	# convert the wav file to raw
	sox "$file" -b 16 -c 1 -r 8000 "${f}.raw"
	# generate the index file so we now how to trim the raw file
	../tools/index -t ${1:-20} "${f}.raw"
	# trim quiet data from the beginning and the end
	truncfile "$f"
	# and move the shorter file over the original
	/bin/mv "${f}.tmp" "${f}.raw"
	# generate the index and this time, the list files
	../tools/index -l -t ${1:-20} "${f}.raw"
	# encode the raw audio to Codec2_3200
	../tools/c2enc 3200 "${f}.raw" "${f}.dat"
done

#/bin/ls -l *.{wav,raw,dat,list,index}
