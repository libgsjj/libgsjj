#!/bin/bash

cwd=$(pwd)
directory="${cwd}/files"
cd "../build/benchmarks"
make

methods=(\
    "biermann"\
    "neider"\
    "unary"\
    "binary"\
    "heule"\
    "unaryNonCNF"\
    "binaryNonCNF"\
    "heuleNonCNF"\
)
program="./benchmarks"


for size in {04..21}
do
    outputFile=${cwd}/loop-free-${size}
    printf "" > ${outputFile}

    for file in ${directory}/randm${size}.*.kis
    do
        echo $size $file
        for method in ${methods[@]}
        do
            echo $method
            cmd="${program} --benchmarks --input-file ${file} --loop-free ${method}"
            # We will use the time given by our program when it's possible
            # But when the program crash (due to an out of memory error, for example), we can't retrieve the time taken so far. So, in this case, we will use date to estimate the taken time
            start=$(date +%s.%N)
            # We create a subshell to disable the errors printed by Bash (due to an aborted thread)
            $($cmd > /tmp/times 2> /tmp/error)
            status=$?
            end=$(date +%s.%N)
            if [ $status -eq 0 ]
            then
                t=$(cat /tmp/times)
                success=0
            else
                t=$(echo "$end - $start" | bc -l)
                success=1
            fi
            printf "%f\t%d\t" $t $success >> $outputFile
        done
        printf "\n" >> ${outputFile}
    done
done