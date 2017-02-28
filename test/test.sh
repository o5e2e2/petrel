#!/bin/bash

eol=$'\n'
expect="set timeout -1$eol"
expect+="spawn ../release/petrel$eol"

filename='petrel.rc'
exec 4<$filename
while read -r -u4 line || [[ -n $line ]];
do
    if [[ $line != *[^[:space:]]* ]]
    then
        continue
    fi

    if [[ $line != \;* ]]
    then
        expect+="send \"$line\r\"$eol"
    else
        expect+="expect \"${line:1}\r\n\"$eol"
    fi
done
expect+="expect eof$eol"
expect+="puts \"TEST PASSED\"$eol"

make -C ../src clean && make -C ../src
echo "$expect" | expect -
