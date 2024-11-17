#!/bin/bash

# needs to be outside the scripts
DEST=/mnt/c/ctf_junk/guessy-programmer-1

SCRIPT_LOC=$(readlink -f $BASH_SOURCE)
CHALLENGE_FOLDER=$(readlink -f "$(dirname $SCRIPT_LOC)/..")

rm -rf $DEST
mkdir -p $DEST
cd $DEST

git init .
git config user.email "🕵️@example.com"
git config user.name "🕵️"

ADVENTURE_BOOKS=( "$CHALLENGE_FOLDER/../guessy-programmer-0/books/moby_dick.txt" "$CHALLENGE_FOLDER/../guessy-programmer-0/books/around_the_world_in_80_days.txt" "$CHALLENGE_FOLDER/../guessy-programmer-0/books/kidnapped.txt" )

FLAG="$(cat $CHALLENGE_FOLDER/flag.txt)"

OUTPUT_FILE="adventure_novel_2.txt"

SHOULD_FLAG=0

for book in ${ADVENTURE_BOOKS[@]}; do
    mapfile -t paragraphs <<< $(cat "$book")
    num_paragraphs=${#paragraphs[@]}

    for((j=0;j<num_paragraphs;j++)); do
        paragraph="${paragraphs[$j]}"
        mapfile -t lines <<< $(echo "$paragraph" | tr '%' '\n')
        num_lines="${#lines[@]}"
        if [ $(($RANDOM%2)) -eq 0 ]; then
            mapfile -t fake_flags <<< $(shuf -n "$num_lines" $CHALLENGE_FOLDER/../guessy-programmer-0/fake_flags.txt)
        else
            mapfile -t fake_flags <<< $(shuf -n "$num_lines" $CHALLENGE_FOLDER/../guessy-programmer-0/fake_base64.txt)
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
        if [ $SHOULD_FLAG -eq 1 ]; then
            if [ $j -eq $((num_paragraphs / 4)) ]; then
                echo "$FLAG" >> $OUTPUT_FILE
            fi
        fi

        git add $OUTPUT_FILE
        git commit -m "🕵️" --allow-empty
    done

    SHOULD_FLAG=$((SHOULD_FLAG + 1))
done

# nuke the file
cat /dev/null > $OUTPUT_FILE
git add $OUTPUT_FILE
git commit -m "🕵️" --allow-empty

git bundle create guessy_programmer_1 HEAD

mkdir -p $CHALLENGE_FOLDER/attachments/
mv guessy_programmer_1 $CHALLENGE_FOLDER/attachments/
