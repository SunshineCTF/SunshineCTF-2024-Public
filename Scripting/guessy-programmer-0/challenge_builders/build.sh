#!/bin/bash

# needs to be outside the scripts
DEST=/mnt/c/ctf_junk/guessy-programmer-0

SCRIPT_LOC=$(readlink -f $BASH_SOURCE)
CHALLENGE_FOLDER=$(readlink -f "$(dirname $SCRIPT_LOC)/..")

rm -rf $DEST
mkdir -p $DEST
cd $DEST

git init .
git config user.email "🕵️@example.com"
git config user.name "🕵️"

ADVENTURE_BOOKS=( "$CHALLENGE_FOLDER/books/around_the_world_in_80_days.txt" "$CHALLENGE_FOLDER/books/kidnapped.txt" )

FLAG="$(cat $CHALLENGE_FOLDER/flag.txt)"

OUTPUT_FILE="adventure_novel.txt"

for book in ${ADVENTURE_BOOKS[@]}; do
    mapfile -t paragraphs <<< $(cat "$book")
    num_paragraphs=${#paragraphs[@]}

    for((j=0;j<num_paragraphs;j++)); do
        paragraph="${paragraphs[$j]}"
        mapfile -t lines <<< $(echo "$paragraph" | tr '%' '\n')
        num_lines="${#lines[@]}"
        if [ $(($RANDOM%2)) -eq 0 ]; then
            mapfile -t fake_flags <<< $(shuf -n "$num_lines" $CHALLENGE_FOLDER/fake_flags.txt)
        else
            mapfile -t fake_flags <<< $(shuf -n "$num_lines" $CHALLENGE_FOLDER/fake_base64.txt)
        fi

        echo "$j / $num_paragraphs ${lines[0]}"

        # re/create the file
        cat /dev/null > $OUTPUT_FILE
        for((i=0; i<num_lines; i++)); do
            if [ $(($RANDOM%2)) -eq 0 ]; then
                echo -n "${fake_flags[$i]} " >> $OUTPUT_FILE
                echo "${lines[$i]}" >> $OUTPUT_FILE
            else
                echo -n "${lines[$i]} " >> $OUTPUT_FILE
                echo "${fake_flags[$i]}" >> $OUTPUT_FILE
            fi
        done
        git add $OUTPUT_FILE
        git commit -m "🕵️" --allow-empty
    done
done

# recreate the file
cat /dev/null > $OUTPUT_FILE
# add the flag now, and just sorta mess the file up each time
mapfile -t paragraphs <<< $(cat "$CHALLENGE_FOLDER/books/moby_dick.txt")
num_paragraphs=${#paragraphs[@]}

real_flag_add=$((num_paragraphs / 3))
for((j=0;j<num_paragraphs;j++)); do
    paragraph="${paragraphs[$j]}"
    mapfile -t lines <<< $(echo "$paragraph" | tr '%' '\n')
    num_lines="${#lines[@]}"
    if [ $(($RANDOM%2)) -eq 0 ]; then
        mapfile -t fake_flags <<< $(shuf -n "$num_lines" $CHALLENGE_FOLDER/fake_flags.txt)
    else
        mapfile -t fake_flags <<< $(shuf -n "$num_lines" $CHALLENGE_FOLDER/fake_base64.txt)
    fi

    echo "$j / $num_paragraphs ${lines[0]}"
    for((i=0; i<num_lines; i++)); do
        if [ $(($RANDOM%2)) -eq 0 ]; then
            echo -n "${fake_flags[$i]} " >> $OUTPUT_FILE
            echo "${lines[$i]}" >> $OUTPUT_FILE
        else
            echo -n "${lines[$i]} " >> $OUTPUT_FILE
            echo "${fake_flags[$i]}" >> $OUTPUT_FILE
        fi
    done
    if [ $real_flag_add -eq $j ]; then
        echo "$FLAG" >> $OUTPUT_FILE
    fi
done
git add $OUTPUT_FILE
git commit -m "🕵️" --allow-empty

git bundle create guessy_programmer_0 HEAD

mkdir -p $CHALLENGE_FOLDER/attachments/
mv guessy_programmer_0 $CHALLENGE_FOLDER/attachments/
