#!/bin/sh

# Usage:
#     ./run_simulations.sh [config1 config2 ...]

build=true
bin="../src/CT"
ini_file="omnetpp.ini"
available_configs=$(grep -oP '(?<=\[Config ).*?(?=\])' "$ini_file" | tr -d ' ')

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
    for (( i=0; i < $num_runs; i++ ))
    do
        "$bin" -r $i -u Cmdenv -c "$config" -n ./:../src --debug-on-errors=false "$ini_file"
    done
done

# Aggregate the results
cd results
for config in "${configs[@]}"
do
    scavetool x "$config"*.vec -o control_tower_"$config".csv
    rm "$config"*.vec "$config"*.vci
done
