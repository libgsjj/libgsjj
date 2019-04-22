#!/bin/bash

cwd=$(pwd)
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

# The number of generations by size
maxGeneration=100

for size in $(seq 9 10)
do
    outputFile="${cwd}/random-${size}"
    # To clear the output file
    printf "" > ${outputFile}

    for generation in $(seq 1 ${maxGeneration})
    do
        echo "Size ${size}, generation ${generation}/${maxGeneration}"

        # We first generate a sample
        $(${program} --generate-sample --output-file /tmp/out${size} --number-words ${size} --min-word-size 6 --max-word-size 14)

        # We test if the generation went fine or not
        if [ $? -eq 0 ] 
        then
            # Generation of the sample went fine
            # We can benchmark every method on this sample
            for method in ${methods[@]}
            do
                echo $method
                cmd="${program} --benchmarks --input-file /tmp/out${size} ${method}"
                # We will use the time given by our program when it's possible
                # But when the program crash (due to an out of memory error, for example), we can't retrieve the time taken so far. So, in this case, we will use date to estimate the taken time
                start=$(date +%s.%N)
                # We create a subshell to disable the errors printed by Bash (due to an aborted thread)
                $($cmd > /tmp/times${size} 2> /tmp/error${size})
                status=$?
                end=$(date +%s.%N)
                if [ $status -eq 0 ]
                then
                    t=$(cat /tmp/times${size})
                    success=0
                else
                    t=$(echo "$end - $start" | bc -l)
                    success=1
                fi
                printf "%f\t%d\t" $t $success >> $outputFile
            done
            printf "\n" >> ${outputFile}
        else
            # Generation of the sample failed
            echo "Could not generate the sample"
            exit 1
        fi
    done
done
