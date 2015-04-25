#!/bin/sh

# Argument $1 is the binary 
# Argument $2 is the config file 
# Argument $3 is the output file
benchmarkBinary () {
    $1 $2 &
    redispid=$!
    sleep 1s
    warmUpRedis

    echo "Starting benchmark"
    ./src/redis-benchmark -t get -r 10000 -n 10000000 > $3

    echo "Benchmark done. "

    echo "Killing redis with pid $redispid"
    kill $redispid
    sleep 4s
}


warmUpRedis() {
    # Fixes the hash based sampling, the hash value was always the same with the benchmark wtf...
    echo "Initializing keyspace"
    redis-cli set 1 1
}
    

for n_run in $(seq 1 10) 
do
    echo "Benchmarking vanilla redis"
    benchmarkBinary ./redis-vanilla redis-vanilla.conf throughput_vanilla.out-${n_run}

    for p in $(seq 0.0 0.1 1.0) 
    do
        echo "n_run=${n_run} p=$p"
        sed  '/key-sampling-p /d' redis.conf > redis.conf.new
        mv redis.conf.new redis.conf
        sed  '/key-sampling-policy /d' redis.conf > redis.conf.new
        mv redis.conf.new redis.conf
        echo "key-sampling-p $p" >> redis.conf
        echo "key-sampling-policy random" >> redis.conf

        echo "Benchmarking modified redis with random sampling and p=${p}"
        benchmarkBinary ./src/redis-server redis.conf throughput_random_sampling_p${p}.out-${n_run}
        

        sed  '/key-sampling-policy /d' redis.conf > redis.conf.new
        mv redis.conf.new redis.conf
        echo "key-sampling-policy hash" >> redis.conf

        echo "Benchmarking modified redis with hash sampling and p=${p}"
        benchmarkBinary ./src/redis-server redis.conf throughput_hash_sampling_p${p}.out-${n_run}
        # TODO: Fix the hash based sampling, the hash value is always the same with the benchmark wtf...
    done
done
