#!/bin/bash

if [[ $# < 2 ]]
then

    cat << EOF
Usage:
$0 [engine] [script]
EOF

exit
fi

engine=$1
script=$2
eol=$'\n'

expect="set timeout -1$eol"
expect+="spawn $engine$eol"

exec 4<$script
while read -r -u4 line || [[ -n $line ]];
do
    #skip blank lines
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

echo "$expect" | expect -
