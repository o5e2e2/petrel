#!/bin/bash

if [[ $# < 2 ]]
then

    cat << EOF
Usage:
$0 [executable_file] [script_file]
EOF

exit
fi

target=$1
resource=$2

eol=$'\n'
expect="set timeout -1$eol"
expect+="spawn $1$eol"

exec 4<$resource
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

echo "$expect" | expect -
