#!/bin/bash
SCRIPT_LOC=$(readlink -f $BASH_SOURCE)
CHALLENGE_FOLDER=$(readlink -f "$(dirname $SCRIPT_LOC)/..")

cd $CHALLENGE_FOLDER

rm -f fake_base64.txt
rm -f fake_flags.txt

for i in {0..200}; do 
    echo "sun[base64_is_no_fair_$(head -c 4 /dev/urandom | xxd -ps)_wait_how_is_this]" | base64 --wrap=0 >> ./fake_base64.txt
    echo >> ./fake_base64.txt
done
for i in {0..200}; do 
    echo "sun[base64_fair_$(head -c 4 /dev/urandom | xxd -ps)_wait_how_is_this}" | base64 --wrap=0 >> ./fake_base64.txt
    echo >> ./fake_base64.txt
done
for i in {0..200}; do 
    echo "sun{base64_fair_$(head -c 4 /dev/urandom | xxd -ps)_wait_why_is_this]]" | base64 --wrap=0 >> ./fake_base64.txt
    echo >> ./fake_base64.txt
done
for i in {0..200}; do 
    echo "sun   {base64_fair_$(head -c 4 /dev/urandom | xxd -ps)_wait_why_is_this]  ]" | base64 --wrap=0 >> ./fake_base64.txt
    echo >> ./fake_base64.txt
done
for i in {0..200}; do 
    echo " sun { base64_fair_$(head -c 4 /dev/urandom | xxd -ps)_wait_why_is_this]  ]" | base64 --wrap=0 >> ./fake_base64.txt
    echo >> ./fake_base64.txt
done

for i in {0..400}; do 
    echo "sun[base64_is_no_fair_$(head -c 8 /dev/urandom | xxd -ps)_wait_how_is_this]" >> ./fake_flags.txt
done
for i in {0..400}; do 
    echo "sun[base64_fair_$(head -c 6 /dev/urandom | xxd -ps)_wait_how_is_this}" >> ./fake_flags.txt
done
for i in {0..400}; do 
    echo "sun{base64_fair_$(head -c 10 /dev/urandom | xxd -ps)_wait_why_is_this]]" >> ./fake_flags.txt
done
for i in {0..400}; do 
    echo "sun{    base64_fair_$(head -c 10 /dev/urandom | xxd -ps)_wait_why_is_this]   {]" >> ./fake_flags.txt
done
