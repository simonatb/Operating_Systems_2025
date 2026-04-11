#!/bin/bash

pw="$(pwgen 8 1)"

dict=$(mktemp)
files=$(mktemp)

for arg in "${@}"; do
    if echo "${arg}" | grep -Eq "^-R[[:alnum:]]+=[[:alnum:]]+"; then
        echo "${arg}" | sed -E 's/-R([[:alnum:]]+=[[:alnum:]]+)/\1/' >> "${dict}"
    elif echo "${arg}" | grep -Eqv "^-" && [[ -f "${arg}" ]]; then
        echo "${arg}" >> "${files}"
    else
        echo "invalid arg: ${arg}"
        exit 1
    fi
done

while read filename; do
    while read entry; do
        replace="$(echo "${entry}" | cut -d '=' -f 1)"
        replacement="$(echo "${entry}" | cut -d '=' -f 2)"
        sed -iE "/^#/! s/\b${replace}\b/${replacement}${pw}/g" "${filename}"
    done < "${dict}"
    sed -iE "s/${pw}//g" "${filename}"
done < "${files}"

rm "$dict" "$files"
