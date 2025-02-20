#!/bin/bash

if [[ $# -eq 0 ]]; then
    echo "Usage:"
    echo "   $0 <directory/to/test/files>"
    exit -1
fi

test_dir=$1

separator() {
    echo ""
    echo "---------------------------------------------------------------------"
    echo ""
}

testError() {
    if [ $? -ne 0 ]; then
        echo ""
        echo "---------------------------------------------------------------------"
        echo "--- Test failed. Stopped!"
        echo "---------------------------------------------------------------------"
        exit $?
    fi
}

doPacking() {
    echo "---------------------------------------------------------------------"
    echo "--- Start packing"
    echo "---------------------------------------------------------------------"
    echo ""
    find $test_dir -type d -maxdepth 1 -not -path $test_dir -print0 | while read -d $'\0' i; do
        echo "---------------------------------------------------------------------"
        echo "--- Packing: '$i'"
        echo "---------------------------------------------------------------------"

        if [ -f ./texpacker-old ]; then
            ./texpacker-old $@ "$i" -o "$i-old.png"
            testError
            separator
        fi

        if [ -f ./texpacker ]; then
            ./texpacker $@ "$i" -o "$i.png"
            testError
            separator
        fi

        echo ""
        echo ""
        echo ""
    done
}

doTests() {
    NC='\033[0m'
    RED='\033[0;31m'
    GREEN='\033[0;32m'

    px_diff="0"
    files_total=0
    files_better=0
    files_worse=0

    echo "---------------------------------------------------------------------"
    echo "--- Compare results"
    echo "---------------------------------------------------------------------"
    echo ""
    find $test_dir -type d -maxdepth 1 -not -path $test_dir -print0 | while read -d $'\0' i; do
        echo "---------------------------------------------------------------------"

        dim_old=$(magick identify -format "%w * %h" "$i-old.png")
        echo " [$dim_old]: '$i-old.png'"

        dim_new=$(magick identify -format "%w * %h" "$i.png")
        echo " [$dim_new]: '$i.png'"

        diff=$(echo "$dim_new - $dim_old" | bc)
        px_diff="${px_diff} + $diff"

        # echo "100 * ($dim_new) / ($dim_old) - 100"
        percent=$(echo "100 * ($dim_new) / ($dim_old) - 100" | bc -l)

        files_total=$((files_total + 1))

        if [ $diff == 0 ]; then
            echo "Unchanged"
        elif [ $diff -gt 0 ]; then
            files_worse=$((files_worse + 1))
            printf "Worse: ${RED}+%'d${NC} (${RED}+%.1f%%${NC})\n" $diff $percent
        else
            files_better=$((files_better + 1))
            printf "Better: ${GREEN}%'d${NC} (${GREEN}%.1f%%${NC})\n" $diff $percent
        fi

        echo ""
    done

    echo -e "Out of a total of $files_total files, ${GREEN}$files_better${NC} packed better + $(($files_total - $files_worse - $files_better)) unchanged, ${RED}$files_worse${NC} packed worse."

    # echo "'$px_diff'"
    px_diff=$(echo "$px_diff" | bc)
    str="The total pixel difference across all files is"
    if [ $px_diff == 0 ]; then
        echo "$str no difference"
    elif [ $px_diff -gt 0 ]; then
        printf "$str ${RED}+%'d${NC}.\n" $px_diff
    else
        printf "$str ${GREEN}%'d${NC}.\n" $px_diff
    fi
}

doPacking ${@:2}

if [ -f ./texpacker-old -a -f ./texpacker ]; then
    doTests
fi
