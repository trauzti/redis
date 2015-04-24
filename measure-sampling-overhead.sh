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
# TODO: Increase the number of requests
#    ./src/redis-benchmark -t get -r 10000 -n 1000000 > $3
     ./src/redis-benchmark -t get -r 10000 -n 500000 > $3
    echo "Benchmark done. "

    echo "Killing redis with pid $redispid"
    kill $redispid
    sleep 4s
}


warmUpRedis() {
    echo "Warming up Redis"
    for i in {1..10} 
    do
        redis-cli set $i $i
    done
    for i in {1..10} 
    do
        redis-cli get $i
    done
}
    
echo "Benchmarking vanilla redis"
benchmarkBinary ./redis-vanilla redis-vanilla.conf throughput_vanilla.out

ps="0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0"
for p in $ps
do
    echo "Using p=$p"
    sed  '/key-sampling-p /d' redis.conf > redis.conf.new
    mv redis.conf.new redis.conf
    sed  '/key-sampling-policy /d' redis.conf > redis.conf.new
    mv redis.conf.new redis.conf
    echo "key-sampling-p $p" >> redis.conf
    echo "key-sampling-policy random" >> redis.conf

    echo "Benchmarking modified redis with random sampling and p=${p}"
    benchmarkBinary ./src/redis-server redis.conf throughput_random_sampling_p${p}.out
    

    sed  '/key-sampling-policy /d' redis.conf > redis.conf.new
    mv redis.conf.new redis.conf
    echo "key-sampling-policy hash" >> redis.conf

    echo "Benchmarking modified redis with hash sampling and p=${p}"
    benchmarkBinary ./src/redis-server redis.conf throughput_hash_sampling_p${p}.out
    # TODO: Fix the hash based sampling, the hash value is always the same with the benchmark wtf...

done
