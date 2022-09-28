#!/bin/bash

DIR=www/Upload/file_to_upload
if ! [ -d "$DIR" ]; then
    #echo "$DIR don t exist ... Creating the directory."
    rm -rf www/Upload/file_to_upload
    mkdir www/Upload/file_to_upload
fi