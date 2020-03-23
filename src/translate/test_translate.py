#! /usr/bin/env python
# -*- coding: utf-8 -*-
from translate import translate_from_strings
import os
import subprocess
import tempfile

def test_translate_from_strings():
    '''Check whether translate from strings does the same as the main of translate.py'''
    this_dir = os.path.dirname(__file__)
    test_domain_path = os.path.join(this_dir, 'test1-domain.pddl')
    test_problem_path = os.path.join(this_dir, 'test1-problem.pddl')

    # First we call translate.py manually.
    temp_dir = tempfile.mkdtemp()
    sas_cli_path = os.path.join(temp_dir, 'test1-cli.sas')
    sas_module_path = os.path.join(temp_dir, 'test1-module.sas')

    translate_dir = this_dir
    assert(translate_dir == this_dir)
    translate_py = os.path.join(translate_dir, 'translate.py')
    subprocess.check_call(
        ['python3', translate_py, test_domain_path, test_problem_path, '--sas-file', sas_cli_path])
    
    # Then we do the same using the imported module.
    with open(test_domain_path, 'r') as test_domain_file:
        with open(test_problem_path, 'r') as test_problem_file:
            sas = translate_from_strings(test_domain_file.read(), test_problem_file.read())
            with open(sas_module_path, 'w') as sas_module_file:
                sas_module_file.write(sas)
            print('Command-line result is in %s' % sas_cli_path)
            print('Module result is in %s' % sas_module_path)

    # Both should output the same SAS data.
    with open(sas_cli_path, 'r') as test_sas_file:
        assert(sas == test_sas_file.read())
    print('Command-line and module results are the same')

if __name__ == "__main__":
    test_translate_from_strings()