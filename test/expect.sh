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

ex=""

exec 4<$script
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

    #collect expect lines together
    if [[ $line = \;* ]]
    then
        ex+="${line:1}\r\n"
        continue
    fi

    if [[ -n $ex ]]
    then
        expect+="expect \"$ex\"$eol"
        ex=""
    fi

    expect+="send \"$line\r\"$eol"
done

if [[ -n $ex ]]
then
    expect+="expect \"$ex\"$eol"
    ex=""
fi

expect+="expect eof$eol"
expect+="puts \"TEST PASSED\"$eol"

echo "$expect" | expect -
