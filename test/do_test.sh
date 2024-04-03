#!/bin/bash

project_dir=$(dirname "$(realpath $0)")/..

obj=${project_dir}/build/tape_sort

test_folder=${project_dir}/test

int_tests=${test_folder}/int_tests

answers_folder=${int_tests}/answers
configs_folder=${int_tests}/configs
rightans_folder=${int_tests}/right_answers
int_tests_folder=${int_tests}/tests


bin_test_folder=${test_folder}/bin_tests
bin_tests_folder=${bin_test_folder}/tests
bin_answers_folder=${bin_test_folder}/answers
bin_rightans_folder=${bin_test_folder}/right_answers

test_files_names=( "01" "02" "03" "04" "05" "06" "07" "08" "09" "10")

for i in ${!test_files_names[@]}; do
    echo -n ${test_files_names[$i]}: ""
    time ${obj} ${int_tests_folder}/test_${test_files_names[$i]}.dat ${answers_folder}/ans_${test_files_names[$i]}.dat ${configs_folder}/config_${test_files_names[$i]}.dat
    echo ${test_files_names[$i]} diff:
    diff ${rightans_folder}/ans_${test_files_names[$i]}.dat ${answers_folder}/ans_${test_files_names[$i]}.dat
    echo
done


bin_files_names=( "08" "09" )

for i in ${!bin_files_names[@]}; do
    echo -n ${bin_files_names[$i]}: ""
    time ${obj} -bin ${bin_tests_folder}/test_${bin_files_names[$i]}.bin ${bin_answers_folder}/ans_${bin_files_names[$i]}.bin
    echo ${bin_files_names[$i]} bin diff:
    diff ${bin_rightans_folder}/ans_${bin_files_names[$i]}.bin ${bin_answers_folder}/ans_${bin_files_names[$i]}.bin
    echo
done
