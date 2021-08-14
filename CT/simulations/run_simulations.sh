#!/bin/sh

ini_file="omnetpp.ini"
num_runs=$(awk -F "=" '/repeat/ {print $2}' "$ini_file" | tr -d ' ')
configs=($(grep -oP '(?<=\[Config ).*?(?=\])' "$ini_file" | tr -d ' '))

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
