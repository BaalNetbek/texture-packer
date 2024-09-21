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

for i in $(find $test_dir -type d -maxdepth 1 -not -path $test_dir); do
    echo "---------------------------------------------------------------------"
    echo "--- Start testing: $i"
    echo "---------------------------------------------------------------------"
    echo ""
    if [ -f ./packimages-old ]; then
        ./packimages-old ${@:2} $i -o $i-old.png
        testError
        separator
    fi

    if [ -f ./packimages ]; then
        ./packimages ${@:2} $i -o $i.png
        testError
        separator
    fi

    echo ""
    echo ""
    echo ""
done
