#!/bin/bash

if [[ $# < 2 ]]
then

    cat << EOF
Usage:
$0 [engine] [scenario]
EOF

exit
fi

engine=$1
scenario=$2
eol=$'\n'

script="set timeout -1$eol"
script+="spawn $engine$eol"

expect=""
send=""

exec 4<$scenario
while read -r -u4 line || [[ -n $line ]];
do
    #skip blank lines
    if [[ $line != *[^[:space:]]* ]]
    then
        continue
    fi

    #skip comment lines
    if [[ $line = \#* ]]
    then
        continue
    fi

    #collect EXPECT lines together
    if [[ $line = \<* ]]
    then
        if [[ -n $send ]]
        then
            script+="send \"$send\"$eol"
            send=""
        fi
        expect+="${line:1}\r\n"
        continue
    fi

    #collect SEND lines together

    if [[ -n $expect ]]
    then
        script+="expect \"$expect\"$eol"
        expect=""
    fi

    send+="$line\r"
done

if [[ -n $expect ]]
then
    script+="expect \"$expect\"$eol"
fi

if [[ -n $send ]]
then
    script+="send \"$send\"$eol"
fi

script+="expect eof$eol"
script+="puts \"TEST PASSED\"$eol"

echo "$script" | expect -
