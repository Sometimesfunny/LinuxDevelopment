#!/bin/bash

delay="$1"
if [ -z "$delay" ]; then
    delay=0
fi

tput clear

positions=""
line_number=0
while IFS= read -r line; do
    len=${#line}
    for ((i = 1; i <= len; i++)); do
        char="${line:i-1:1}"
        if [[ -n "$char" && "$char" != ' ' ]]; then
            positions+="$line_number"$'\t'"$((i + 1))"$'\t'"$char"$'\n'
        fi
    done
    line_number=$((line_number + 1))
done

total_lines=$line_number

positions=$(echo "$positions" | shuf)

while IFS=$'\t' read -r line_number column_number char; do
    tput cup "$line_number" "$column_number"
    printf '%s' "$char"
    sleep "$delay"
done <<< "$positions"

tput cup "$total_lines" 0
