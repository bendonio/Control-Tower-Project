#!/bin/sh

# Usage:
#     ./run_simulations.sh [config1 config2 ...]

build=true
bin="../src/CT"
ini_file="omnetpp.ini"
available_configs=$(grep -oP '(?<=\[Config ).*?(?=\])' "$ini_file" | tr -d ' ')
max_runs_batch=180

# Build
if [ $build ]
then
    cd ../src
    make
    cd -
fi

# Check for specific configurations to run
if [ "$#" -eq 0 ]
then
    configs=($available_configs)
else
    configs=($(echo ${available_configs[@]} $@ | sed 's/ /\n/g' | sort | uniq -d))
fi

# Run the simulations
for config in "${configs[@]}"
do
    num_runs=$("$bin" -u Cmdenv -c "$config" -q runs | grep -oP '(?<=Number of runs: ).[0-9]+')
    iters=$((num_runs/max_runs_batch+1))
    for (( i=0; i < $iters; i++ ))
    do
        for (( j=$i * $max_runs_batch; j < ($i + 1) * $max_runs_batch; j++ ))
        do
            if [ $j -eq $num_runs ]
            then
                break
            fi
            "$bin" -r $j -u Cmdenv -c "$config" -n ./:../src --debug-on-errors=false "$ini_file"
        done

        # Aggregate the results
        cd results
        scavetool x "$config"*.vec "$config"*.sca -o control_tower_"$config"_"$i".csv
        rm "$config"*.vec "$config"*.vci "$config"*.sca
        cd -
    done
done
