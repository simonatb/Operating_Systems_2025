#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "need two args"
    exit 1
fi

if [[ ! -f "${1}" ]]; then
    echo "first arg should be file"
    exit 2
fi

if ! echo "${2}" | grep -qE "^(0|[1-9][0-9]*)$"; then
    echo "second arg should be number"
    exit 3
fi

K="${2}"
FILE="${1}"
SHIFT=$(( k % 26 ))
ALPHABET="ABCDEFGHIJKLMNOPQRSTUVWXYZ"

new_alph=$(echo "${ALPHABET:${SHIFT}}${ALPHABET:0:${SHIFT}}")

cat "$FILE" | tr "$ALPHABET" "$new_alph" > "$file.tmp"
mv "$file.tmp" "$FILE"

count=0
dictionary="/usr/share/dict/words"

if [[ ! -f "$dictionary" ]]; then
    echo "dictionary not file existing"
    exit 4
fi

for word in $(cat "$FILE" | tr -d '[:punct:]' | tr ' ' '\n' | sort | uniq ); do
    if [[ -n "$word" ]]; then

       lower="$(echo "$word" | tr '[:upper:]' '[:lower:]')"

      if grep -qx "$lower" "$dictionary" 2>/dev/null; then
            ((count++))
            echo "word found: $lower"
       fi
    fi
done

echo "words found in the dictionary $count"
