#!/bin/bash 
#Simple shell script to unpack the update files.

BASE_DIR="/home/timeserver"
UPDATE_GPG="$BASE_DIR/update.tar.gz.gpg"
UPDATE_TAR="$BASE_DIR/update.tar"
UPDATE_TAR_GZ="$BASE_DIR/update.tar.gz"                                                           
CALCULATED_HASH="$BASE_DIR/calc.hash"
UPDATE_HASH="$BASE_DIR/update.hash"
UPDATE_SCRIPT="$BASE_DIR/UpdateScript.sh"

if [ -f "$UPDATE_GPG" ]; then
    echo "$UPDATE_GPG does exist."
    gpg --output "$UPDATE_TAR_GZ" --decrypt "$UPDATE_GPG" 
    if [ -f "$UPDATE_TAR_GZ" ]; then
        
        echo "$UPDATE_TAR_GZ does exist."
        tar -xzvf "$UPDATE_TAR_GZ" -C "$BASE_DIR"
        if [ -f "$UPDATE_TAR" ]; then
            echo "$UPDATE_TAR does exist."
            sha256sum "$UPDATE_TAR">"$CALCULATED_HASH"
            if cmp -n 64 "$CALCULATED_HASH" "$UPDATE_HASH"; then
                printf 'UPDATE_HASH "%s" \n is the same as \n CALCULATED_HASH "%s"\n' "$UPDATE_HASH" "$CALCULATED_HASH"
                tar -xvf "$UPDATE_TAR" -C "$BASE_DIR"
                source "$UPDATE_SCRIPT"
            else
                printf 'The file "%s" is different from "%s"\n' "$CALCULATED_HASH" "$UPDATE_HASH"
            
                exit 1
            fi
        else
            echo "$UPDATE_TAR does exist."
        fi

    else
        echo "$UPDATE_TAR_GZ does not exist."
        exit 1
    fi

else
    echo "$UPDATE_GPG does not exist."  
    exit 1
fi

rm "$UPDATE_GPG" "$UPDATE_TAR" "$UPDATE_TAR_GZ" 
rm "$CALCULATED_HASH" "$UPDATE_HASH" "$UPDATE_SCRIPT"
exit 0

