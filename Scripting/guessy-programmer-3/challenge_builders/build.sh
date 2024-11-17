#!/bin/bash

# needs to be outside the scripts
DEST=/mnt/c/ctf_junk/guessy-programmer-3

SCRIPT_LOC=$(readlink -f $BASH_SOURCE)
CHALLENGE_FOLDER=$(readlink -f "$(dirname $SCRIPT_LOC)/..")

# generate the fake flags and fake base64 text
$CHALLENGE_FOLDER/../guessy-programmer-2/challenge_builders/new_base64.sh

rm -rf $DEST
mkdir -p $DEST
cd $DEST

git init .
git config user.email "🕵️@example.com"
git config user.name "🕵️"

cat > .gitattributes <<GIT_ATTRIBUTES_HERE_DOC
# git keeps trying to add .gif files as text! idk why, it's like it sees them as text.
## anyway nice catch, here's a flag:
## sun{nice_flag_nice_catch_o_wait_this_is_not_a_valid_flag_huh_must_be_in_the_gifs_after_all]
*.gif binary
GIT_ATTRIBUTES_HERE_DOC

git add .gitattributes
git commit -m "🕵️" --allow-empty

ADVENTURE_BOOKS=( "$CHALLENGE_FOLDER/../guessy-programmer-0/books/around_the_world_in_80_days.txt" "$CHALLENGE_FOLDER/../guessy-programmer-0/books/moby_dick.txt" "$CHALLENGE_FOLDER/../guessy-programmer-0/books/kidnapped.txt" )

FLAG="$(cat $CHALLENGE_FOLDER/flag.txt)"

OUTPUT_FILE="adventure_novel_3.gif"

SHOULD_FLAG=0

for book in ${ADVENTURE_BOOKS[@]}; do
    mapfile -t paragraphs <<< $(cat "$book")
    num_paragraphs=${#paragraphs[@]}

    for((j=0;j<num_paragraphs;j++)); do
        paragraph="${paragraphs[$j]}"
        mapfile -t lines <<< $(echo "$paragraph" | tr '%' '\n')
        num_lines="${#lines[@]}"
        if [ $(($RANDOM%2)) -eq 0 ]; then
            mapfile -t fake_flags <<< $(shuf -n "$num_lines" $CHALLENGE_FOLDER/../guessy-programmer-2/fake_flags.txt)
        else
            mapfile -t fake_flags <<< $(shuf -n "$num_lines" $CHALLENGE_FOLDER/../guessy-programmer-2/fake_base64.txt)
        fi

        echo "$j / $num_paragraphs ${lines[0]}"

        # re/create the file -> add junk so it flags as a gif file
        printf 'GIF87a' > $OUTPUT_FILE
        printf 'GIF89a' >> $OUTPUT_FILE

        for((i=0; i<num_lines; i++)); do
            if [ $(($RANDOM%2)) -eq 0 ]; then
                echo -n "${fake_flags[$i]} " >> $OUTPUT_FILE
                echo "${lines[$i]}" >> $OUTPUT_FILE
            else
                echo -n "${lines[$i]} " >> $OUTPUT_FILE
                echo "${fake_flags[$i]}" >> $OUTPUT_FILE
            fi
        done
        if [ $SHOULD_FLAG -eq 0 ]; then
            if [ $j -eq $(((num_paragraphs * 2) / 4)) ]; then
                echo "$FLAG" | base64 --wrap=0 >> $OUTPUT_FILE
            fi
        fi

        git add $OUTPUT_FILE
        git commit -m "🕵️" --allow-empty
    done

    SHOULD_FLAG=$((SHOULD_FLAG + 1))
done

# nuke the file
rm $OUTPUT_FILE
git add $OUTPUT_FILE
git commit -m "🕵️" --allow-empty

git bundle create guessy_programmer_3 HEAD

mkdir -p $CHALLENGE_FOLDER/attachments/
mv guessy_programmer_3 $CHALLENGE_FOLDER/attachments/
