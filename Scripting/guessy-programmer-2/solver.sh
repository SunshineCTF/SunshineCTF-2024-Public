#!/bin/bash

# How to use this solver?
# copy this solver to outside the repo
# copy all the guessy_programmer bundle repo files (_0, _1, etc) into the same directory as the script
# run the script
# it should print out the flags for all bundles passed (#2 lags a bit, but eventually prints it out)

FLAG_REGEX='sun{.*}'

BUNDLE_FILES=$(readlink -f guessy_programmer_*)

# clean up the checkout_dir first in case we have it preset
rm -rf checkout_dir
mkdir checkout_dir
cd checkout_dir
current_dir=$PWD

# loop through each bundle, check it out, and start searching for files in the history
for i in $BUNDLE_FILES; do
    cd $current_dir
    git clone $i >/dev/null
    cd $(basename $i)
    # turn the messages about detached heads off to clean up output.
    git config advice.detachedHead false

    while true; do
        # we found the flag directly -> terminate search early
        found_flag=$(strings * | grep -Ee $FLAG_REGEX)
        if [[ -n $found_flag ]]; then
            echo "found_flag for $i! $found_flag"
            break;
        fi
        # check for base64 type strings (c3Vu = sun{)
        # without the c3Vu check it flags a LOT of english words as base64.
        found_base64=($(strings * | grep -Ee 'c3Vu.*' -o | grep -P '((?<![A-Za-z0-9+\/])(([A-Za-z0-9+\/]{4})*)([A-Za-z0-9+\/]{4}|[A-Za-z0-9+\/]{3}=|[A-Za-z0-9+\/]{2}==)(?![=A-Za-z0-9+\/]))' -o))
        if [[ -n $found_base64 ]]; then
            for base_64 in ${found_base64[@]}; do
                found_flag=$(echo "$base_64" | base64 --decode | grep -Ee $FLAG_REGEX)
                if [[ -n $found_flag ]]; then
                    echo "found base64 flag for $i! $found_flag"
                    break;
                fi
            done
        fi
        # we haven't found base64 nor the flag, so revert the repo back one step
        git reset --hard HEAD~1 >/dev/null
        if [ $? -ne 0 ]; then
            break;
        fi
    done
done
