#!/bin/sh

# Usage:
#     ./run_simulations.sh [config1 config2 ...]

ini_file="omnetpp.ini"
num_runs=$(awk -F "=" '/repeat/ {print $2}' "$ini_file" | tr -d ' ')
available_configs=$(grep -oP '(?<=\[Config ).*?(?=\])' "$ini_file" | tr -d ' ')

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
    for (( i=0; i < $num_runs; i++ ))
    do
        ../src/CT -r $i -u Cmdenv -c "$config" -n ./:../src --debug-on-errors=false "$ini_file"
    done
done

# Aggregate the results
cd results
for config in "${configs[@]}"
do
    scavetool x "$config"*.vec -o control_tower_"$config".csv
done
